/*
 * afe_sync.c - Synchronized ADE9430 start via UDP + PTP 1PPS
 *
 * Both boards boot normally with AFE running. After the slave's PTP servo
 * converges and TS_TIMER is realigned, the slave sends AFE_SYNC_READY to the
 * master. The master picks a target PTP second (current + 3s) and sends
 * AFE_SYNC_GO back. Both boards arm their 1PPS handler to restart the AFE
 * at the target second, aligning cycle boundaries to within ZX jitter.
 *
 * If the handshake fails, the AFE continues with its boot-time phase offset.
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/logging/log.h>

#include "afe_sync.h"
#include "afe_config.h"
#include "pqlib_example.h"
#include "ptp_clock_adin1110_hw.h"

LOG_MODULE_REGISTER(afe_sync, LOG_LEVEL_INF);

/* UDP socket */
static int sync_sock = -1;

/* Peer address — slave only: the master to send READY to. */
static struct sockaddr_in peer_addr;

/* Role */
static bool sync_is_master;

/*
 * Master: dynamic slave list (Option A — settling window).
 *
 * The first AFE_SYNC_READY opens a 10s settling window. Every distinct slave
 * IP that sends READY during the window is added to the list. When the window
 * closes, the master picks a target PTP second and unicasts GO to every slave.
 * A single-slave setup behaves identically to the original 2-board flow.
 */
#define MAX_SLAVES                 8
#define SETTLING_WINDOW_MS         10000

static struct sockaddr_in slave_addr[MAX_SLAVES];
static int slave_count;
static struct k_work_delayable settling_work;
static volatile bool settling_open;   /* Window collecting slaves */
static struct k_mutex slave_lock;

/*
 * GO burst (master only) — non-blocking. A delayable work emits one GO round
 * to every slave per fire and reschedules itself GO_RETRY_INTERVAL_MS later
 * until GO_RETRY_COUNT rounds are sent. No thread ever sleeps for the burst,
 * so the wf_uart consumer is never starved (the old blocking 5×k_msleep loop
 * in the afe_sync listener thread / sysworkq was the source of recurring
 * waveform drops).
 */
static struct k_work_delayable go_burst_work;
static volatile uint32_t go_burst_target;
static volatile int go_burst_round;

/* State */
static volatile bool sync_initialized;
static volatile bool sync_go_received;   /* Slave: got GO from master */
static volatile bool sync_complete;      /* AFE restart done */
static volatile bool ready_sending;      /* Slave: READY loop active */

/* READY retry state (slave only) — retries indefinitely until GO received */
static struct k_work_delayable ready_retry_work;
#define READY_RETRY_INTERVAL_MS  2000

/* GO burst (master only) — send GO multiple times to handle packet loss */
#define GO_RETRY_COUNT           5
#define GO_RETRY_INTERVAL_MS     100

/* AFE sync arm — checked by 1PPS handler in ptp_clock_adin1110_zephyr.c */
static volatile bool afe_sync_armed;
static volatile uint32_t afe_sync_target_sec;

/* Listener thread */
#define SYNC_THREAD_STACK_SIZE  2048
K_THREAD_STACK_DEFINE(sync_thread_stack, SYNC_THREAD_STACK_SIZE);
static struct k_thread sync_thread_data;

/* Forward declarations */
static void sync_listener_fn(void *p1, void *p2, void *p3);
static void ready_retry_handler(struct k_work *work);
static int send_msg_to(const struct sockaddr_in *dst, uint8_t type,
		       uint32_t target_sec);
static int send_msg(uint8_t type, uint32_t target_sec);
static void arm_afe_restart(uint32_t target_sec);
static void afe_sync_begin_prep(void);
static void settling_handler(struct k_work *work);
static void go_burst_handler(struct k_work *work);

