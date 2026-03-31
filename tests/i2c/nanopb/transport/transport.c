#include "transport.h"

#include <pb_encode.h>
#include <pb_decode.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/tcp.h>

struct transport_methodes {
    int (*transport_connect)(transport_conn_t *conn, const char *host, uint16_t port);
    void (*transport_disconnect)(transport_conn_t *conn);
    int (*transport_listen)(transport_conn_t *conn, uint16_t port);
    int (*transport_accept)(transport_conn_t *server, transport_conn_t *client);
    bool (*transport_is_connected)(int fd);
    bool (*transport_write)(int fd, const uint8_t *buf, size_t count);
    bool (*transport_read)(int fd, uint8_t *buf, size_t count);
};

static int tcp_connect(transport_conn_t *conn, const char *host, uint16_t port)
{
    struct sockaddr_in servaddr;
    struct hostent *he;

    if (!conn || !host) {
        return -EINVAL;
    }

    conn->fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(conn->fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    if (conn->fd < 0) {
        return -errno;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &servaddr.sin_addr) <= 0) {
        he = gethostbyname(host);
        if (!he) {
            close(conn->fd);
            conn->fd = -1;
            return -ENOENT;
        }
        memcpy(&servaddr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    if (connect(conn->fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        int err = errno;
        close(conn->fd);
        conn->fd = -1;
        return -err;
    }

    return 0;
}

static void tcp_disconnect(transport_conn_t *conn)
{
    if (conn && conn->fd >= 0) {
        close(conn->fd);
        conn->fd = -1;
    }
}

static int tcp_listen(transport_conn_t *conn, uint16_t port)
{
    struct sockaddr_in servaddr;
    int reuse = 1;

    if (!conn) {
        return -EINVAL;
    }

    conn->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->fd < 0) {
        return -errno;
    }

    setsockopt(conn->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(conn->fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        int err = errno;
        close(conn->fd);
        conn->fd = -1;
        return -err;
    }

    if (listen(conn->fd, 5) != 0) {
        int err = errno;
        close(conn->fd);
        conn->fd = -1;
        return -err;
    }

    return 0;
}

static int tcp_accept(transport_conn_t *server, transport_conn_t *client)
{
    if (!server || !client) {
        return -EINVAL;
    }

    client->fd = accept(server->fd, NULL, NULL);
    if (client->fd < 0) {
        return -errno;
    }

    return 0;
}

static bool tcp_is_connected(int fd) {
    uint8_t dummy;

    ssize_t result = recv(fd, &dummy, 1, MSG_PEEK | MSG_DONTWAIT);

    if (result == 0) {
        return false; // Client closed the socket gracefully
    }
    if (result < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return true; // No data, but socket is still healthy and open
        }
        return false; // Connection reset (ECONNRESET) or other fatal error
    }
    return true; // Data is waiting to be read, so client is definitely connected
}

static bool tcp_write(int fd, const uint8_t *buf, size_t count)
{
    ssize_t sent = send(fd, buf, count, 0);
    return sent == (ssize_t)count;
}

static bool tcp_read(int fd, uint8_t *buf, size_t count)
{
    if (count == 0) {
        return true;
    }

    ssize_t received = recv(fd, buf, count, MSG_WAITALL);
    return received == (ssize_t)count;
}

// SERIAL
// #include <libserialport.h>

// struct transport_methodes {
//     int (*transport_connect)(transport_conn_t *conn, const char *host, uint16_t port);
//     void (*transport_disconnect)(transport_conn_t *conn);
//     int (*transport_listen)(transport_conn_t *conn, uint16_t port);
//     int (*transport_accept)(transport_conn_t *server, transport_conn_t *client);
//     bool (*transport_is_connected)(int fd);
//     bool (*transport_write)(int fd, const uint8_t *buf, size_t count);
//     bool (*transport_read)(int fd, uint8_t *buf, size_t count);
// };

// int serial_connect(transport_conn_t *conn, const char *host, uint16_t port)
// {

//     char *port_name = host;
//     struct sp_port *port = conn;

//     if (sp_get_port_by_name(port_name, &port) != SP_OK) return -1;

//     if (sp_open(port, SP_MODE_READ_WRITE) != SP_OK) return -1;

//     sp_set_baudrate(port, 115200);
//     sp_set_bits(port, 8);
//     sp_set_parity(port, SP_PARITY_NONE);
//     sp_set_stopbits(port, 1);
//     sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);

//     return 0;
// }

// void serial_disconnect(transport_conn_t *conn)
// {

// }

// int serial_listen(transport_conn_t *conn, uint16_t port)
// {

// }

// int serial_accept(transport_conn_t *server, transport_conn_t *client)
// {

// }

// bool serial_is_connected(int fd) {

// }

// static bool serial_write(int fd, const uint8_t *buf, size_t count)
// {
//     int result = sp_blocking_write(port, buf, count, 1000);
    
//     sp_drain(port); 
    
//     return result;
// }

// static bool serial_read(int fd, uint8_t *buf, size_t count)
// {
//     return (bool)sp_blocking_read(port, buf, count, 1000);
// }

// static struct transport_methodes serial = {
//     .transport_connect = serial_connect,
//     .transport_disconnect = serial_disconnect,
//     .transport_listen = serial_listen,
//     .transport_accept = serial_accept,
//     .transport_is_connected = serial_is_connected,
//     .transport_write = serial_write,
//     .transport_read = serial_read
// };

// GENERAL

static struct transport_methodes tcp = {
    .transport_connect = tcp_connect,
    .transport_disconnect = tcp_disconnect,
    .transport_listen = tcp_listen,
    .transport_accept = tcp_accept,
    .transport_is_connected = tcp_is_connected,
    .transport_write = tcp_write,
    .transport_read = tcp_read
};

static struct transport_methodes *selected = &tcp;

int transport_connect(transport_conn_t *conn, const char *host, uint16_t port)
{
    return selected->transport_connect(conn, host, port);
}

int transport_accept(transport_conn_t *server, transport_conn_t *client)
{
    return selected->transport_accept(server, client);
}

bool transport_is_connected(int fd) {
    return selected->transport_is_connected(fd);
}

static bool transport_write(int fd, const uint8_t *buf, size_t count)
{
    return selected->transport_write(fd, buf, count);
}

static bool transport_read(int fd, uint8_t *buf, size_t count)
{
    return selected->transport_read(fd, buf, count);
}

void transport_disconnect(transport_conn_t *conn)
{
    selected->transport_disconnect(conn);
}

int transport_listen(transport_conn_t *conn, uint16_t port)
{
    return  selected->transport_listen(conn, port);
}

/* ============================================================================
 * NANOPB STREAM CALLBACKS
 * ============================================================================ */

static bool write_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count)
{
    int fd = (intptr_t)stream->state;
    return transport_write(fd, buf, count);
}

static bool read_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    int fd = (intptr_t)stream->state;

    if (count == 0) {
        return true;
    }

    if (!transport_read(fd, buf, count)) {
        stream->bytes_left = 0; /* EOF or error */
        return false;
    }

    return true;
}

pb_ostream_t transport_get_ostream(transport_conn_t *conn)
{
    pb_ostream_t stream = {&write_callback, (void *)(intptr_t)conn->fd, SIZE_MAX, 0};
    return stream;
}

pb_istream_t transport_get_istream(transport_conn_t *conn)
{
    pb_istream_t stream = {&read_callback, (void *)(intptr_t)conn->fd, SIZE_MAX};
    return stream;
}

/* ============================================================================
 * HIGH-LEVEL MESSAGE SEND/RECEIVE
 * Uses length-delimited encoding (varint length prefix, like gRPC)
 * ============================================================================ */

bool transport_send_message(transport_conn_t *conn, const pb_msgdesc_t *fields, const void *msg)
{
    pb_ostream_t stream = transport_get_ostream(conn);
    return pb_encode_delimited(&stream, fields, msg);
}

bool transport_receive_message(transport_conn_t *conn, const pb_msgdesc_t *fields, void *msg)
{
    pb_istream_t stream = transport_get_istream(conn);
    return pb_decode_delimited(&stream, fields, msg);
}
