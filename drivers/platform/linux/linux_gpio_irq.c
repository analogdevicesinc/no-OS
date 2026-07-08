/***************************************************************************//**
 *   @file   linux_gpio_irq.c
 *   @brief  Implementation of Linux platform GPIO IRQ driver using the GPIO
 *           character device (/dev/gpiochipN). No libgpiod dependency.
 *   @author Villy Jay Tolentino 
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* Linux / libc dependencies (implementation-only). */
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

/* no-OS portable dependencies. */
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/* This driver's public API. */
#include "linux_gpio_irq.h"

/* -------------------------------------------------------------------------- */
/* Private types                                                              */
/* -------------------------------------------------------------------------- */

/** Maximum number of lines we can manage per controller. Bump if needed. */
#define LINUX_GPIO_IRQ_MAX_LINES	32

/**
 * @struct linux_gpio_irq_line
 * @brief  Per-line (per-pin) interrupt bookkeeping.
 */
struct linux_gpio_irq_line {
	/** GPIO line offset (pin number); used as irq_id. */
	uint32_t offset;
	/** Line event request file descriptor (-1 if unused). */
	int line_fd;
	/** Requested trigger edge(s). */
	enum no_os_irq_trig_level trig;
	/** Whether this line is currently enabled. */
	int enabled;
	/** Registered user callback. */
	struct no_os_callback_desc cb;
	/** Set when a callback has been registered. */
	int registered;
	/** Last latched level from the most recent edge (1 = HIGH, 0 = LOW). */
	uint8_t last_level;
};

/**
 * @struct linux_gpio_irq_ctrl
 * @brief  Linux GPIO IRQ controller runtime state.
 */
struct linux_gpio_irq_ctrl {
	/** GPIO chip number. */
	int chip;
	/** Managed lines. */
	struct linux_gpio_irq_line lines[LINUX_GPIO_IRQ_MAX_LINES];
	/** Number of lines in use. */
	unsigned int num_lines;
	/** Worker thread that polls for edge events. */
	pthread_t thread;
	/** Mutex protecting the lines array. */
	pthread_mutex_t lock;
	/** Self-pipe used to wake/stop the poll thread. */
	int wake_fd[2];
	/** Flag telling the worker thread to keep running. */
	volatile int running;
	/** Whether the worker thread has been started. */
	int thread_started;
};

/* -------------------------------------------------------------------------- */
/* Private helper prototypes                                                  */
/* -------------------------------------------------------------------------- */

static uint64_t linux_gpio_irq_edge_flags(enum no_os_irq_trig_level trig);
static struct linux_gpio_irq_line *linux_gpio_irq_find(
	struct linux_gpio_irq_ctrl *ctrl, uint32_t offset);
static struct linux_gpio_irq_line *linux_gpio_irq_get_line(
	struct linux_gpio_irq_ctrl *ctrl, uint32_t offset);
static int  linux_gpio_irq_request_line(struct linux_gpio_irq_ctrl *ctrl,
					struct linux_gpio_irq_line *line);
static void linux_gpio_irq_wake(struct linux_gpio_irq_ctrl *ctrl);
static void *linux_gpio_irq_worker(void *arg);
static int  linux_gpio_irq_start_thread(struct linux_gpio_irq_ctrl *ctrl);

/* -------------------------------------------------------------------------- */
/* no-OS IRQ ops prototypes                                                   */
/* -------------------------------------------------------------------------- */

static int linux_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				    const struct no_os_irq_init_param *param);
static int linux_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *cb);
static int linux_gpio_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *cb);
static int linux_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, enum no_os_irq_trig_level trig);
static int linux_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc,
				 uint32_t irq_id);
static int linux_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id);
static int linux_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc);

/* -------------------------------------------------------------------------- */
/* Private helper definitions                                                 */
/* -------------------------------------------------------------------------- */

/* Translate a no-OS trigger level into GPIO v2 edge flags. */
static uint64_t linux_gpio_irq_edge_flags(enum no_os_irq_trig_level trig)
{
	switch (trig) {
	case NO_OS_IRQ_EDGE_RISING:
		return GPIO_V2_LINE_FLAG_EDGE_RISING;
	case NO_OS_IRQ_EDGE_FALLING:
		return GPIO_V2_LINE_FLAG_EDGE_FALLING;
	case NO_OS_IRQ_EDGE_BOTH:
	default:
		return GPIO_V2_LINE_FLAG_EDGE_RISING |
		       GPIO_V2_LINE_FLAG_EDGE_FALLING;
	}
}

