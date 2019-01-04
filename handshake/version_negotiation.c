#include "handshake/version_negotiation.h"
#include "util/bigendian.h"
#include <stdlib.h>

GUIC_SUPPORT_VERSION(0x00000000);

#ifndef DEBUG
// support version

#endif

static struct rbroot *__versions_store = NULL;

static void __versions_store_init()
{
    if (__versions_store != NULL)
        return;
    __versions_store = rbtree_create_root();
    if (__versions_store == NULL)
        exit(-1);
}

static void __versions_store_append(struct __guic_support_version * const version)
{
    struct rbnode *obj = &version->node;
    struct rbnode **place = &__versions_store->root;
    struct rbnode *parent = &__versions_store->nil;

    obj->left = &__versions_store->nil;
    obj->right = &__versions_store->nil;
    obj->parent = &__versions_store->nil;

    if (__versions_store->root == &__versions_store->nil)
        __versions_store->root = obj;
    else {
        while (*place != &__versions_store->nil) {
            parent = *place;
            if (version->version < rb_entry(parent,
                                            struct __guic_support_version,
                                            node)->version) {
                place = &parent->left;
            }
            else if (rb_entry(parent,
                              struct __guic_support_version,
                              node)->version < version->version) {
                place = &parent->right;
            }
            else {
                // bug
                return;
            }
        }
        rbtree_link(parent, place, obj);
    }

    rbtree_fixup(__versions_store, obj);
}

static struct __guic_support_version *__start = NULL;
static struct __guic_support_version *__end = NULL;
static int __support_version_size = 0;

static void __support_version_init()
{
    struct __guic_support_version *itr;

    if (__start != NULL && __end != NULL)
        return;

    __support_version_size = 0;
    __start = &GUIC_SUPPORT_VERSION_NAME(0x00000000);
    __end = &GUIC_SUPPORT_VERSION_NAME(0x00000000);

    while (1) {
        if ((__start - 1)->magic != GUIC_SUPPORT_VERSION_MAGIC)
            break;
        __start--;
    }

    while (1) {
        if ((__end + 1)->magic != GUIC_SUPPORT_VERSION_MAGIC)
            break;
        __end++;
    }
    __end++;

    __versions_store_init();

    for (itr = __start; itr != __end; itr++) {
        if (itr->version != 0x00000000) {
            __support_version_size++;
            __versions_store_append(itr);
        }
    }
}

/**
 * check this version is support
 * @param version: version
 * @return: 0 is support
 * 
 */
int version_support(version_t version)
{
    __support_version_init();

    struct rbnode *node = __versions_store->root;

    while (node != &__versions_store->nil) {
        if (rb_entry(node,
                     struct __guic_support_version,
                     node)->version == version)
            return 0;
        else if (rb_entry(node,
                          struct __guic_support_version,
                          node)->version > version)
            node = node->left;
        else
            node = node->right;
    }

    return -1;
}

/**
 * get support version count
 *
 * @return: version count
 */
int version_count()
{
    __support_version_init();

    return __support_version_size;
}

/**
 * construct version negotiation payload
 * @return: version negotiation payload
 * 
 */
size_t version_fill(void * const buf, size_t size)
{
    __support_version_init();

    size_t used_size = 0;
    struct __guic_support_version *version_itr;
    if ((size_t) version_count() * 4 > size)
        return 0;

    for (version_itr = __start; version_itr != __end; version_itr++) {
        if (version_itr == &GUIC_SUPPORT_VERSION_NAME(0x00000000))
            continue;
        bigendian_encode(buf + used_size,
                         size - used_size,
                         version_itr->version,
                         4);       
        used_size += 4;
    }

    return used_size;
}
