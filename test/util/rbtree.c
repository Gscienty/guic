#include "test/testcase.h"
#include "util/rbtree.h"

struct entity_t {
    struct rbnode node;

    int magic_1;
    int magic_2;
};

GUIC_TEST(rbtree, rb_entry) 
{
    struct entity_t entity; 

    entity.magic_1 = 0xABCD;
    entity.magic_2 = 0x1234;

    struct rbnode *node_ptr = &entity.node;

    ASSERT(0xABCD, rb_entry(node_ptr, struct entity_t, node)->magic_1, ==);
    ASSERT(0x1234, rb_entry(node_ptr, struct entity_t, node)->magic_2, ==);
}

struct insert_entity {
    struct rbnode node;
    int key;
};

GUIC_TEST(rbtree, insert)
{
    struct rbroot *root   = rbtree_create_root();
    struct rbnode **place = NULL;
    struct rbnode *parent = NULL;
    struct rbnode *queue[10];
    int expect[] = { 3, 1, 5, 0, 2, 4, 7, 6, 8, 9 };
    int front = 0;
    int tail = 0;
    int i;

    for (i = 0; i < 10; i++) {
        struct rbnode *node = rbtree_create_node(root, sizeof(struct insert_entity));
        rb_entry(node, struct insert_entity, node)->key = i;

        if (root->root == &root->nil) {
            root->root = node;
        }
        else {
            place = &root->root;
            while (*place != &root->nil) {
                parent = *place;
                if (rb_entry(parent, struct insert_entity, node)->key > i) {
                    place = &parent->left;
                }
                else {
                    place = &parent->right;
                }
            }
        }

        rbtree_link(parent, place, node);
        rbtree_fixup(root, node);
    }
    
    queue[tail++] = root->root;
    while (front != 10) {
        if (queue[front]->left != &root->nil) {
            queue[tail++] = queue[front]->left;
        }
        if (queue[front]->right != &root->nil) {
            queue[tail++] = queue[front]->right;
        }
        ASSERT(expect[front],
               rb_entry(queue[front], struct insert_entity, node)->key,
               ==);
        front++;
    }
}

GUIC_TEST(rbtree, remove)
{
    struct rbroot *root   = rbtree_create_root();
    struct rbnode **place = NULL;
    struct rbnode *parent = NULL;
    int i;

    for (i = 0; i < 100; i++) {
        struct rbnode *node = rbtree_create_node(root, sizeof(struct insert_entity));
        rb_entry(node, struct insert_entity, node)->key = i;

        if (root->root == &root->nil) {
            root->root = node;
        }
        else {
            place = &root->root;
            while (*place != &root->nil) {
                parent = *place;
                if (rb_entry(parent, struct insert_entity, node)->key > i) {
                    place = &parent->left;
                }
                else {
                    place = &parent->right;
                }
            }
        }

        rbtree_link(parent, place, node);
        rbtree_fixup(root, node);
    }

    for (i = 0; i < 100; i++)
        rbtree_delete(root, root->root);
    ASSERT(0, rbtree_is_nil(root, root->root), ==);
}