/* Find a managed line by its offset. Caller must hold the lock. */
static struct linux_gpio_irq_line *linux_gpio_irq_find(
	struct linux_gpio_irq_ctrl *ctrl, uint32_t offset)
{
	unsigned int i;

	for (i = 0; i < ctrl->num_lines; i++) {
		if (ctrl->lines[i].offset == offset)
			return &ctrl->lines[i];
	}
	return NULL;
}

/* Get-or-create a managed line by offset. Caller must hold the lock. */
static struct linux_gpio_irq_line *linux_gpio_irq_get_line(
	struct linux_gpio_irq_ctrl *ctrl, uint32_t offset)
{
	struct linux_gpio_irq_line *line;

	line = linux_gpio_irq_find(ctrl, offset);
	if (line)
		return line;

	if (ctrl->num_lines >= LINUX_GPIO_IRQ_MAX_LINES)
		return NULL;

	line = &ctrl->lines[ctrl->num_lines++];
	line->offset = offset;
	line->line_fd = -1;
	line->enabled = 0;
	line->registered = 0;
	line->last_level = 0;
	return line;
}

/* (Re)request a line as an edge-event input on /dev/gpiochipN. */
static int linux_gpio_irq_request_line(struct linux_gpio_irq_ctrl *ctrl,
				       struct linux_gpio_irq_line *line)
{
	struct gpio_v2_line_request req = {0};
	char path[64];
	int chip_fd;
	int ret;

	if (line->line_fd >= 0) {
		close(line->line_fd);
		line->line_fd = -1;
	}

	snprintf(path, sizeof(path), "/dev/gpiochip%d", ctrl->chip);
	chip_fd = open(path, O_RDONLY | O_CLOEXEC);
	if (chip_fd < 0) {
		pr_err("Can't open %s\n", path);
		return -errno;
	}

	req.offsets[0] = line->offset;
	req.num_lines = 1;
	req.config.flags = GPIO_V2_LINE_FLAG_INPUT |
			   linux_gpio_irq_edge_flags(line->trig);
	strncpy(req.consumer, "no_os_gpio_irq", sizeof(req.consumer) - 1);

	ret = ioctl(chip_fd, GPIO_V2_GET_LINE_IOCTL, &req);
	close(chip_fd);
	if (ret < 0) {
		pr_err("Can't get line %u\n", line->offset);
		return -errno;
	}

	line->line_fd = req.fd;
	return 0;
}

/* Wake the poll thread so it re-evaluates the fd set. */
static void linux_gpio_irq_wake(struct linux_gpio_irq_ctrl *ctrl)
{
	uint8_t b = 1;

	if (ctrl->wake_fd[1] >= 0)
		(void)!write(ctrl->wake_fd[1], &b, 1);
}

/* Worker thread: poll all enabled line fds, dispatch callbacks on events. */
static void *linux_gpio_irq_worker(void *arg)
{
	struct linux_gpio_irq_ctrl *ctrl = arg;
	struct pollfd pfds[LINUX_GPIO_IRQ_MAX_LINES + 1];
	struct linux_gpio_irq_line *map[LINUX_GPIO_IRQ_MAX_LINES + 1];
	struct gpio_v2_line_event event;
	nfds_t nfds;
	unsigned int i;
	int ret;

	while (ctrl->running) {
		nfds = 0;

		/* Slot 0 is always the wake/self-pipe. */
		pfds[nfds].fd = ctrl->wake_fd[0];
		pfds[nfds].events = POLLIN;
		map[nfds] = NULL;
		nfds++;

		pthread_mutex_lock(&ctrl->lock);
		for (i = 0; i < ctrl->num_lines; i++) {
			struct linux_gpio_irq_line *l = &ctrl->lines[i];

			if (l->enabled && l->registered && l->line_fd >= 0) {
				pfds[nfds].fd = l->line_fd;
				pfds[nfds].events = POLLIN;
				map[nfds] = l;
				nfds++;
			}
		}
		pthread_mutex_unlock(&ctrl->lock);

		ret = poll(pfds, nfds, -1);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			break;
		}

		for (i = 0; i < nfds; i++) {
			if (!(pfds[i].revents & POLLIN))
				continue;

			/* Self-pipe: drain and loop to rebuild the fd set. */
			if (map[i] == NULL) {
				uint8_t buf[16];
				(void)!read(ctrl->wake_fd[0], buf, sizeof(buf));
				continue;
			}

			/* Drain all pending events on this line. */
			while (read(map[i]->line_fd, &event,
				    sizeof(event)) == sizeof(event)) {
				struct no_os_callback_desc cb;
				int has_cb;

				pthread_mutex_lock(&ctrl->lock);
				/* Latch the new level from the edge type. */
				map[i]->last_level =
					(event.id ==
					 GPIO_V2_LINE_EVENT_RISING_EDGE) ? 1 : 0;
				cb = map[i]->cb;
				has_cb = map[i]->registered;
				pthread_mutex_unlock(&ctrl->lock);

				if (has_cb && cb.callback)
					cb.callback(cb.ctx);
			}
		}
	}

	return NULL;
}

