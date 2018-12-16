#ifndef _GUIC_UNIT_LINKED_LIST_H
#define _GUIC_UNIT_LINKED_LIST_H

struct llnode {
    struct llnode *prev;
    struct llnode *next;
};

#define ll_entry(ptr, type, member) \
    ((type *) ((void *) ptr - (void *)(&((type *) 0)->member - 0)))

/**
 * init linked list head
 * @param head: linked list head
 * 
 */
void ll_head_init(struct llnode * const head);

/**
 * check linked list is empty
 * @param head: linked list
 * @return: is empty
 * 
 */
int ll_empty(const struct llnode * const head);

/**
 * insert entity before someone
 * @param place: insert place
 * @param newly: new entity
 * 
 */
void ll_insert_before(struct llnode * const place,
                      struct llnode * const newly);

/**
 * insert entity after someone
 * @param place: insert place
 * @param newly: new entity
 * 
 */
void ll_insert_after(struct llnode * const place,
                     struct llnode * const newly);


#endif
