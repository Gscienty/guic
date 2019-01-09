#ifndef _GUIC_WIRE_TYPE_H
#define _GUIC_WIRE_TYPE_H

#include <stdint.h>

enum long_header_packet_type {
    LONG_HEADER_PACKET_INITIAL   = 0x00,
    LONG_HEADER_PACKET_0RTT      = 0x01,
    LONG_HEADER_PACKET_HANDSHAKE = 0x02,
    LONG_HEADER_PACKET_RETRY     = 0x03
};

enum wire_type {
    G_WTP_PING                         = 0x00,
    G_WTP_PONG                         = 0x01,
    G_WTP_ACK                          = 0x02,
    G_WTP_ACK_ECN                      = 0x03,
    G_WTP_RESET                        = 0x04,
    G_WTP_STOP_SENDING                 = 0x05,
    G_WTP_CRYPTO                       = 0x06,
    G_WTP_NET_TOKEN                    = 0x07,
    G_WTP_STREAM                       = 0x08,
    G_WTP_MAX_DATA                     = 0x10,
    G_WTP_MAX_STREAM_DATA              = 0x11,
    G_WTP_STREAMS_BIDIRECTION          = 0x12,
    G_WTP_STREAMS_UNIDIRECTION         = 0x13,
    G_WTP_DATA_BLOCKED                 = 0x14,
    G_WTP_STREAM_DATA_BLOCKED          = 0x15,
    G_WTP_STREAMS_BLOCKED_BIDIRECTION  = 0x16,
    G_WTP_STREAMS_BLOCKED_UNIDIRECTION = 0x17,
    G_WTP_NET_CONNECTION_ID            = 0x18,
    G_WTP_RETIRE_CONNECTION_ID         = 0x19,
    G_WTP_PATH_CHALLENGE               = 0x1A,
    G_WTP_PATH_RESPONSE                = 0x1B,
    G_WTP_CONNECTION_CLOSE_QUIC_LAYER  = 0x1C,
    G_WTP_CONNECTION_CLOSE_APPLICATION = 0x1D
};

enum stream_type {
    STREAM_TYPE_CLI_BI,
    STREAM_TYPE_SER_BI,
    STREAM_TYPE_CLI_UN,
    STREAM_TYPE_SER_UN,

    __STREAM_TYPE_MAX
};

typedef uint32_t version_t;
typedef uint32_t packet_number_t;
typedef uint64_t stream_id_t;
typedef uint16_t app_err_t;
typedef uint64_t bytecount_t;

struct conn_id_t {
    uint8_t bytes[18];
    int size;
};

#endif
