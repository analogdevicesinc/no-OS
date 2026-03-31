/**
 * @file transport.h
 * @brief TCP transport layer for nanopb messaging
 *
 * This provides a simple request/response transport over TCP.
 * For bare metal: replace the socket operations in transport.c
 * with your TCP stack (lwIP, FreeRTOS+TCP, etc.)
 */

#ifndef CAPI_TRANSPORT_H
#define CAPI_TRANSPORT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <pb.h>

/* Connection handle - opaque to allow different implementations */
typedef struct {
    int fd;  /* Socket file descriptor (replace with your handle type for bare metal) */
} transport_conn_t;

/**
 * @brief Connect to a remote server
 * @param conn Pointer to connection handle (will be initialized)
 * @param host Server hostname or IP address
 * @param port Server port number
 * @return 0 on success, negative error code on failure
 */
int transport_connect(transport_conn_t *conn, const char *host, uint16_t port);

/**
 * @brief Disconnect from server/client
 * @param conn Connection handle
 */
void transport_disconnect(transport_conn_t *conn);

/**
 * @brief Start listening for incoming connections (server-side)
 * @param conn Pointer to server connection handle (will be initialized)
 * @param port Port number to listen on
 * @return 0 on success, negative error code on failure
 */
int transport_listen(transport_conn_t *conn, uint16_t port);

/**
 * @brief Accept an incoming connection (server-side)
 * @param server Server connection handle (from transport_listen)
 * @param client Pointer to client connection handle (will be initialized)
 * @return 0 on success, negative error code on failure
 */
int transport_accept(transport_conn_t *server, transport_conn_t *client);


bool transport_is_connected(int fd);

/**
 * @brief Send a protobuf message with length prefix
 * @param conn Connection handle
 * @param fields Protobuf field descriptors
 * @param msg Pointer to message structure
 * @return true on success, false on failure
 */
bool transport_send_message(transport_conn_t *conn, const pb_msgdesc_t *fields, const void *msg);

/**
 * @brief Receive a protobuf message with length prefix
 * @param conn Connection handle
 * @param fields Protobuf field descriptors
 * @param msg Pointer to message structure (will be filled)
 * @return true on success, false on failure
 */
bool transport_receive_message(transport_conn_t *conn, const pb_msgdesc_t *fields, void *msg);

/**
 * @brief Get nanopb output stream from connection (for advanced use)
 * @param conn Connection handle
 * @return pb_ostream_t bound to the connection
 */
pb_ostream_t transport_get_ostream(transport_conn_t *conn);

/**
 * @brief Get nanopb input stream from connection (for advanced use)
 * @param conn Connection handle
 * @return pb_istream_t bound to the connection
 */
pb_istream_t transport_get_istream(transport_conn_t *conn);

#endif /* CAPI_TRANSPORT_H */
