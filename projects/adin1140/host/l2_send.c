// SPDX-License-Identifier: MIT
/*
 * l2_send - blast raw Ethernet frames of a fixed EtherType out an interface as
 * fast as the link allows, then report the layer-2 send rate. Pairs with the
 * ADIN1140 frame_blast example's "RX count" mode.
 *
 * Build:  cc -O2 -o l2_send l2_send.c
 * Run:    sudo ./l2_send <interface> [seconds]
 *   e.g.  sudo ./l2_send eth0 10
 *
 * Frames are 1514 bytes (14-byte header + 1500 payload), broadcast dest, so the
 * board receives them in promiscuous/broadcast mode and filters on EtherType.
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
#define FRAME_LEN       1514            /* 14 header + 1500 payload */

static double now_s(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char **argv)
{
	const char *ifname;
	int seconds = 10;
	int fd, ifindex;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	unsigned char frame[FRAME_LEN];
	unsigned char src[6];
	unsigned long long frames = 0, l2_bytes = 0;
	double start, elapsed;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <interface> [seconds]\n", argv[0]);
		return 1;
	}
	ifname = argv[1];
	if (argc >= 3)
		seconds = atoi(argv[2]);

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

	/* Fetch our own MAC for the source address. */
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
		perror("SIOCGIFHWADDR");
		return 1;
	}
	memcpy(src, ifr.ifr_hwaddr.sa_data, 6);

	/* Build the frame: dest = broadcast, src = our MAC, our EtherType. */
	memset(frame, 0xFF, 6);                 /* dest broadcast */
	memcpy(frame + 6, src, 6);              /* source */
	frame[12] = (BLAST_ETHERTYPE >> 8) & 0xFF;
	frame[13] = BLAST_ETHERTYPE & 0xFF;
	memset(frame + 14, 0xA5, FRAME_LEN - 14);

	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_protocol = htons(BLAST_ETHERTYPE);
	sll.sll_ifindex = ifindex;
	sll.sll_halen = 6;
	memset(sll.sll_addr, 0xFF, 6);

	printf("Blasting %d-byte frames on %s (EtherType 0x%04X) for %d s...\n",
	       FRAME_LEN, ifname, BLAST_ETHERTYPE, seconds);

	start = now_s();
	while ((elapsed = now_s() - start) < seconds) {
		ssize_t n = sendto(fd, frame, FRAME_LEN, 0,
				   (struct sockaddr *)&sll, sizeof(sll));
		if (n < 0) {
			if (errno == ENOBUFS || errno == EAGAIN)
				continue; /* TX ring full; retry */
			perror("sendto");
			break;
		}
		frames++;
		l2_bytes += n;
	}

	if (elapsed <= 0)
		elapsed = 1e-3;
	printf("Sent %llu frames, %llu L2 bytes in %.3f s\n",
	       frames, l2_bytes, elapsed);
	printf("  L2 send rate: %.3f Mbit/s\n",
	       (l2_bytes * 8.0) / elapsed / 1e6);

	close(fd);
	return 0;
}
