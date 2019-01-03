#include "util/variable_integer.h"
#include "util/bigendian.h"
#include "wire/stop_sending.h"

inline static size_t __size(const struct stop_sending * const frm)
{
    return varint_size(frm->stream_id) + 2;
}

/**
 * get stop sending frame size
 * @param frm: stop sending frame
 * @return:frame size
 * 
 */
size_t stop_sending_size(const struct stop_sending * const frm)
{
    return __size(frm);
}

/**
 * encode stop sending frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: stop sending frame
 * @return: used size
 * 
 */
size_t stop_sending_encode(void * const buf, const size_t size, const struct stop_sending * const frm)
{
    size_t used_size = 0;

    // encode stream id
    used_size += varint_encode(buf + used_size, size - used_size, frm->stream_id);

    // encode err code
    bigendian_encode(buf + used_size, size - used_size, frm->err, 2);
    used_size += 2;

    return used_size;
}

/**
 * decode stop sending frame
 * @param frm: stop sending frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t stop_sending_decode(struct stop_sending * const frm, void * const buf, const size_t size)
{
    size_t used_size = 0;

    // decode stream id
    used_size += varint_decode(buf + used_size, size - used_size, &frm->stream_id);

    // decode err code
    bigendian_decode(buf + used_size, size - used_size, &frm->err, 2);
    used_size += 2;
    
    return used_size;
}
