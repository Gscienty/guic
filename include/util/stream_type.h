#ifndef _GUIC_UNIT_STREAM_TYPE_H
#define _GUIC_UNIT_STREAM_TYPE_H

#include <stdint.h>

typedef uint64_t stream_t;

enum stream_type {
    STREAM_TYPE_CLI_BI,
    STREAM_TYPE_SER_BI,
    STREAM_TYPE_CLI_UN,
    STREAM_TYPE_SER_UN,

    __STREAM_TYPE_MAX
};

enum stream_type stream_type_get(stream_t sid);

#endif