int afe_sync_init(bool is_master, const char *peer_ip)
{
	struct sockaddr_in bind_addr;
	int ret;

	sync_is_master = is_master;
	sync_complete = false;
	sync_go_received = false;
	afe_sync_armed = false;

	/* Create UDP socket */
	sync_sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sync_sock < 0) {
		LOG_ERR("AFE sync: socket create failed: %d", errno);
		return -errno;
	}

	/* Bind to port 9999 */
	memset(&bind_addr, 0, sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(AFE_SYNC_UDP_PORT);
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = zsock_bind(sync_sock, (struct sockaddr *)&bind_addr,
			 sizeof(bind_addr));
	if (ret < 0) {
		LOG_ERR("AFE sync: bind failed: %d", errno);
		zsock_close(sync_sock);
		sync_sock = -1;
		return -errno;
	}

	/* Set receive timeout so thread can check for shutdown */
	struct zsock_timeval tv = { .tv_sec = 2, .tv_usec = 0 };
	zsock_setsockopt(sync_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	/* Slave: set up the master peer address. Master discovers slaves
	 * dynamically, so peer_ip is NULL and unused. */
	memset(&peer_addr, 0, sizeof(peer_addr));
	if (!is_master) {
		peer_addr.sin_family = AF_INET;
		peer_addr.sin_port = htons(AFE_SYNC_UDP_PORT);
		ret = zsock_inet_pton(AF_INET, peer_ip, &peer_addr.sin_addr);
		if (ret != 1) {
			LOG_ERR("AFE sync: invalid peer IP: %s", peer_ip);
			zsock_close(sync_sock);
			sync_sock = -1;
			return -EINVAL;
		}
	}

	ready_sending = false;

	if (!is_master) {
		k_work_init_delayable(&ready_retry_work, ready_retry_handler);
	} else {
		/* Reset slave list for a clean re-sync after reboot. */
		k_mutex_init(&slave_lock);
		k_work_init_delayable(&settling_work, settling_handler);
		k_work_init_delayable(&go_burst_work, go_burst_handler);
		slave_count = 0;
		settling_open = false;
		memset(slave_addr, 0, sizeof(slave_addr));
	}

	/* Start listener thread */
	k_thread_create(&sync_thread_data, sync_thread_stack,
			K_THREAD_STACK_SIZEOF(sync_thread_stack),
			sync_listener_fn, NULL, NULL, NULL,
			K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
	k_thread_name_set(&sync_thread_data, "afe_sync");

	sync_initialized = true;
	LOG_INF("AFE sync: initialized as %s, peer=%s",
		is_master ? "MASTER" : "SLAVE",
		is_master ? "(dynamic)" : peer_ip);
	return 0;
}

static int send_msg_to(const struct sockaddr_in *dst, uint8_t type,
		       uint32_t target_sec)
{
	struct afe_sync_msg msg = {
		.magic = AFE_SYNC_MAGIC,
		.type = type,
		.target_sec = target_sec,
	};

	int ret = zsock_sendto(sync_sock, &msg, sizeof(msg), 0,
			       (const struct sockaddr *)dst,
			       sizeof(*dst));
	if (ret < 0) {
		LOG_ERR("AFE sync: sendto failed: %d", errno);
		return -errno;
	}
	return 0;
}

/* Slave helper: send to the master peer. */
static int send_msg(uint8_t type, uint32_t target_sec)
{
	return send_msg_to(&peer_addr, type, target_sec);
}

void afe_sync_send_ready(void)
{
	if (!sync_initialized || sync_is_master || sync_complete) {
		return;
	}

	if (ready_sending) {
		return;
	}

	ready_sending = true;
	LOG_INF("AFE sync: sending READY to master");
	send_msg(AFE_SYNC_READY, 0);

	k_work_schedule(&ready_retry_work, K_MSEC(READY_RETRY_INTERVAL_MS));
}

static void ready_retry_handler(struct k_work *work)
{
	if (sync_go_received || sync_complete) {
		ready_sending = false;
		return;
	}

	send_msg(AFE_SYNC_READY, 0);
	k_work_schedule(&ready_retry_work, K_MSEC(READY_RETRY_INTERVAL_MS));
}

static void arm_afe_restart(uint32_t target_sec)
{
	afe_sync_target_sec = target_sec;
	afe_sync_armed = true;

	/* Start Phase 1: AFE thread will do config_measurement(),
	 * then arm the 1PPS handler for Phase 2 (afe_start). */
	afe_sync_begin_prep();

	LOG_INF("AFE sync: armed for PTP sec %u", target_sec);
}

/*
 * Master: GO burst worker. Emits one GO round to every known slave, then
 * reschedules itself until GO_RETRY_COUNT rounds have been sent. Runs on the
 * system workqueue but never sleeps — each fire does a few non-blocking
 * sendto()s and returns, so no thread is held at any priority. This avoids
 * starving the wf_uart consumer the way the old 5×k_msleep(100) loop did.
 */
static void go_burst_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	uint32_t target = go_burst_target;

	k_mutex_lock(&slave_lock, K_FOREVER);
	int n = slave_count;
	struct sockaddr_in dst[MAX_SLAVES];
	memcpy(dst, slave_addr, sizeof(dst));
	k_mutex_unlock(&slave_lock);

	for (int i = 0; i < n; i++) {
		send_msg_to(&dst[i], AFE_SYNC_GO, target);
	}

	if (++go_burst_round < GO_RETRY_COUNT) {
		k_work_schedule(&go_burst_work, K_MSEC(GO_RETRY_INTERVAL_MS));
	}
}

/*
 * Master: start (or restart) a non-blocking GO burst to all known slaves.
 * Sends the first round immediately and schedules the rest. Safe to call from
 * the settling-window close and from the missed-GO re-burst path; a fresh call
 * simply resets the round counter so every current slave gets the full burst.
 */
static void start_go_burst(uint32_t target)
{
	go_burst_target = target;
	go_burst_round = 0;
	k_work_reschedule(&go_burst_work, K_NO_WAIT);
}

/* Master: add a slave IP to the list if not already present.
 * Returns true if newly added. Caller must hold slave_lock. */
static bool slave_list_add(const struct sockaddr_in *src)
{
	for (int i = 0; i < slave_count; i++) {
		if (slave_addr[i].sin_addr.s_addr == src->sin_addr.s_addr) {
			return false;  /* Duplicate */
		}
	}

	if (slave_count >= MAX_SLAVES) {
		LOG_WRN("AFE sync: slave list full, ignoring extra slave");
		return false;
	}

	slave_addr[slave_count] = *src;
	slave_addr[slave_count].sin_port = htons(AFE_SYNC_UDP_PORT);
	slave_count++;
	return true;
}

/*
 * Settling window expired: pick a target PTP second and GO to all slaves.
 * Runs in the system workqueue.
 */
static void settling_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	struct net_ptp_time capt;
	int ret = adin1110_ptp_capture_timestamp(&capt);
	if (ret < 0) {
		LOG_ERR("AFE sync: failed to read PTP time: %d", ret);
		settling_open = false;
		return;
	}

	/* +3s leaves >2s margin for the GO bursts to reach every slave. */
	uint32_t target = (uint32_t)capt.second + 3;

	LOG_INF("AFE sync: window closed, %d slave(s), GO at PTP sec %u "
		"(now=%u.%09u)", slave_count, target,
		(uint32_t)capt.second, capt.nanosecond);

	settling_open = false;

	sync_complete = false;
	arm_afe_restart(target);

	/* Non-blocking GO burst to every slave in the list. */
	start_go_burst(target);
}

