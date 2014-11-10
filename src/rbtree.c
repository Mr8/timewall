/*
    * file: rbtree.c
    * author: zhangbo
    * time: 2014.11.6
    * Copyright (c) 2014, zhangbo <zhangbolinux@sina.con>.

    * Redistribution and use in source and binary forms, with or without
    * modification, are permitted provided that the following conditions are
    * met:
    *
    *  1. Redistributions of source code must retain the above copyright
    *     notice, this list of conditions and the following disclaimer.
    *
    *  2. Redistributions in binary form must reproduce the above copyright
    *     notice, this list of conditions and the following disclaimer in the
    *     documentation and/or other materials provided with the distribution.
    *
    * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* rbtree implement
 */

#define EXPORT_SYMBOL(X)
#define TRUE 1
#define FALSE

#include <memory.h>
#include <stdlib.h>
#include "rbtree.h"
#include <sys/types.h>

static void __rb_rotate_left(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *right = node->rb_right;
    struct rb_node *parent = rb_parent(node);

    if ((node->rb_right = right->rb_left))
        rb_set_parent(right->rb_left, node);
    right->rb_left = node;

    rb_set_parent(right, parent);

    if (parent)
    {
        if (node == parent->rb_left)
            parent->rb_left = right;
        else
            parent->rb_right = right;
    }
    else
        root->rb_node = right;
    rb_set_parent(node, right);
}

static void __rb_rotate_right(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *left = node->rb_left;
    struct rb_node *parent = rb_parent(node);

    if ((node->rb_left = left->rb_right))
        rb_set_parent(left->rb_right, node);
    left->rb_right = node;

    rb_set_parent(left, parent);

    if (parent)
    {
        if (node == parent->rb_right)
            parent->rb_right = left;
        else
            parent->rb_left = left;
    }
    else
        root->rb_node = left;
    rb_set_parent(node, left);
}

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *parent, *gparent;

    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        if (parent == gparent->rb_left)
        {
            {
                register struct rb_node *uncle = gparent->rb_right;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_right == node)
            {
                register struct rb_node *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_right(gparent, root);
        } else {
            {
                register struct rb_node *uncle = gparent->rb_left;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_left == node)
            {
                register struct rb_node *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            rb_set_black(parent);
            rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    rb_set_black(root->rb_node);
}
EXPORT_SYMBOL(rb_insert_color);

static void __rb_erase_color(struct rb_node *node, struct rb_node *parent,
                 struct rb_root *root)
{
    struct rb_node *other;

    while ((!node || rb_is_black(node)) && node != root->rb_node)
    {
        if (parent->rb_left == node)
        {
            other = parent->rb_right;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->rb_right;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_right || rb_is_black(other->rb_right))
                {
                    rb_set_black(other->rb_left);
                    rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_right);
                __rb_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        }
        else
        {
            other = parent->rb_left;
            if (rb_is_red(other))
            {
                rb_set_black(other);
                rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->rb_left;
            }
            if ((!other->rb_left || rb_is_black(other->rb_left)) &&
                (!other->rb_right || rb_is_black(other->rb_right)))
            {
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->rb_left || rb_is_black(other->rb_left))
                {
                    rb_set_black(other->rb_right);
                    rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->rb_left;
                }
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->rb_left);
                __rb_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

void rb_erase(struct rb_node *node, struct rb_root *root)
{
    struct rb_node *child, *parent;
    int color;

    if (!node->rb_left)
        child = node->rb_right;
    else if (!node->rb_right)
        child = node->rb_left;
    else
    {
        struct rb_node *old = node, *left;

        node = node->rb_right;
        while ((left = node->rb_left) != NULL)
            node = left;

        if (rb_parent(old)) {
            if (rb_parent(old)->rb_left == old)
                rb_parent(old)->rb_left = node;
            else
                rb_parent(old)->rb_right = node;
        } else
            root->rb_node = node;

        child = node->rb_right;
        parent = rb_parent(node);
        color = rb_color(node);

        if (parent == old) {
            parent = node;
        } else {
            if (child)
                rb_set_parent(child, parent);
            parent->rb_left = child;

            node->rb_right = old->rb_right;
            rb_set_parent(old->rb_right, node);
        }

        node->rb_parent_color = old->rb_parent_color;
        node->rb_left = old->rb_left;
        rb_set_parent(old->rb_left, node);

        goto color;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child)
        rb_set_parent(child, parent);
    if (parent)
    {
        if (parent->rb_left == node)
            parent->rb_left = child;
        else
            parent->rb_right = child;
    }
    else
        root->rb_node = child;

 color:
    if (color == RB_BLACK)
        __rb_erase_color(child, parent, root);
}
EXPORT_SYMBOL(rb_erase);

static void rb_augment_path(struct rb_node *node, rb_augment_f func, void *data)
{
    struct rb_node *parent;

up:
    func(node, data);
    parent = rb_parent(node);
    if (!parent)
        return;

    if (node == parent->rb_left && parent->rb_right)
        func(parent->rb_right, data);
    else if (parent->rb_left)
        func(parent->rb_left, data);

    node = parent;
    goto up;
}

void rb_augment_insert(struct rb_node *node, rb_augment_f func, void *data)
{
    if (node->rb_left)
        node = node->rb_left;
    else if (node->rb_right)
        node = node->rb_right;

    rb_augment_path(node, func, data);
}
EXPORT_SYMBOL(rb_augment_insert);

struct rb_node *rb_augment_erase_begin(struct rb_node *node)
{
    struct rb_node *deepest;

    if (!node->rb_right && !node->rb_left)
        deepest = rb_parent(node);
    else if (!node->rb_right)
        deepest = node->rb_left;
    else if (!node->rb_left)
        deepest = node->rb_right;
    else {
        deepest = rb_next(node);
        if (deepest->rb_right)
            deepest = deepest->rb_right;
        else if (rb_parent(deepest) != node)
            deepest = rb_parent(deepest);
    }

    return deepest;
}
EXPORT_SYMBOL(rb_augment_erase_begin);

void rb_augment_erase_end(struct rb_node *node, rb_augment_f func, void *data)
{
    if (node)
        rb_augment_path(node, func, data);
}
EXPORT_SYMBOL(rb_augment_erase_end);

struct rb_node *rb_first(const struct rb_root *root)
{
    struct rb_node  *n;

    n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_left)
        n = n->rb_left;
    return n;
}
EXPORT_SYMBOL(rb_first);

struct rb_node *rb_last(const struct rb_root *root)
{
    struct rb_node  *n;

    n = root->rb_node;
    if (!n)
        return NULL;
    while (n->rb_right)
        n = n->rb_right;
    return n;
}
EXPORT_SYMBOL(rb_last);

struct rb_node *rb_next(const struct rb_node *node)
{
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    if (node->rb_right) {
        node = node->rb_right;
        while (node->rb_left)
            node=node->rb_left;
        return (struct rb_node *)node;
    }

    while ((parent = rb_parent(node)) && node == parent->rb_right)
        node = parent;

    return parent;
}
EXPORT_SYMBOL(rb_next);

struct rb_node *rb_prev(const struct rb_node *node)
{
    struct rb_node *parent;

    if (rb_parent(node) == node)
        return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->rb_left) {
        node = node->rb_left;
        while (node->rb_right)
            node=node->rb_right;
        return (struct rb_node *)node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = rb_parent(node)) && node == parent->rb_left)
        node = parent;

    return parent;
}
EXPORT_SYMBOL(rb_prev);

