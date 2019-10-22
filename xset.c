#include <stdlib.h>
#include <string.h>

#include "xset.h"

/*
 * +++++ linux kernel rbtree interface - start +++++
 */

#define	RB_RED              0
#define	RB_BLACK	        1

#define __rb_parent(pc)     ((struct rb_node *)(pc & ~3))
#define rb_parent(rb)       ((struct rb_node *)((rb)->__rb_parent_color & ~3))

#define __rb_color(pc)      ((pc) & 1)
#define __rb_is_black(pc)   __rb_color(pc)
#define __rb_is_red(pc)     (!__rb_color(pc))
#define rb_color(rb)        __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb)       __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb)     __rb_is_black((rb)->__rb_parent_color)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
    rb->__rb_parent_color = rb_color(rb) | (unsigned long)p;
}

static inline void rb_set_parent_color(struct rb_node *rb,
                        struct rb_node *p, int color)
{
    rb->__rb_parent_color = (unsigned long)p | color;
}

static inline void rb_set_black(struct rb_node *rb)
{
    rb->__rb_parent_color |= RB_BLACK;
}

static inline struct rb_node *rb_red_parent(struct rb_node *red)
{
    return (struct rb_node *)red->__rb_parent_color;
}

static inline void __rb_change_child(struct rb_node *old, struct rb_node *new,
            struct rb_node *parent, struct rb_node **root)
{
    if (parent) {
        if (parent->rb_left == old)
            parent->rb_left = new;
        else
            parent->rb_right = new;
    } else {
        *root = new;
    }
}

/*
 * Helper function for rotations:
 * - old's parent and color get assigned to new
 * - old gets assigned new as a parent and 'color' as a color.
 */
static inline void __rb_rotate_set_parents(struct rb_node *old, struct rb_node *new,
            struct rb_node **root, int color)
{
    struct rb_node *parent = rb_parent(old);
    new->__rb_parent_color = old->__rb_parent_color;
    rb_set_parent_color(old, new, color);
    __rb_change_child(old, new, parent, root);
}

/*
 * Insert 'node' into 'rb_link', below 'parent'
 */
