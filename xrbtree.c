#include <stdlib.h>
#include <string.h>

#include "xrbtree.h"

/*
 * +++++ linux kernel rbtree interface - start +++++
 */

#define	RB_RED              0
#define	RB_BLACK            1

#define __rb_parent(pc)     ((xrbt_node_t *)(pc & ~3))
#define rb_parent(rb)       ((xrbt_node_t *)((rb)->rb_parent_color & ~3))

#define __rb_color(pc)      ((pc) & 1)
#define __rb_is_black(pc)   __rb_color(pc)
#define __rb_is_red(pc)     (!__rb_color(pc))
#define rb_color(rb)        __rb_color((rb)->rb_parent_color)
#define rb_is_red(rb)       __rb_is_red((rb)->rb_parent_color)
#define rb_is_black(rb)     __rb_is_black((rb)->rb_parent_color)

static inline void rb_set_parent(xrbt_node_t *rb, xrbt_node_t *p)
{
    rb->rb_parent_color = rb_color(rb) | (unsigned long)p;
}

static inline void rb_set_parent_color(xrbt_node_t *rb,
                        xrbt_node_t *p, int color)
{
    rb->rb_parent_color = (unsigned long)p | color;
}

static inline void rb_set_black(xrbt_node_t *rb)
{
    rb->rb_parent_color |= RB_BLACK;
}

static inline xrbt_node_t *rb_red_parent(xrbt_node_t *red)
{
    return (xrbt_node_t *)red->rb_parent_color;
}

static inline void __rb_change_child(xrbt_node_t *old, xrbt_node_t *new,
            xrbt_node_t *parent, xrbt_node_t **root)
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
static inline void __rb_rotate_set_parents(xrbt_node_t *old, xrbt_node_t *new,
            xrbt_node_t **root, int color)
{
    xrbt_node_t *parent = rb_parent(old);
    new->rb_parent_color = old->rb_parent_color;
    rb_set_parent_color(old, new, color);
    __rb_change_child(old, new, parent, root);
}

/*
 * Insert 'node' into 'rb_link', below 'parent'
 */
