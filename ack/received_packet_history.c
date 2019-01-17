#include "ack/packet.h"
#include <malloc.h>
#include <stdbool.h>

/**
 * init packet history
 * @param history: packet history
 * 
 */
void recv_packet_history_init(struct recv_packet_history * const history)
{
    ll_head_init(&history->head);
    history->lowest_pnum = 0;
    history->size = 0;
}

/**
 * received packet
 * @param history: history
 * @param pnum: packet number
 * 
 */
int history_received_packet(struct recv_packet_history * const history,
                            packet_number_t pnum)
{
    struct llnode *new_interval;
    struct llnode *hele;
    struct recv_packet_llnode *hele_val;
    struct recv_packet_llnode *hele_prev_val;
    bool range_extends;

    if (history->size == 0) {
        new_interval = (struct llnode *)
            malloc(sizeof(struct recv_packet_llnode));
        if (new_interval == NULL) {
            return -1;
        }
        ll_entry(new_interval, struct recv_packet_llnode, node)->min = pnum;
        ll_entry(new_interval, struct recv_packet_llnode, node)->max = pnum;
        ll_insert_before(&history->head, new_interval);
        history->size++;
        return 0;
    }

    for (hele = history->head.prev;
         hele != &history->head;
         hele = hele->prev) {
        hele_val = ll_entry(hele, struct recv_packet_llnode, node);

        if (hele_val->min <= pnum && pnum <= hele_val->max) {
            return 0;
        }

        range_extends = false;
        if (hele_val->max == pnum - 1) {
            range_extends = true;
            hele_val->max = pnum;
        }
        else if (hele_val->min == pnum + 1) {
            range_extends = true;
            hele_val->min = pnum;
        }

        if (range_extends) {
            hele_prev_val = (struct recv_packet_llnode *)
                hele_val->node.prev;
            if ((struct llnode *) hele_prev_val != &history->head
                && hele_prev_val->max + 1 == hele_val->max) {
                // merge
                hele_prev_val->max = hele_val->max;
                ll_remove(&hele_val->node);
                history->size--;
                free(hele_val);
                return 0;
            }
            return 0;
        }

        if (pnum > hele_val->max) {
            new_interval = (struct llnode *)
                malloc(sizeof(struct recv_packet_llnode));
            if (new_interval == NULL) {
                return -1;
            }
            ll_entry(new_interval,
                     struct recv_packet_llnode,
                     node)->min = pnum;
            ll_entry(new_interval,
                     struct recv_packet_llnode,
                     node)->max = pnum;
            
            ll_insert_after(&hele_val->node, new_interval);
            history->size++;

            return 0;
        }
    }

    new_interval = (struct llnode *)
        malloc(sizeof(struct recv_packet_llnode));
    if (new_interval == NULL) {
        return -1;
    }
    ll_entry(new_interval, struct recv_packet_llnode, node)->min = pnum;
    ll_entry(new_interval, struct recv_packet_llnode, node)->max = pnum;
    ll_insert_after(&history->head, new_interval);
    history->size++;

    return 0;
}

/**
 * delete below
 * @param history: history
 * @param pnum: packet number
 * 
 */
void recv_packet_delete_below(struct recv_packet_history * const history,
                              packet_number_t pnum)
{
    struct llnode *next;
    struct llnode *cur;
    struct recv_packet_llnode *val;
    if (pnum <= history->lowest_pnum) {
        return;
    }
    history->lowest_pnum = pnum;

    next = history->head.next;

    for (cur = history->head.next;
         next != &history->head;
         cur = next) {
        next = cur->next;

        val = ll_entry(cur, struct recv_packet_llnode, node);

        if (pnum > val->min && pnum <= val->max) {
            val->min = pnum;
        }
        else if (val->max < pnum) {
            ll_remove(&val->node);
            history->size--;
            free(val);
        }
        else {
            return;
        }
    }
}

/**
 * generate ack ranges
 * @param history: history
 * @return: ack ranges
 *
 */
struct ack_block *
recv_packet_generate_ack(const struct recv_packet_history * const history)
{
    int i;
    struct llnode *node;
    struct ack_block *ranges;
    if (history->size == 0) {
        return NULL;
    }

    ranges = (struct ack_block *)
        calloc(history->size, sizeof(struct ack_block));
    i = 0;
    for (node = history->head.prev;
         node != &history->head;
         node = node->prev) {
        ranges[i].start = ll_entry(node, struct recv_packet_llnode, node)->min;
        ranges[i].end = ll_entry(node, struct recv_packet_llnode, node)->max;

        i++;
    }

    return ranges;
}