static inline void __rb_insert_node(struct rb_node *node, struct rb_node *parent,
                struct rb_node **rb_link)
{
    node->__rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

static inline void __rb_insert_color(struct rb_node *node, struct rb_node **root)
{
    struct rb_node *parent = rb_red_parent(node), *gparent, *tmp;

    while (1) {
        /*
         * Loop invariant: node is red.
         */
        if (!parent) {
            /*
             * The inserted node is root. Either this is the
             * first node, or we recursed at Case 1 below and
             * are no longer violating 4).
             */
            rb_set_parent_color(node, NULL, RB_BLACK);
            break;
        }

        /*
         * If there is a black parent, we are done.
         * Otherwise, take some corrective action as,
         * per 4), we don't want a red root or two
         * consecutive red nodes.
         */
        if(rb_is_black(parent))
            break;

        gparent = rb_red_parent(parent);

        tmp = gparent->rb_right;
        if (parent != tmp) {	/* parent == gparent->rb_left */
            if (tmp && rb_is_red(tmp)) {
                /*
                 * Case 1 - node's uncle is red (color flips).
                 *
                 *       G            g
                 *      / \          / \
                 *     p   u  -->   P   U
                 *    /            /
                 *   n            n
                 *
                 * However, since g's parent might be red, and
                 * 4) does not allow this, we need to recurse
                 * at g.
                 */
                rb_set_parent_color(tmp, gparent, RB_BLACK);
                rb_set_parent_color(parent, gparent, RB_BLACK);
                node = gparent;
                parent = rb_parent(node);
                rb_set_parent_color(node, parent, RB_RED);
                continue;
            }

            tmp = parent->rb_right;
            if (node == tmp) {
                /*
                 * Case 2 - node's uncle is black and node is
                 * the parent's right child (left rotate at parent).
                 *
                 *      G             G
                 *     / \           / \
                 *    p   U  -->    n   U
                 *     \           /
                 *      n         p
                 *
                 * This still leaves us in violation of 4), the
                 * continuation into Case 3 will fix that.
                 */
                tmp = node->rb_left;
                parent->rb_right = tmp;
                node->rb_left = parent;
                if (tmp)
                    rb_set_parent_color(tmp, parent, RB_BLACK);
                rb_set_parent_color(parent, node, RB_RED);
                parent = node;
                tmp = node->rb_right;
            }

            /*
             * Case 3 - node's uncle is black and node is
             * the parent's left child (right rotate at gparent).
             *
             *        G           P
             *       / \         / \
             *      p   U  -->  n   g
             *     /                 \
             *    n                   U
             */
            gparent->rb_left = tmp; /* == parent->rb_right */
            parent->rb_right = gparent;
            if (tmp)
                rb_set_parent_color(tmp, gparent,
                            RB_BLACK);
            __rb_rotate_set_parents(gparent, parent, root, RB_RED);
            break;
        } else {
            tmp = gparent->rb_left;
            if (tmp && rb_is_red(tmp)) {
                /* Case 1 - color flips */
                rb_set_parent_color(tmp, gparent, RB_BLACK);
                rb_set_parent_color(parent, gparent, RB_BLACK);
                node = gparent;
                parent = rb_parent(node);
                rb_set_parent_color(node, parent, RB_RED);
                continue;
            }

            tmp = parent->rb_left;
            if (node == tmp) {
                /* Case 2 - right rotate at parent */
                tmp = node->rb_right;
                parent->rb_left = tmp;
                node->rb_right = parent;
                if (tmp)
                    rb_set_parent_color(tmp, parent,
                                RB_BLACK);
                rb_set_parent_color(parent, node, RB_RED);
                parent = node;
                tmp = node->rb_left;
            }

            /* Case 3 - left rotate at gparent */
            gparent->rb_right = tmp; /* == parent->rb_left */
            parent->rb_left = gparent;
            if (tmp)
                rb_set_parent_color(tmp, gparent, RB_BLACK);
            __rb_rotate_set_parents(gparent, parent, root, RB_RED);
            break;
        }
    }
}

static inline struct rb_node *__rb_erase_node(struct rb_node *node, struct rb_node **root)
{
    struct rb_node *child = node->rb_right;
    struct rb_node *tmp = node->rb_left;
    struct rb_node *parent, *rebalance;
    unsigned long pc;

    if (!tmp) {
        /*
         * Case 1: node to erase has no more than 1 child (easy!)
         *
         * Note that if there is one child it must be red due to 5)
         * and node must be black due to 4). We adjust colors locally
         * so as to bypass __rb_erase_color() later on.
         */
        pc = node->__rb_parent_color;
        parent = __rb_parent(pc);
        __rb_change_child(node, child, parent, root);
        if (child) {
            child->__rb_parent_color = pc;
            rebalance = NULL;
        } else
            rebalance = __rb_is_black(pc) ? parent : NULL;
        tmp = parent;
    } else if (!child) {
        /* Still case 1, but this time the child is node->rb_left */
        tmp->__rb_parent_color = pc = node->__rb_parent_color;
        parent = __rb_parent(pc);
        __rb_change_child(node, tmp, parent, root);
        rebalance = NULL;
        tmp = parent;
    } else {
        struct rb_node *successor = child, *child2;

        tmp = child->rb_left;
        if (!tmp) {
            /*
             * Case 2: node's successor is its right child
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (s)  ->  (x) (c)
             *        \
             *        (c)
             */
            parent = successor;
            child2 = successor->rb_right;
        } else {
            /*
             * Case 3: node's successor is leftmost under
             * node's right child subtree
             *
             *    (n)          (s)
             *    / \          / \
             *  (x) (y)  ->  (x) (y)
             *      /            /
             *    (p)          (p)
             *    /            /
             *  (s)          (c)
             *    \
             *    (c)
             */
            do {
                parent = successor;
                successor = tmp;
                tmp = tmp->rb_left;
            } while (tmp);
            child2 = successor->rb_right;
            parent->rb_left = child2;
            successor->rb_right = child;
            rb_set_parent(child, successor);
        }

        tmp = node->rb_left;
        successor->rb_left = tmp;
        rb_set_parent(tmp, successor);

        pc = node->__rb_parent_color;
        tmp = __rb_parent(pc);
        __rb_change_child(node, successor, tmp, root);

        if (child2) {
            successor->__rb_parent_color = pc;
            rb_set_parent_color(child2, parent, RB_BLACK);
            rebalance = NULL;
        } else {
            unsigned long pc2 = successor->__rb_parent_color;
            successor->__rb_parent_color = pc;
            rebalance = __rb_is_black(pc2) ? parent : NULL;
        }
        tmp = successor;
    }

    return rebalance;
}

static inline void __rb_erase_color(struct rb_node *parent, struct rb_node **root)
{
    struct rb_node *node = NULL, *sibling, *tmp1, *tmp2;

    while (1) {
        /*
         * Loop invariants:
         * - node is black (or NULL on first iteration)
         * - node is not the root (parent is not NULL)
         * - All leaf paths going through parent and node have a
         *   black node count that is 1 lower than other leaf paths.
         */
        sibling = parent->rb_right;
        if (node != sibling) {	/* node == parent->rb_left */
            if (rb_is_red(sibling)) {
                /*
                 * Case 1 - left rotate at parent
                 *
                 *     P               S
                 *    / \             / \
                 *   N   s    -->    p   Sr
                 *      / \         / \
                 *     Sl  Sr      N   Sl
                 */
                tmp1 = sibling->rb_left;
                parent->rb_right = tmp1;
                sibling->rb_left = parent;
                rb_set_parent_color(tmp1, parent, RB_BLACK);
                __rb_rotate_set_parents(parent, sibling, root,
                            RB_RED);
                sibling = tmp1;
            }
            tmp1 = sibling->rb_right;
            if (!tmp1 || rb_is_black(tmp1)) {
                tmp2 = sibling->rb_left;
                if (!tmp2 || rb_is_black(tmp2)) {
                    /*
                     * Case 2 - sibling color flip
                     * (p could be either color here)
                     *
                     *    (p)           (p)
                     *    / \           / \
                     *   N   S    -->  N   s
                     *      / \           / \
                     *     Sl  Sr        Sl  Sr
                     *
                     * This leaves us violating 5) which
                     * can be fixed by flipping p to black
                     * if it was red, or by recursing at p.
                     * p is red when coming from Case 1.
                     */
                    rb_set_parent_color(sibling, parent,
                                RB_RED);
                    if (rb_is_red(parent))
                        rb_set_black(parent);
                    else {
                        node = parent;
                        parent = rb_parent(node);
                        if (parent)
                            continue;
                    }
                    break;
                }
                /*
                 * Case 3 - right rotate at sibling
                 * (p could be either color here)
                 *
                 *   (p)           (p)
                 *   / \           / \
                 *  N   S    -->  N   sl
                 *     / \             \
                 *    sl  Sr            S
                 *                       \
                 *                        Sr
                 *
                 * Note: p might be red, and then both
                 * p and sl are red after rotation(which
                 * breaks property 4). This is fixed in
                 * Case 4 (in __rb_rotate_set_parents()
                 *         which set sl the color of p
                 *         and set p RB_BLACK)
                 *
                 *   (p)            (sl)
                 *   / \            /  \
                 *  N   sl   -->   P    S
                 *       \        /      \
                 *        S      N        Sr
                 *         \
                 *          Sr
                 */
                tmp1 = tmp2->rb_right;
                sibling->rb_left = tmp1;
                tmp2->rb_right = sibling;
                parent->rb_right = tmp2;
                if (tmp1)
                    rb_set_parent_color(tmp1, sibling,
                                RB_BLACK);
                tmp1 = sibling;
                sibling = tmp2;
            }
            /*
             * Case 4 - left rotate at parent + color flips
             * (p and sl could be either color here.
             *  After rotation, p becomes black, s acquires
             *  p's color, and sl keeps its color)
             *
             *      (p)             (s)
             *      / \             / \
             *     N   S     -->   P   Sr
             *        / \         / \
             *      (sl) sr      N  (sl)
             */
            tmp2 = sibling->rb_left;
            parent->rb_right = tmp2;
            sibling->rb_left = parent;
            rb_set_parent_color(tmp1, sibling, RB_BLACK);
            if (tmp2)
                rb_set_parent(tmp2, parent);
            __rb_rotate_set_parents(parent, sibling, root,
                        RB_BLACK);
            break;
        } else {
            sibling = parent->rb_left;
            if (rb_is_red(sibling)) {
                /* Case 1 - right rotate at parent */
                tmp1 = sibling->rb_right;
                parent->rb_left = tmp1;
                sibling->rb_right = parent;
                rb_set_parent_color(tmp1, parent, RB_BLACK);
                __rb_rotate_set_parents(parent, sibling, root,
                            RB_RED);
                sibling = tmp1;
            }
            tmp1 = sibling->rb_left;
            if (!tmp1 || rb_is_black(tmp1)) {
                tmp2 = sibling->rb_right;
                if (!tmp2 || rb_is_black(tmp2)) {
                    /* Case 2 - sibling color flip */
                    rb_set_parent_color(sibling, parent,
                                RB_RED);
                    if (rb_is_red(parent))
                        rb_set_black(parent);
                    else {
                        node = parent;
                        parent = rb_parent(node);
                        if (parent)
                            continue;
                    }
                    break;
                }
                /* Case 3 - left rotate at sibling */
                tmp1 = tmp2->rb_left;
                sibling->rb_right = tmp1;
                tmp2->rb_left = sibling;
                parent->rb_left = tmp2;
                if (tmp1)
                    rb_set_parent_color(tmp1, sibling,
                                RB_BLACK);
                tmp1 = sibling;
                sibling = tmp2;
            }
            /* Case 4 - right rotate at parent + color flips */
            tmp2 = sibling->rb_right;
            parent->rb_left = tmp2;
            sibling->rb_right = parent;
            rb_set_parent_color(tmp1, sibling, RB_BLACK);
            if (tmp2)
                rb_set_parent(tmp2, parent);
            __rb_rotate_set_parents(parent, sibling, root,
                        RB_BLACK);
            break;
        }
    }
}

/*
 * +++++ linux kernel rbtree interface - end +++++
 */

xset_t* xset_new(size_t key_size,
            xset_compare_cb compare_cb, xset_destroy_cb destroy_cb)
{
    xset_t* xs = malloc(sizeof(xset_t));

    if (xs)
    {
        xs->compare_cb = compare_cb;
        xs->destroy_cb = destroy_cb;
        xs->key_size = key_size;
        xs->size = 0;
        xs->root = NULL;
    }

    return xs;
}

void xset_free(xset_t* xs)
{
    if (xs)
    {
        xset_clear(xs);
        free(xs);
    }
}

xset_iter_t xset_insert(xset_t* xs, const void* pvalue)
{
    xset_iter_t* iter = &xs->root;
    xset_node_t* parent = NULL;
    int result;

    while (*iter)
    {
        result = xs->compare_cb((void*)pvalue, xset_iter_value(*iter));
        parent = *iter;

        if (result > 0)
            iter = &(*iter)->rb_right;
        else if (result < 0)
            iter = &(*iter)->rb_left;
        else
        {
            memcpy(xset_iter_value(*iter), pvalue, xs->key_size);
            return *iter;
        }
    }

    xset_node_t* nwnd = malloc(sizeof(xset_node_t) + xs->key_size);

    if (nwnd)
    {
        memcpy(xset_iter_value(nwnd), pvalue, xs->key_size);

        __rb_insert_node(nwnd, parent, iter);
        __rb_insert_color(nwnd, &xs->root);
        ++xs->size;

        return nwnd;
    }

    return NULL;
}

xset_iter_t xset_find(xset_t* xs, const void* pvalue)
{
    xset_iter_t iter = xs->root;
    int result;

    while (iter)
    {
        result = xs->compare_cb((void*)pvalue, xset_iter_value(iter));

        if (result > 0)
            iter = iter->rb_right;
        else if (result < 0)
            iter = iter->rb_left;
        else
            return iter;
    }

    return NULL;
}

void xset_erase(xset_t* xs, xset_iter_t iter)
{
    xset_iter_t* root = &xs->root;
    xset_node_t* rebalance;

    rebalance = __rb_erase_node(iter, root);
    if (rebalance)
        __rb_erase_color(rebalance, root);

    if (xs->destroy_cb)
        xs->destroy_cb(xset_iter_value(iter));
    free(iter);

    --xs->size;
}

void xset_clear(xset_t* xs)
{
    xset_iter_t iter = xset_begin(xs);
    xset_iter_t next;

    // TODO...
    if (xs->destroy_cb) while (iter)
    {
        next = xset_iter_next(iter);
        xs->destroy_cb(xset_iter_value(iter));
        free(iter);
        iter = next;
    }
    else while (iter)
    {
        next = xset_iter_next(iter);
        free(iter);
        iter = next;
    }

    xs->size = 0;
    xs->root = NULL;
}

xset_iter_t xset_begin(xset_t* xs)
{
    xset_iter_t iter = xs->root;

    if (!iter) return NULL;

    while (iter->rb_left)
        iter = iter->rb_left;
    return iter;
}

xset_iter_t xset_iter_next(xset_iter_t iter)
{
    xset_node_t* parent;

    /*
     * If we have a right-hand child, go down and then left as far
     * as we can.
     */
    if (iter->rb_right)
    {
        iter = iter->rb_right;
        while (iter->rb_left)
            iter = iter->rb_left;
        return iter;
    }

    /*
     * No right-hand children. Everything down and left is smaller than us,
     * so any 'next' node must be in the general direction of our parent.
     * Go up the tree; any time the ancestor is a right-hand child of its
     * parent, keep going up. First time it's a left-hand child of its
     * parent, said parent is our 'next' node.
     */
    while ((parent = rb_parent(iter)) && iter == parent->rb_right)
        iter = parent;

    return parent;
}

xset_iter_t xset_rbegin(xset_t* xs)
{
    xset_iter_t iter = xs->root;

    if (!iter) return NULL;

    while (iter->rb_right)
        iter = iter->rb_right;
    return iter;
}

xset_iter_t xset_riter_next(xset_iter_t iter)
{
    xset_node_t* parent;

    /*
     * If we have a left-hand child, go down and then right as far
     * as we can.
     */
    if (iter->rb_left)
    {
        iter = iter->rb_left;
        while (iter->rb_right)
            iter = iter->rb_right;
        return iter;
    }

    /*
     * No left-hand children. Go up till we find an ancestor which
     * is a right-hand child of its parent.
     */
    while ((parent = rb_parent(iter)) && iter == parent->rb_left)
        iter = parent;

    return parent;
}