static inline void __rb_insert_node(xrbt_node_t *node, xrbt_node_t *parent,
                xrbt_node_t **rb_link)
{
    node->rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

static inline void __rb_insert_color(xrbt_node_t *node, xrbt_node_t **root)
{
    xrbt_node_t *parent = rb_red_parent(node), *gparent, *tmp;

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

static inline xrbt_node_t *__rb_erase_node(xrbt_node_t *node, xrbt_node_t **root)
{
    xrbt_node_t *child = node->rb_right;
    xrbt_node_t *tmp = node->rb_left;
    xrbt_node_t *parent, *rebalance;
    unsigned long pc;

    if (!tmp) {
        /*
         * Case 1: node to erase has no more than 1 child (easy!)
         *
         * Note that if there is one child it must be red due to 5)
         * and node must be black due to 4). We adjust colors locally
         * so as to bypass __rb_erase_color() later on.
         */
        pc = node->rb_parent_color;
        parent = __rb_parent(pc);
        __rb_change_child(node, child, parent, root);
        if (child) {
            child->rb_parent_color = pc;
            rebalance = NULL;
        } else
            rebalance = __rb_is_black(pc) ? parent : NULL;
        tmp = parent;
    } else if (!child) {
        /* Still case 1, but this time the child is node->rb_left */
        tmp->rb_parent_color = pc = node->rb_parent_color;
        parent = __rb_parent(pc);
        __rb_change_child(node, tmp, parent, root);
        rebalance = NULL;
        tmp = parent;
    } else {
        xrbt_node_t *successor = child, *child2;

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

        pc = node->rb_parent_color;
        tmp = __rb_parent(pc);
        __rb_change_child(node, successor, tmp, root);

        if (child2) {
            successor->rb_parent_color = pc;
            rb_set_parent_color(child2, parent, RB_BLACK);
            rebalance = NULL;
        } else {
            unsigned long pc2 = successor->rb_parent_color;
            successor->rb_parent_color = pc;
            rebalance = __rb_is_black(pc2) ? parent : NULL;
        }
        tmp = successor;
    }

    return rebalance;
}

static inline void __rb_erase_color(xrbt_node_t *parent, xrbt_node_t **root)
{
    xrbt_node_t *node = NULL, *sibling, *tmp1, *tmp2;

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

xrbt_t* xrbt_new(size_t data_size, xrbt_compare_cb compare_cb,
                xrbt_destroy_cb destroy_cb)
{
    xrbt_t* tr = malloc(sizeof(xrbt_t));

    if (tr)
    {
        tr->compare_cb = compare_cb;
        tr->destroy_cb = destroy_cb;
        tr->data_size = data_size;
        tr->size = 0;
#ifndef XRBT_NO_CACHE
        tr->cache = NULL;
#endif
        tr->root = NULL;
    }

    return tr;
}

void xrbt_free(xrbt_t* tr)
{
    if (tr)
    {
        xrbt_clear(tr);
#ifndef XRBT_NO_CACHE
        xrbt_cache_free(tr);
#endif
        free(tr);
    }
}

xrbt_iter_t xrbt_insert(xrbt_t* tr, const void* pdata)
{
    xrbt_iter_t* iter = &tr->root;
    xrbt_node_t* parent = NULL;
    xrbt_node_t* nwnd;
    int result;

    while (*iter)
    {
        result = tr->compare_cb(xrbt_iter_data(*iter), (void*)pdata);
        parent = *iter;

        if (result > 0)
            iter = &(*iter)->rb_right;
        else if (result < 0)
            iter = &(*iter)->rb_left;
        else
            return *iter;
    }

#ifndef XRBT_NO_CACHE
    if (tr->cache)
    {
        nwnd = tr->cache;
        tr->cache = nwnd->rb_right;
    }
    else
    {
#endif
        nwnd = malloc(sizeof(xrbt_node_t) + tr->data_size);
        if (!nwnd)
            return NULL;
#ifndef XRBT_NO_CACHE
    }
#endif
    memcpy(xrbt_iter_data(nwnd), pdata, tr->data_size);

    __rb_insert_node(nwnd, parent, iter);
    __rb_insert_color(nwnd, &tr->root);

    ++tr->size;

    return nwnd;
}

xrbt_iter_t xrbt_find(xrbt_t* tr, const void* pdata)
{
    xrbt_iter_t iter = tr->root;
    int result;

    while (iter)
    {
        result = tr->compare_cb(xrbt_iter_data(iter), (void*)pdata);

        if (result > 0)
            iter = iter->rb_right;
        else if (result < 0)
            iter = iter->rb_left;
        else
            return iter;
    }

    return NULL;
}

void xrbt_erase(xrbt_t* tr, xrbt_iter_t iter)
{
    xrbt_iter_t* root = &tr->root;
    xrbt_node_t* rebalance;

    rebalance = __rb_erase_node(iter, root);
    if (rebalance)
        __rb_erase_color(rebalance, root);

    if (tr->destroy_cb)
        tr->destroy_cb(xrbt_iter_data(iter));

#ifndef XRBT_NO_CACHE
    iter->rb_right = tr->cache;
    tr->cache = iter;
#else
    free(iter);
#endif

    --tr->size;
}

#ifndef XRBT_NO_CACHE
void xrbt_cache_free(xrbt_t* tr)
{
    xrbt_node_t* c = tr->cache;

    while (c)
    {
        tr->cache = c->rb_right;
        free(c);
        c = tr->cache;
    }
}
#endif

void xrbt_clear(xrbt_t* tr)
{
    xrbt_iter_t iter = tr->root;
    xrbt_iter_t parent;

    while (iter)
    {
        if (iter->rb_left)
        {
            parent = iter;
            iter = iter->rb_left;
            parent->rb_left = NULL;
        }
        else if (iter->rb_right)
        {
            parent = iter;
            iter = iter->rb_right;
            parent->rb_right = NULL;
        }
        else
        {
            parent = rb_parent(iter);
            if (tr->destroy_cb)
                tr->destroy_cb(xrbt_iter_data(iter));
            free(iter);
            iter = parent;
        }
    }

    tr->size = 0;
    tr->root = NULL;
}

xrbt_iter_t xrbt_begin(xrbt_t* tr)
{
    xrbt_iter_t iter = tr->root;

    if (!iter) return NULL;

    while (iter->rb_left)
        iter = iter->rb_left;
    return iter;
}

xrbt_iter_t xrbt_iter_next(xrbt_iter_t iter)
{
    xrbt_node_t* parent;

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

xrbt_iter_t xrbt_rbegin(xrbt_t* tr)
{
    xrbt_iter_t iter = tr->root;

    if (!iter) return NULL;

    while (iter->rb_right)
        iter = iter->rb_right;
    return iter;
}

xrbt_iter_t xrbt_riter_next(xrbt_iter_t iter)
{
    xrbt_node_t* parent;

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