static void handle_master_rx(struct afe_sync_msg *msg,
			     struct sockaddr_in *src)
{
	if (msg->type != AFE_SYNC_READY) {
		return;
	}

	/* If already armed (window closed, target chosen), this slave either
	 * joined late or missed its GO — re-send GO immediately. Also make
	 * sure it is in the list for any future re-burst. */
	if (afe_sync_armed) {
		k_mutex_lock(&slave_lock, K_FOREVER);
		slave_list_add(src);
		k_mutex_unlock(&slave_lock);

		/* Non-blocking re-burst. start_go_burst() sends to the whole
		 * list, so the late/missed-GO slave (now in the list) is
		 * covered along with everyone else. */
		start_go_burst(afe_sync_target_sec);
		return;
	}

	k_mutex_lock(&slave_lock, K_FOREVER);
	bool added = slave_list_add(src);
	bool first = !settling_open;
	k_mutex_unlock(&slave_lock);

	char ip[NET_IPV4_ADDR_LEN];
	net_addr_ntop(AF_INET, &src->sin_addr, ip, sizeof(ip));

	if (first) {
		/* First READY opens the settling window. */
		settling_open = true;
		LOG_INF("AFE sync: READY from %s — opening %dms settling window",
			ip, SETTLING_WINDOW_MS);
		k_work_schedule(&settling_work, K_MSEC(SETTLING_WINDOW_MS));
	} else if (added) {
		LOG_INF("AFE sync: READY from %s — added to window (%d slaves)",
			ip, slave_count);
	}
	/* Duplicate READY during the window: ignore, window already running. */
}

