#include "wire/long_header.h"
#include "util/variable_integer.h"
#include "util/bigendian.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

static struct rbroot *__lpack_obj_store = NULL;

static void __lpack_init_rbroot()
{
    if (__lpack_obj_store != NULL)
        return;

    __lpack_obj_store = rbtree_create_root();
    if (__lpack_obj_store == NULL)
        exit(-1);
}

static void __lpack_obj_insert(void *ptr)
{
    struct rbnode *obj = rbtree_create_node(__lpack_obj_store, sizeof(struct __lpack_obj));
    struct rbnode **place = &__lpack_obj_store->root;
    struct rbnode *parent = &__lpack_obj_store->nil;

    rb_entry(obj, struct __lpack_obj, node)->ptr = ptr;

    if (__lpack_obj_store->root == &__lpack_obj_store->nil) {
        __lpack_obj_store->root = obj;
    }
    else {
        while (*place != &__lpack_obj_store->nil) {
            parent = *place;
            if (ptr < rb_entry(parent, struct __lpack_obj, node)->ptr) {
                place = &parent->left;
            }
            else if (rb_entry(parent, struct __lpack_obj, node)->ptr < ptr) {
                place = &parent->right;
            }
            else {
                // bug
                return;
            }
        }
        rbtree_link(parent, place, obj);
    }

    rbtree_fixup(__lpack_obj_store, obj);
}

static const size_t LONG_HEADER_MAX_SIZE = 1 + 4 + 1 + 18 + 18 + 8 + 4;

/**
 * malloc long header packet
 * @param size: packet payload size
 * @return: readable/writable buffer
 *
 */
void *lpack_malloc(size_t size)
{
    __lpack_init_rbroot();

    void *ptr = malloc(LONG_HEADER_MAX_SIZE + size);
    if (ptr == NULL)
        return NULL;

    __lpack_obj_insert(ptr);

    return ptr + LONG_HEADER_MAX_SIZE;
}

static struct rbnode *__find_obj(void *ptr)
{
    void *target = ptr - LONG_HEADER_MAX_SIZE;
    struct rbnode *findret = __lpack_obj_store->root;

    while (findret != &__lpack_obj_store->nil) {
        if (target < rb_entry(findret, struct __lpack_obj, node)->ptr)
            findret = findret->left;
        else if (rb_entry(findret, struct __lpack_obj, node)->ptr < target)
            findret = findret->right;
        else 
            return findret;
    }

    return findret;
}

/**
 * free long header
 * @param ptr: payload buffer ptr
 *
 */
void lpack_free(void *ptr)
{
    __lpack_init_rbroot();

    struct rbnode *target_node = __find_obj(ptr);
    rbtree_delete(__lpack_obj_store, target_node);
    free(rb_entry(target_node, struct __lpack_obj, node)->ptr);
    free(target_node);
}

static size_t __long_header_size(const struct long_header * const lh_ptr)
{
    return 1                                  // long header first byte
        + 4                                   // version
        + 1                                   // DCIL && SCIL
        + lh_ptr->dst_connid.size             // dst connid size
        + lh_ptr->src_connid.size             // src connid size
        + varint_size(lh_ptr->len)            // length
        + packet_number_length(lh_ptr->pnum); // packet number
}

