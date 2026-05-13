/*
 * Project-specific lwipopts.h for the adin1140 iperf_lwip_example.
 * Tuned for the MAX78000 (128 KB SRAM).  Overrides the shared
 * libraries/lwip/configs/lwipopts.h via an early -I in CFLAGS.
 */
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

/* NO_SYS==1: Use lwIP without OS-awareness */
#define NO_SYS                          1
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_NETIF_API                  0
#define SYS_LIGHTWEIGHT_PROT            0

#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0

/* ---------- Memory Options ---------- */
#define MEM_ALIGNMENT                   4

/* Increased for iperf performance: the static heap array is the largest .bss consumer.
 * 32 KB is suitable for iperf testing to achieve good throughput. */
#define MEM_SIZE                        32768

#define MEMP_NUM_PBUF                   10
#define MEMP_NUM_RAW_PCB                4
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                4
#define MEMP_NUM_TCP_PCB_LISTEN         2
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define MEMP_NUM_SYS_TIMEOUT            17

/* ---------- PBUF Options ---------- */
#define PBUF_POOL_SIZE                  10

#define LWIP_CHECKSUM_ON_COPY           1

/* ---------- ARP Options ---------- */
/* Reduced from 255: each entry is ~28 bytes; 255 entries = ~7 KB waste. */
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10
#define ARP_QUEUEING                    1
#define ETHARP_TABLE_MATCH_NETIF        1
#define ETHARP_SUPPORT_STATIC_ENTRIES   1

/* ---------- IP Options ---------- */
#define IP_FORWARD                      0
#define IP_REASSEMBLY                   1
#define IP_FRAG                         1
#define IP_REASS_MAX_PBUFS              (5 * ((1500 + PBUF_POOL_BUFSIZE - 1) / PBUF_POOL_BUFSIZE))
#define MEMP_NUM_REASSDATA              IP_REASS_MAX_PBUFS

/* ---------- UDP Options ---------- */
#define LWIP_UDP                        1
#define LWIP_UDPLITE                    LWIP_UDP
#define UDP_TTL                         255

/* ---------- TCP Options ---------- */
#define LWIP_TCP                        1
#define TCP_TTL                         255
#define LWIP_ALTCP                      LWIP_TCP

#define USE_DHCP                        1
#define SO_REUSE                        1

#define TCP_QUEUE_OOSEQ                 0
#define TCP_OOSEQ_MAX_PBUFS             0
#define TCP_OOSEQ_MAX_BYTES             0

#define LWIP_TCP_TIMESTAMPS             1

#define TCP_MSS                         1460
#define TCP_SND_BUF                     4096
#define TCP_SND_QUEUELEN                (4 * TCP_SND_BUF / TCP_MSS)
#define TCP_SNDLOWAT                    (TCP_SND_BUF / 2)
#define TCP_WND                         (4 * TCP_MSS)
#define TCP_OVERSIZE                    TCP_MSS
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   4

/* ---------- ICMP Options ---------- */
#define LWIP_ICMP                       LWIP_IPV4
#define ICMP_TTL                        255

/* ---------- DHCP Options ---------- */
#define LWIP_DHCP                       LWIP_UDP

/* ---------- AUTOIP / MDNS Options ---------- */
#define LWIP_AUTOIP                     1
#define LWIP_DHCP_AUTOIP_COOP           1
#define LWIP_DHCP_AUTOIP_COOP_TRIES     2
#define LWIP_IGMP                       1
#define LWIP_MDNS_RESPONDER             1
/* Reduced from 255: each slot is non-trivial; 4 is enough for basic services. */
#define MDNS_MAX_SERVICES               4
#define LWIP_RAND                       rand

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define MDNS_RESP_USENETIF_EXTCALLBACK  1
#define LWIP_NUM_NETIF_CLIENT_DATA      1

/* ---------- lwiperf Options ---------- */
#define LWIPERF_TCP_MAX_IDLE_SEC        10

/* ---------- RAW Options ---------- */
#define LWIP_RAW                        1

#endif /* LWIP_LWIPOPTS_H */
