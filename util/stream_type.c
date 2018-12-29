#include "util/stream_type.h"

enum stream_type stream_type_get(stream_t sid)
{
    return (enum stream_type) (sid % __STREAM_TYPE_MAX);
}