static int __long_header_encode(void *buf,
                                 const struct long_header * const lh_ptr,
                                 const size_t lh_size)
{
    size_t used_size = 0;
    size_t pnum_size = packet_number_length(lh_ptr->pnum);
    uint8_t byte;

    byte  = 0xC0;                                 // header form && fixed bit
    byte ^= (lh_ptr->type << 4);                  // long packet type
    byte ^= (packet_number_length(lh_ptr->pnum)); // packet number length
    *((uint8_t *) buf) = byte;
    used_size += 1;
    if (used_size >= lh_size)
        return -1;

    // encode version
    if (bigendian_encode(buf + used_size, lh_size - used_size, lh_ptr->version, 4) < 0)
        return -1;
    used_size += 4;
    if (used_size >= lh_size)
        return -1;

    // encode cli
    byte = 0x00;
    if (lh_ptr->dst_connid.size != 0)
        byte ^= (lh_ptr->dst_connid.size - 3) << 4;
    if (lh_ptr->src_connid.size != 0)
        byte ^= lh_ptr->src_connid.size - 3;
    *((uint8_t *) (buf + used_size)) = byte;
    used_size += 1;
    if (used_size >= lh_size)
        return -1;

    // encode dst connid
    memcpy(buf + used_size, lh_ptr->dst_connid.bytes, lh_ptr->dst_connid.size);
    used_size += lh_ptr->dst_connid.size;
    if (used_size >= lh_size)
        return -1;

    memcpy(buf + used_size, lh_ptr->src_connid.bytes, lh_ptr->src_connid.size);
    used_size += lh_ptr->src_connid.size;
    if (used_size >= lh_size)
        return -1;

    used_size += varint_encode(buf + used_size, lh_size - used_size, lh_ptr->len);
    if (used_size >= lh_size)
        return -1;

    bigendian_encode(buf + used_size, lh_size - used_size, lh_ptr->pnum, pnum_size);
    used_size += pnum_size;
    if (used_size >= lh_size)
        return -1;

    return lh_size - 1 - used_size;
}

/**
 * put long header
 * @param lh_ptr: long header ptr
 * @param payload: payload ptr
 * @return: buffer && buffer size
 * 
 */
struct buf lpack_put_header(struct long_header * const lh_ptr, void *payload, size_t size)
{
    struct buf ret;
    size_t header_size = __long_header_size(lh_ptr);

    ret.size = header_size + size;
    ret.ptr = payload - header_size;

    __long_header_encode(ret.ptr, lh_ptr, header_size);

    return ret;
}

/**
 * get long header
 * @param buf: buf
 * @param size: buf size
 * @return: long header
 * 
 */
struct long_header lpack_get_header(void *buf, size_t size)
{
    size_t used_size = 0;
    size_t pnum_size;
    uint8_t cil;
    struct long_header header = {
        .payload = NULL
    };
    
    // get first byte
    uint8_t first_byte = *((uint8_t *) buf);
    used_size += 1;
    if (used_size >= size)
        return header;
    if ((first_byte & 0xC0) != 0xC0)
        return header;
    pnum_size = 0x03 & first_byte;
    header.type = (enum long_header_packet_type) ((first_byte & 0x30) >> 4);

    // decode version
    bigendian_decode(buf + used_size, size - used_size, &header.version, 4);
    used_size += 4;
    if (used_size >= size)
        return header;

    // decode dcil && scil
    cil = *((uint8_t *) buf + used_size);
    used_size += 1;
    if (used_size >= size)
        return header;

    // decode dst_connid
    if ((cil & 0xF0) != 0x00) {
        header.dst_connid.size = ((cil & 0xF0) >> 4) + 3;
        memcpy(header.dst_connid.bytes, buf + used_size, header.dst_connid.size);
        used_size += header.dst_connid.size;
        if (used_size >= size)
            return header;
    }
    else
        header.dst_connid.size = 0;

    // decode src_connid
    if ((cil & 0x0F) != 0x00) {
        header.src_connid.size = (cil & 0x0F) + 3;
        memcpy(header.src_connid.bytes, buf + used_size, header.src_connid.size);
        used_size += header.src_connid.size;
        if (used_size >= size)
            return header;
    }
    else
        header.src_connid.size = 0;

    // decode length
    used_size += varint_decode(buf + used_size, size - used_size, &header.len);
    if (used_size >= size)
        return header;

    // decode packet number
    used_size += bigendian_decode(buf + used_size, size - used_size, &header.pnum, pnum_size);
    if (used_size > size)
        return header;

    // set payload ptr
    header.payload = buf + used_size;

    return header;
}
