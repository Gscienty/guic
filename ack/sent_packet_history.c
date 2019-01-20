#include "ack/packet.h"
#include <malloc.h>

/**
 * init sent packet history
 * @param history: history
 * 
 */
void sent_packet_history_init(struct sent_packet_history * const history)
{
    history->crypto_packets_count = 0;
    rbtree_root_init(&history->dict);
    history->first = NULL;
    ll_head_init(&history->list);
    history->packets_count = 0;
}

static void __dict_insert(struct rbroot * const root,
                          struct sent_packet_node * const node)
{
    struct rbnode **place = &root->root;
    struct rbnode *parent = &root->nil;
    struct sent_packet_node *flag;

    if (root->root == &root->nil) {
        root->root = &node->rbnode;
    }
    else {
        while (*place != &root->nil) {
            parent = *place;
            flag = rb_entry(parent, struct sent_packet_node, rbnode);

            if (node->packet->pnum < flag->packet->pnum) {
                place = &parent->left;
            }
            else if (flag->packet->pnum < node->packet->pnum) {
                place = &parent->right;
            }
            else {
                return;
            }
        }
        rbtree_link(parent, place, &node->rbnode);
    }

    rbtree_fixup(root, &node->rbnode);
}

/**
 * sent packet
 * @param history: history
 * @param packet: packet
 * 
 */
struct llnode *sent_packet_sent(struct sent_packet_history * const history,
                                const struct packet *packet)
{
    // init node
    struct sent_packet_node *node =
        (struct sent_packet_node *) malloc(sizeof(struct sent_packet_node));
    if (node == NULL) {
        return NULL;
    }
    rbtree_node_init(&history->dict, &node->rbnode);
    node->packet = packet;

    // node inserted to rbtree & list
    ll_insert_before(&history->list, &node->llnode);
    __dict_insert(&history->dict, node);

    if (history->first == NULL) {
        history->first = &node->llnode;
    }
    if (packet->allow_retrans) {
        history->packets_count++;
        if (packet->encrypt_level != ENCRYPT_LEVEL_1RTT) {
            history->crypto_packets_count++;
        }
    }

    return &node->llnode;
}

struct rbnode * __dict_find(struct rbroot * const root,
                            packet_number_t pnum)
{
    struct rbnode *node = root->root;
    struct sent_packet_node *val;

    while (node != &root->nil) {
        val = rb_entry(node, struct sent_packet_node, rbnode);
        if (pnum < val->packet->pnum) {
            node = node->left;
        }
        else if (val->packet->pnum < pnum) {
            node = node->right;
        }
        else {
            break;
        }
    }

    return node;
}

/**
 * retransmission sent packet
 * @param history: history
 * @param head: packet list
 * @param retrans_pnum: retransmission packet number
 * 
 */
void sent_packet_retransmission(struct sent_packet_history * const history,
                                struct llnode * const head,
                                packet_number_t retrans_pnum)
{
    struct rbnode *entry = __dict_find(&history->dict, retrans_pnum);
    struct llnode *pll_node;
    struct packet *pll_val;
    struct packet *entry_val;
    struct packet_retrans_as_llnode *pnum_node;

    if (entry == &history->dict.nil) {
        for (pll_node = head->next;
             pll_node != head;
             pll_node = pll_node->next) {
            pll_val = (struct packet *) ll_entry(pll_node,
                                                 struct packet_llnode,
                                                 node)->packet;
            sent_packet_sent(history,
                             ll_entry(pll_node, struct packet_llnode, node)
                             ->packet);
        }
        return;
    }

    entry_val = (struct packet *) rb_entry(entry,
                                           struct sent_packet_node,
                                           rbnode)->packet;
    for (pll_node = head->next;
         pll_node != head;
         pll_node = pll_node->next) {
        pll_val = (struct packet *) ll_entry(pll_node,
                                             struct packet_llnode,
                                             node)->packet;

        pnum_node = (struct packet_retrans_as_llnode *)
            malloc(sizeof(struct packet_retrans_as_llnode));
        if (pnum_node == NULL) {
            // BUG
            return;
        }
        pnum_node->pnum = pll_val->pnum;
        ll_insert_before(&entry_val->retrans_as, &pnum_node->node);
        pll_val->is_retrans = true;
        pll_val->retrans_pnum = retrans_pnum;
    }
}

/**
 * get packet
 * @param history: history
 * @param pnum: packet number
 * @return packet
 * 
 */
const struct packet *sent_packet_get(struct sent_packet_history * const history,
                               packet_number_t pnum)
{
    struct rbnode *node = __dict_find(&history->dict, pnum);
    if (node == &history->dict.nil) {
        return NULL;
    }

    return rb_entry(node, struct sent_packet_node, rbnode)->packet;
}

static void __readjust_first(struct sent_packet_history* const history)
{
    struct llnode *next = history->first->next;
    while (next != &history->list
           && ll_entry(next,
                       struct packet_llnode,
                       node)->packet->allow_retrans) {
        next = next->next;
    }
    history->first = next;
}

/**
 * mark retransmission
 * @param history: history
 * @param pnum: packet number
 * 
 */
void sent_packet_mark_cannot_retrans(struct sent_packet_history * const history,
                                     packet_number_t pnum)
{
    struct rbnode *packet_node = __dict_find(&history->dict, pnum);
    struct packet *packet;
    if (packet_node == &history->dict.nil) {
        return;
    }
    packet = (struct packet *) rb_entry(packet_node,
                                        struct sent_packet_node,
                                        rbnode)->packet;

    if (packet->allow_retrans) {
        history->packets_count--;
        if (history->packets_count < 0) {
            // BUG
            return;
        }
        if (packet->encrypt_level != ENCRYPT_LEVEL_1RTT) {
            history->crypto_packets_count--;
            if (history->crypto_packets_count < 0) {
                // BUG
                return;
            }
        }
    }

    packet->allow_retrans = false;
    if (packet == ll_entry(&history->first,
                           struct packet_llnode,
                           node)->packet) {
        __readjust_first(history);
    }
}

/**
 * remove packet
 * @param history: history
 * @param pnum: packet number
 * 
 */
void sent_packet_remove(struct sent_packet_history * const history,
                        packet_number_t pnum)
{
    struct rbnode *packet_node = __dict_find(&history->dict, pnum);
    struct packet *packet;
    if (packet_node == &history->dict.nil) {
        return;
    }
    packet = (struct packet *) rb_entry(packet_node,
                                        struct sent_packet_node,
                                        rbnode)->packet;

    if (packet == ll_entry(history->first,
                           struct packet_llnode,
                           node)->packet) {
        __readjust_first(history);
    }

    if (packet->allow_retrans) {
        history->packets_count--;
        if (history->packets_count < 0) {
            return;
        }
        if (packet->encrypt_level != ENCRYPT_LEVEL_1RTT) {
            history->crypto_packets_count--;
            if (history->crypto_packets_count < 0) {
                return;
            }
        }
    }

    ll_remove(&rb_entry(packet_node, struct sent_packet_node, rbnode)->llnode);
    rbtree_delete(&history->dict, packet_node);
}
