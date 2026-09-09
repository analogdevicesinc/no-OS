// SPDX-License-Identifier: MIT
/*
 * l2_recv - count raw Ethernet frames of a given EtherType on an interface and
 * report the layer-2 throughput. Pairs with the ADIN1140 frame_blast example's
 * "TX blast" mode.
 *
 * Build:  cc -O2 -o l2_recv l2_recv.c
 * Run:    sudo ./l2_recv <interface>
 *   e.g.  sudo ./l2_recv eth0
 *
 * Measures over the active window (first frame -> last frame) so idle time
 * before the board starts blasting does not dilute the result.
 *
 * The idle gap that marks the end of a burst is a compile-time constant
 * (IDLE_GAP_S); change it here and rebuild if needed.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define BLAST_ETHERTYPE 0x88B5

/* Seconds of silence that mark the end of a burst. Compile-time only: long
 * enough to ride over inter-frame gaps within a blast, short enough to close
 * the window promptly once the board stops. */
#define IDLE_GAP_S 1.0

static double now_s(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char **argv)
{
	const char *ifname;
	int fd, ifindex;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	unsigned char buf[2048];

	if (argc < 2) {
		fprintf(stderr, "usage: %s <interface>\n", argv[0]);
		fprintf(stderr, "  Runs forever. Reports one result per burst: counting\n");
		fprintf(stderr, "  starts at the first frame and ends after %.3f s with no\n",
			(double)IDLE_GAP_S);
		fprintf(stderr, "  frames. Ctrl-C to quit.\n");
		return 1;
	}
	ifname = argv[1];

	/* Only receive our experimental EtherType. */
	fd = socket(AF_PACKET, SOCK_RAW, htons(BLAST_ETHERTYPE));
	if (fd < 0) {
		perror("socket (need root/CAP_NET_RAW)");
		return 1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		return 1;
	}
	ifindex = ifr.ifr_ifindex;

	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_protocol = htons(BLAST_ETHERTYPE);
	sll.sll_ifindex = ifindex;
	if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
		perror("bind");
		return 1;
	}

	printf("Listening on %s for EtherType 0x%04X (idle gap %.3f s). "
	       "Ctrl-C to quit.\n", ifname, BLAST_ETHERTYPE, (double)IDLE_GAP_S);

	/* Run forever. Each iteration of the outer loop measures one burst: the
	 * active window opens at the first frame and closes once no frame has
	 * arrived for IDLE_GAP_S seconds (the transmitter stopped). Idle time between
	 * bursts is never counted. */
	while (1) {
		unsigned long long frames = 0, l2_bytes = 0;
		double first = 0, last = 0;
		int started = 0;

		while (1) {
			ssize_t n = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
			if (n < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					/* No frame right now. If we were mid-burst and the
					 * silence exceeds IDLE_GAP_S, the burst has ended. */
					if (started && (now_s() - last) >= IDLE_GAP_S)
						break;
					continue;
				}
				perror("recv");
				close(fd);
				return 1;
			}
			if (!started) {
				started = 1;
				first = now_s();
			}
			last = now_s();
			frames++;
			/* n is the L2 frame length (header + payload), excluding FCS. */
			l2_bytes += n;
		}

		/* Active window = first frame -> last frame (excludes the trailing
		 * idle gap used only to detect the end). */
		double active = last - first;
		if (active <= 0)
			active = 1e-3;
		double mbps = (l2_bytes * 8.0) / active / 1e6;
		printf("burst: %llu frames, %llu L2 bytes in %.3f s -> "
		       "%.3f Mbit/s\n", frames, l2_bytes, active, mbps);
	}

	close(fd);
	return 0;
}