static void handle_slave_rx(struct afe_sync_msg *msg)
{
	if (msg->type != AFE_SYNC_GO) {
		return;
	}

	if (sync_go_received) {
		return;
	}

	sync_go_received = true;
	uint32_t target = msg->target_sec;

	LOG_INF("AFE sync: received GO, target PTP sec %u", target);

	/* Arm our 1PPS handler */
	arm_afe_restart(target);
}

static void sync_listener_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	struct afe_sync_msg msg;
	struct sockaddr_in src_addr;
	socklen_t src_len;

	LOG_DBG("AFE sync: listener thread started");

	while (1) {
		src_len = sizeof(src_addr);
		int ret = zsock_recvfrom(sync_sock, &msg, sizeof(msg), 0,
					(struct sockaddr *)&src_addr,
					&src_len);
		if (ret < 0) {
			if (errno == EAGAIN || errno == ETIMEDOUT) {
				continue;  /* Timeout, loop again */
			}
			LOG_ERR("AFE sync: recvfrom error: %d", errno);
			continue;
		}

		if (ret < (int)sizeof(msg)) {
			continue;  /* Short packet */
		}

		if (msg.magic != AFE_SYNC_MAGIC) {
			continue;  /* Not our protocol */
		}

		if (sync_is_master) {
			handle_master_rx(&msg, &src_addr);
		} else {
			handle_slave_rx(&msg);
		}
	}

	LOG_DBG("AFE sync: listener thread exiting");
}

bool afe_sync_is_complete(void)
{
	return sync_complete;
}

/*
 * Two-phase synchronized restart:
 *
 * Phase 1 (AFE thread, BEFORE target second):
 *   config_measurement() → adi_pqlib_Reset() + SetConfig
 *   This reinitializes PQLib cleanly — same as boot.
 *   Happens a few seconds before the sync point.
 *
 * Phase 2 (1PPS handler, AT target second):
 *   afe_start() → RUN=0 → clear STATUS0 → RUN=1
 *   Both boards execute within <100ns of each other.
 *
 * Phase 3 (AFE thread, AFTER afe_start):
 *   afe_wait_settling() → consume first 50 noisy cycles
 *   Then normal processing resumes.
 *
 * This matches boot order (config first, then start) and gives
 * precise synchronization via the 1PPS handler.
 */

/* Phase tracking */
enum afe_sync_phase {
	SYNC_PHASE_IDLE,
	SYNC_PHASE_PREP,         /* AFE thread: do config_measurement */
	SYNC_PHASE_WAITING_1PPS, /* Waiting for 1PPS at target second */
	SYNC_PHASE_SETTLE,       /* AFE thread: afe_wait_settling */
};
static volatile enum afe_sync_phase sync_phase = SYNC_PHASE_IDLE;

/*
 * Called from arm_afe_restart() after UDP handshake completes.
 * Signals the AFE thread to start Phase 1 (config_measurement).
 */
static void afe_sync_begin_prep(void)
{
	sync_phase = SYNC_PHASE_PREP;
}

/*
 * Called from the 1PPS COOP(0) thread at the target second.
 * Phase 2: calls afe_start() for precise cross-board alignment.
 */
