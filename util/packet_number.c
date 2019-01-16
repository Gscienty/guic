#include "util/packet_number.h"
#include <stdlib.h>

/**
 * get packet number size
 * @param pnum: packet number
 * @return: packet number size
 * 
 */
size_t packet_number_length(packet_number_t pnum)
{
    if (pnum <= 0xFF)
        return 1;
    else if (pnum <= 0xFFFF)
        return 2;
    else if (pnum <= 0xFFFFFF)
        return 3;
    else
        return 4;
}

packet_number_t packet_number_decode(packet_number_t largest_pn,
                                     packet_number_t truncated_pn,
                                     size_t pn_nbits)
{
    packet_number_t expected_pn = largest_pn + 1;
    packet_number_t pn_win = 1 << pn_nbits;
    packet_number_t pn_hwin = pn_win / 2;
    packet_number_t pn_mask = pn_win - 1;
    packet_number_t candidate_pn = (expected_pn & ~pn_mask) | truncated_pn;

    if (candidate_pn <= expected_pn - pn_win)
        return candidate_pn + pn_win;

    if (candidate_pn > expected_pn + pn_hwin
        && candidate_pn > pn_win)
        return candidate_pn - pn_win;

    return candidate_pn;
}

static void __generate_skip(struct pnum_gener * const gener)
{
    packet_number_t num = rand();
    packet_number_t skip = num * (gener->period - 1) / (0xFFFF / 2);

    gener->skip = gener->next + 2 + skip;
}

/**
 * init packet number generator
 * @param gener: generator
 * @param init: init packet number
 * @param period: period
 *
 */
void pnum_gener_init(struct pnum_gener * const gener,
                     packet_number_t init,
                     packet_number_t period)
{
    gener->next         = init;
    gener->period       = period;
    gener->history_size = 0;
    gener->history      = rbtree_create_root();

    __generate_skip(gener);
}

/**
 * get packet number peek
 * 
 */
packet_number_t pnum_gener_peek(const struct pnum_gener * const gener)
{
    return gener->next;
}

static int __add_history(struct rbroot * const history, packet_number_t pnum)
{
    struct rbnode *node;
    struct rbnode *parent;
    struct rbnode **place;
    packet_number_t flag_pnum;

    if (history->root == &history->nil) {
        node = rbtree_create_node(history, sizeof(struct pnum_history));
        if (node == NULL) {
            return -1;
        }
        rb_entry(node, struct pnum_history, node)->pnum = pnum;
        history->root = node;
    }
    else {
        place = &history->root;
        while (*place != &history->nil) {
            parent = *place;
            flag_pnum = rb_entry(parent, struct pnum_history, node)->pnum;

            if (pnum < flag_pnum) {
                place = &parent->left;
            }
            else if (flag_pnum < pnum) {
                place = &parent->right;
            }
            else {
                return -1;
            }
        }
        node = rbtree_create_node(history, sizeof(struct pnum_history));
        if (node == NULL) {
            return -1;
        }
        rb_entry(node, struct pnum_history, node)->pnum = pnum;
        rbtree_link(parent, place, node);
    }
    rbtree_fixup(history, node);
    return 0;
}

/**
 * get packet number && generate next packet number
 * @param gener: generator
 * @return: packet number
 *
 */
packet_number_t pnum_gener_pop(struct pnum_gener * const gener)
{
    packet_number_t next = gener->next;

    gener->next++;

    if (gener->next == gener->skip) {
        __add_history(gener->history, next);
        gener->next++;
        __generate_skip(gener);
    }

    return next;
}

/**
 * exist packet number
 * @param gener: generator
 * @param pnum: packet number
 * @return exist packet number
 *
 */
bool pnum_gener_existed(const struct pnum_gener * const gener,
                        const packet_number_t pnum)
{
    packet_number_t flag_pnum;
    struct rbnode *node = gener->history->root;

    while (node != &gener->history->nil) {
        flag_pnum = rb_entry(node, struct pnum_history, node)->pnum;

        if (flag_pnum == pnum) {
            return true;
        }
        else if (pnum < flag_pnum) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    return false;
}
