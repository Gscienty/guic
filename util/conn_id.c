#include "util/conn_id.h"

/**
 * init conn_id
 * @return conn_id
 * 
 */
struct conn_id_t conn_id_init() {
    struct conn_id_t ret = {
        .bytes = { 0 },
        .size = 0
    };
    return ret;
}