/* Lazily start the worker thread on first enable. */
static int linux_gpio_irq_start_thread(struct linux_gpio_irq_ctrl *ctrl)
{
	if (ctrl->thread_started)
		return 0;

	ctrl->running = 1;
	if (pthread_create(&ctrl->thread, NULL,
			   linux_gpio_irq_worker, ctrl)) {
		ctrl->running = 0;
		return -errno;
	}
	ctrl->thread_started = 1;
	return 0;
}

/* -------------------------------------------------------------------------- */
/* no-OS IRQ ops definitions                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the Linux GPIO IRQ controller.
 */
static int linux_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				    const struct no_os_irq_init_param *param)
{
	struct linux_gpio_irq_init_param *extra;
	struct no_os_irq_ctrl_desc *descriptor;
	struct linux_gpio_irq_ctrl *ctrl;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ctrl = no_os_calloc(1, sizeof(*ctrl));
	if (!ctrl) {
		no_os_free(descriptor);
		return -ENOMEM;
	}

	extra = param->extra;
	ctrl->chip = extra ? extra->chip : 0;
	ctrl->num_lines = 0;
	ctrl->running = 0;
	ctrl->thread_started = 0;
	ctrl->wake_fd[0] = -1;
	ctrl->wake_fd[1] = -1;
	pthread_mutex_init(&ctrl->lock, NULL);

	if (pipe(ctrl->wake_fd) < 0) {
		pthread_mutex_destroy(&ctrl->lock);
		no_os_free(ctrl);
		no_os_free(descriptor);
		return -errno;
	}

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = ctrl;

	*desc = descriptor;
	return 0;
}

/**
 * @brief Register a callback for a GPIO line (irq_id == line offset).
 */
static int linux_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *cb)
{
	struct linux_gpio_irq_ctrl *ctrl;
	struct linux_gpio_irq_line *line;
	int ret = 0;

	if (!desc || !cb)
		return -EINVAL;

	ctrl = desc->extra;

	pthread_mutex_lock(&ctrl->lock);
	line = linux_gpio_irq_get_line(ctrl, irq_id);
	if (!line) {
		ret = -ENOMEM;
		goto unlock;
	}

	line->cb = *cb;
	line->registered = 1;

	/* Default to both edges until the user sets a specific trigger. */
	if (line->line_fd < 0) {
		line->trig = NO_OS_IRQ_EDGE_BOTH;
		ret = linux_gpio_irq_request_line(ctrl, line);
	}

unlock:
	pthread_mutex_unlock(&ctrl->lock);
	return ret;
}

/**
 * @brief Unregister a callback for a GPIO line.
 */
static int linux_gpio_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *cb)
{
	struct linux_gpio_irq_ctrl *ctrl;
	struct linux_gpio_irq_line *line;

	(void)cb;
	if (!desc)
		return -EINVAL;

	ctrl = desc->extra;

	pthread_mutex_lock(&ctrl->lock);
	line = linux_gpio_irq_find(ctrl, irq_id);
	if (line) {
		line->registered = 0;
		line->enabled = 0;
		if (line->line_fd >= 0) {
			close(line->line_fd);
			line->line_fd = -1;
		}
	}
	pthread_mutex_unlock(&ctrl->lock);

	linux_gpio_irq_wake(ctrl);
	return 0;
}

/**
 * @brief Set the trigger edge for a GPIO line.
 */