static void afe_sync_do_restart(void)
{
	LOG_INF("AFE sync: afe_start() at target sec %u",
		afe_sync_target_sec);

	int ret = afe_start();
	if (ret != 0) {
		LOG_ERR("AFE sync: afe_start() failed: %d", ret);
	}

	afe_sync_armed = false;
	sync_phase = SYNC_PHASE_SETTLE;
}

/*
 * Called from the AFE thread (pqm_one_cycle path) every iteration.
 * Handles Phase 1 (prep) and Phase 3 (settle).
 * Returns true if a phase was active (caller should skip normal processing).
 */
bool afe_sync_check_restart(void)
{
	if (sync_phase == SYNC_PHASE_PREP) {
		/* Phase 1: reinitialize PQLib BEFORE the synchronized start.
		 * config_measurement() does Reset + SetConfig — same as boot.
		 * This happens seconds before target_sec, so no time pressure. */
		LOG_DBG("AFE sync: Phase 1 — config_measurement()");

		pqlibExample.processedCycles = 0;
		pqlibExample.zeroCrossingCount = 0;
		pqlibExample.timeOutCount = 0;
		pqlibExample.inputCycle.sequenceNumber = 0;
		pqlibExample.inputWaveform.sequenceNumber = 0;
		pqlibExample.input1012Cycles.sequenceNumber = 0;
		pqlibExample.input1012Cycles.isDataProcessed = 1u;
		pqlibExample.inputCycle.isDataProcessed = 1u;
		pqlibExample.inputWaveform.isDataProcessed = true;

		int ret = config_measurement(&pqlibExample);
		if (ret != 0) {
			LOG_ERR("AFE sync: config_measurement failed: %d", ret);
		}

		/* Now arm the 1PPS handler for Phase 2 */
		adin1110_ptp_arm_afe_sync();
		sync_phase = SYNC_PHASE_WAITING_1PPS;

		LOG_DBG("AFE sync: Phase 1 done, waiting for 1PPS at sec %u",
			afe_sync_target_sec);
		return true;
	}

	if (sync_phase == SYNC_PHASE_SETTLE) {
		/* Phase 3: consume settling cycles after afe_start().
		 * The AFE was restarted by the 1PPS handler (Phase 2). */
		LOG_INF("AFE sync: Phase 3 — settling");

		afe_wait_settling(pqlibExample.exampleConfig.nominalFrequency);
		pqlibExample.readyToDisplay = false;
		pqlibExample.state = PQLIB_STATE_WAITING_FOR_TRIGGER;

		sync_complete = true;
		sync_phase = SYNC_PHASE_IDLE;
		LOG_INF("AFE sync: restart complete — cycle boundaries aligned");
		return true;
	}

	/* WAITING_1PPS: do nothing, let 1PPS handler trigger Phase 2 */
	if (sync_phase == SYNC_PHASE_WAITING_1PPS) {
		return true;  /* Skip normal processing while waiting */
	}

	return false;
}

/*
 * Check if AFE sync restart should fire on this 1PPS edge.
 * Called from rtc_sync_thread_fn with the captured PTP second.
 * Returns true if the restart was triggered (caller should NOT
 * do any other processing on this 1PPS edge).
 */
bool afe_sync_check_1pps(uint32_t ptp_sec)
{
	if (!afe_sync_armed) {
		return false;
	}

	if (ptp_sec == afe_sync_target_sec) {
		afe_sync_do_restart();
		return true;
	}

	/* Log countdown */
	if (ptp_sec < afe_sync_target_sec) {
		LOG_DBG("AFE sync: %u seconds until restart",
			afe_sync_target_sec - ptp_sec);
	} else if (ptp_sec > afe_sync_target_sec) {
		/* Missed it — shouldn't happen unless PTP time jumped */
		LOG_WRN("AFE sync: missed target sec %u (now %u), disarming",
			afe_sync_target_sec, ptp_sec);
		afe_sync_armed = false;
	}

	return false;
}