void rb_replace_node(struct rb_node *victim, struct rb_node *n,
             struct rb_root *root)
{
    struct rb_node *parent = rb_parent(victim);

    /* Set the surrounding nodes to point to the replacement */
    if (parent) {
        if (victim == parent->rb_left)
            parent->rb_left = n;
        else
            parent->rb_right = n;
    } else {
        root->rb_node = n;
    }
    if (victim->rb_left)
        rb_set_parent(victim->rb_left, n);
    if (victim->rb_right)
        rb_set_parent(victim->rb_right, n);

    /* Copy the pointers/colour from the victim to the replacement */
    *n = *victim;
}
EXPORT_SYMBOL(rb_replace_node);

static int valuecmp(int val1, int val2)
{
    return val1 - val2;
}

int rb_tree_insert(struct rb_root *root, struct rb_node_simple *data)
{
    struct rb_node **n = &(root->rb_node), *parent = NULL;

    if(!data)
        return 0;
    /* Figure out where to put n node */
    while (*n) {
        struct rb_node_simple *t = (struct rb_node_simple *)container_of(*n, struct rb_node_simple, node);
        int result = valuecmp(data->key, t->key);

        parent = *n;
        if (result < 0)
            n = &((*n)->rb_left);
        else if (result > 0)
            n = &((*n)->rb_right);
        else
            return 0;
    }

    /* Add n node and rebalance tree. */
    rb_link_node(&data->node, parent, n);
    rb_insert_color(&data->node, root);

    return TRUE;
}

struct rb_node_simple *rb_tree_search(struct rb_root *root, int key)
{
    struct rb_node *nodes = root->rb_node;
    struct rb_node_simple *data = (struct rb_node_simple *)container_of(root->rb_node, struct rb_node_simple, node);
    int result;
    while (nodes) {
        data = (struct rb_node_simple *)container_of(nodes, struct rb_node_simple, node);

        result = valuecmp(key, data->key);

        if (result < 0)
            nodes = nodes->rb_left;
        else if (result > 0)
            nodes = nodes->rb_right;
        else
            return data;
    }
    return 0;
}

void rb_tree_erase(struct rb_root *root, int key)
{
    struct rb_node_simple *ck = rb_tree_search(root, key);
    if(!ck)
        return;
    rb_erase(&ck->node, (struct rb_root *)root);
    return;
}

void rb_tree_erase_speed(struct rb_root *root, struct rb_node *node)
{
    rb_erase(node, (struct rb_root *)root);
    return;
}

/*static void rb_tree_iterate_and_free(struct rb_root *handle, struct rb_node *start)*/
/*{*/
    /*struct rb_node_simple *debug;*/
    /*if(!start)*/
        /*return;*/
    /*if(start->rb_left) {*/
        /*rb_tree_iterate_and_free(handle, start->rb_left);*/
        /*start->rb_left = 0;*/
    /*}*/
    /*if(start->rb_right) {*/
        /*rb_tree_iterate_and_free(handle, start->rb_right);*/
        /*start->rb_right = 0;*/
    /*}*/
    /*memset(start, 0x0, sizeof(struct rb_node));*/
    /*debug = (struct rb_node_simple *)container_of(start, struct rb_node_simple, node);*/
    /*(debug);*/
/*}*/
