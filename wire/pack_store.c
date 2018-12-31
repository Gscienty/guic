#include "wire/pack_store.h"
#include <stdlib.h>

static struct rbroot *__pack_obj_store = NULL;
static void __pack_init_rbroot()
{
    if (__pack_obj_store != NULL)
        return;

    __pack_obj_store = rbtree_create_root();
    if (__pack_obj_store == NULL)
        exit(-1);
}

static void __pack_obj_insert(void *ptr, size_t header_size)
{
    struct rbnode *obj = rbtree_create_node(__pack_obj_store, sizeof(struct __pack_obj));
    struct rbnode **place = &__pack_obj_store->root;
    struct rbnode *parent = &__pack_obj_store->nil;

    rb_entry(obj, struct __pack_obj, node)->ptr = ptr;
    rb_entry(obj, struct __pack_obj, node)->header_size = header_size;

    if (__pack_obj_store->root == &__pack_obj_store->nil)
        __pack_obj_store->root = obj;
    else {
        while (*place != &__pack_obj_store->nil) {
            parent = *place;
            if (ptr < rb_entry(parent, struct __pack_obj, node)->ptr) {
                place = &parent->left;
            }
            else if (rb_entry(parent, struct __pack_obj, node)->ptr < ptr) {
                place = &parent->right;
            }
            else {
                // bug
                return;
            }
        }
        rbtree_link(parent, place, obj);
    }

    rbtree_fixup(__pack_obj_store, obj);
}

/**
 * malloc packet
 * @param size: packet payload size
 * @return: readable/writable buffer
 *
 */
void *pack_malloc(size_t header_size, size_t size)
{
    __pack_init_rbroot();

    void *ptr = malloc(header_size + size);
    if (ptr == NULL)
        return NULL;

    __pack_obj_insert(ptr + header_size, header_size);

    return ptr + header_size;
}

static struct rbnode *__find_obj(void *ptr)
{
    void *target = ptr;
    struct rbnode *findret = __pack_obj_store->root;

    while (findret != &__pack_obj_store->nil) {
        if (target < rb_entry(findret, struct __pack_obj, node)->ptr)
            findret = findret->left;
        else if (rb_entry(findret, struct __pack_obj, node)->ptr < target)
            findret = findret->right;
        else 
            return findret;
    }

    return findret;
}

/**
 * free packet
 * @param ptr: payload buffer ptr
 *
 */
void pack_free(void *ptr)
{
    __pack_init_rbroot();

    struct rbnode *target_node = __find_obj(ptr);
    if (target_node == &__pack_obj_store->nil)
        target_node = __find_obj(ptr);
    if (target_node == &__pack_obj_store->nil)
        return;

    rbtree_delete(__pack_obj_store, target_node);
    free(rb_entry(target_node, struct __pack_obj, node)->ptr
         - rb_entry(target_node, struct __pack_obj, node)->header_size);
    free(target_node);
}
