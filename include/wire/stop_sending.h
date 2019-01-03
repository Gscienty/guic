#ifndef _GUIC_WIRE_STOP_SENDING_H
#define _GUIC_WIRE_STOP_SENDING_H

#include "wire/type.h"
#include <stddef.h>

struct stop_sending {
    stream_id_t stream_id;
    app_err_t err;
};

/**
 * get stop sending frame size
 * @param frm: stop sending frame
 * @return:frame size
 * 
 */
size_t stop_sending_size(const struct stop_sending * const frm);

/**
 * encode stop sending frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: stop sending frame
 * @return: used size
 * 
 */
size_t stop_sending_encode(void * const buf, const size_t size, const struct stop_sending * const frm);

/**
 * decode stop sending frame
 * @param frm: stop sending frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t stop_sending_decode(struct stop_sending * const frm, void * const buf, const size_t size);

#endif