static int linux_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, enum no_os_irq_trig_level trig)
{
	struct linux_gpio_irq_ctrl *ctrl;
	struct linux_gpio_irq_line *line;
	int ret = 0;

	if (!desc)
		return -EINVAL;

	ctrl = desc->extra;

	pthread_mutex_lock(&ctrl->lock);
	line = linux_gpio_irq_get_line(ctrl, irq_id);
	if (!line) {
		ret = -ENOMEM;
		goto unlock;
	}

	line->trig = trig;
	/* Re-request the line so the new edge flags take effect. */
	ret = linux_gpio_irq_request_line(ctrl, line);

unlock:
	pthread_mutex_unlock(&ctrl->lock);
	linux_gpio_irq_wake(ctrl);
	return ret;
}

/**
 * @brief Enable interrupts for a GPIO line.
 */
static int linux_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc,
				 uint32_t irq_id)
{
	struct linux_gpio_irq_ctrl *ctrl;
	struct linux_gpio_irq_line *line;
	int ret = 0;

	if (!desc)
		return -EINVAL;

	ctrl = desc->extra;

	pthread_mutex_lock(&ctrl->lock);
	line = linux_gpio_irq_find(ctrl, irq_id);
	if (!line || !line->registered) {
		ret = -EINVAL;
		goto unlock;
	}

	if (line->line_fd < 0)
		ret = linux_gpio_irq_request_line(ctrl, line);
	if (!ret)
		line->enabled = 1;

	if (!ret)
		ret = linux_gpio_irq_start_thread(ctrl);

unlock:
	pthread_mutex_unlock(&ctrl->lock);
	linux_gpio_irq_wake(ctrl);
	return ret;
}

/**
 * @brief Disable interrupts for a GPIO line.
 */
static int linux_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id)
{
	struct linux_gpio_irq_ctrl *ctrl;
	struct linux_gpio_irq_line *line;

	if (!desc)
		return -EINVAL;

	ctrl = desc->extra;

	pthread_mutex_lock(&ctrl->lock);
	line = linux_gpio_irq_find(ctrl, irq_id);
	if (line)
		line->enabled = 0;
	pthread_mutex_unlock(&ctrl->lock);

	linux_gpio_irq_wake(ctrl);
	return 0;
}

/**
 * @brief Tear down the controller and stop the worker thread.
 */
static int linux_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct linux_gpio_irq_ctrl *ctrl;
	unsigned int i;

	if (!desc)
		return -EINVAL;

	ctrl = desc->extra;
	if (ctrl) {
		ctrl->running = 0;
		linux_gpio_irq_wake(ctrl);
		if (ctrl->thread_started)
			pthread_join(ctrl->thread, NULL);

		for (i = 0; i < ctrl->num_lines; i++) {
			if (ctrl->lines[i].line_fd >= 0)
				close(ctrl->lines[i].line_fd);
		}
		if (ctrl->wake_fd[0] >= 0)
			close(ctrl->wake_fd[0]);
		if (ctrl->wake_fd[1] >= 0)
			close(ctrl->wake_fd[1]);
		pthread_mutex_destroy(&ctrl->lock);
		no_os_free(ctrl);
	}

	no_os_free(desc);
	return 0;
}

/* -------------------------------------------------------------------------- */
/* Public non-ops helper                                                      */
/* -------------------------------------------------------------------------- */

/**
 * @brief Get the last latched level for a line (1 = HIGH, 0 = LOW).
 */
int linux_gpio_irq_get_value(struct no_os_irq_ctrl_desc *desc,
			     uint32_t irq_id, uint8_t *value)
{
	struct linux_gpio_irq_ctrl *ctrl;
	struct linux_gpio_irq_line *line;
	int ret = 0;

	if (!desc || !value)
		return -EINVAL;

	ctrl = desc->extra;

	pthread_mutex_lock(&ctrl->lock);
	line = linux_gpio_irq_find(ctrl, irq_id);
	if (line)
		*value = line->last_level;
	else
		ret = -EINVAL;
	pthread_mutex_unlock(&ctrl->lock);

	return ret;
}

/* -------------------------------------------------------------------------- */
/* Public ops table                                                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Linux platform GPIO IRQ ops.
 */
const struct no_os_irq_platform_ops linux_gpio_irq_ops = {
	.init = linux_gpio_irq_ctrl_init,
	.register_callback = linux_gpio_irq_register_callback,
	.unregister_callback = linux_gpio_irq_unregister_callback,
	.trigger_level_set = linux_gpio_irq_trigger_level_set,
	.enable = linux_gpio_irq_enable,
	.disable = linux_gpio_irq_disable,
	.remove = linux_gpio_irq_ctrl_remove,
};