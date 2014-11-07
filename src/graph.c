/*
    * file: graph.c
    * author: zhangbo
    * time: 2014.11.7
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

#include "all.h"

static inline int __link_insert(
    struct rb_root *root, struct graph_node_link_t *data
);


static inline struct graph_node_link_t * __link_get(
    struct rb_root * root, char *node_name
);


static inline void __free_link(struct graph_node_link_t * _link){
    free(_link);
    _link = NULL;
}

// grabge collection of link structure
static inline struct graph_node_link_t * __create_link(char *node_name){

    struct graph_node_link_t * _rlink = \
        (struct graph_node_link_t *)malloc(sizeof(struct graph_node_link_t));

    if(_rlink == (void *)NULL){
        return NULL;
    }

    strncpy(_rlink->node_name, node_name, strlen(node_name));
    _rlink->free = &__free_link;
    return _rlink;
}


struct graph_node_t * create_node(
    char *node_name, void *data, uint64_t data_len
){
    if(node_name == NULL)
        return NULL;

    struct graph_node_t * _node = \
        (struct graph_node_t *)malloc(sizeof(struct graph_node_t));

    if(_node == NULL)
        return NULL;

    _node->_rbt_links = RB_ROOT;
    _node->data = data;
    _node->data_len = data_len;
    _node->link_num = 0;
    _node->free = delete_node;
    strncpy(_node->node_name, node_name, GRAPH_NODE_NAME_LEN);

    return _node;
}


void delete_node(struct graph_node_t * pg){
    rb_link_tree_iterate(pg, __free_link);
    free(pg);
}
// create a link with point to node dest which argument give
//
int build_link(
    struct graph_node_t *nodes, struct graph_node_t * noded
){
    char * node_name = noded->node_name;

    if(strcmp(nodes->node_name, noded->node_name) == 0){
        return -1;
    }
    struct graph_node_link_t * _rlink = __create_link(node_name);

    if(_rlink == (void *)NULL){
        return -1;
    }

    _rlink->link = noded;
    nodes->link_num += 1;

    return __link_insert(&(nodes->_rbt_links), _rlink);
}

// create an double linke between node source and node dest
int build_double_link(
    struct graph_node_t *nodes, struct graph_node_t *noded)
{
    if(build_link(nodes, noded) == 0 && build_link(noded, nodes) == 0){
        return 0;
    }
    return -1;
}


static void rb_tree_erase_speed(struct rb_root *root, struct rb_node *node)
{
    rb_erase(node, (struct rb_root *)root);
    return;
}


int destory_link(
    struct graph_node_t * nodes, struct graph_node_t * noded)
{
    char * node_name = noded->node_name;
    if(node_name == NULL){
        return -1;
    }

    struct graph_node_link_t * _link = __link_get(&nodes->_rbt_links, node_name);
    if(_link == NULL){
        printf("[DEBUG] No found link %s in link tree\n", node_name);
        return -1;
    }
    rb_tree_erase_speed(&nodes->_rbt_links, &_link->rb_node);
    _link->free(_link);

    nodes->link_num -= 1;

    return 0;
}


int destory_double_link(
    struct graph_node_t * nodes, struct graph_node_t * noded)
{
    char * node_name = noded->node_name;
    if(node_name == NULL){
        return -1;
    }

    struct graph_node_link_t * _links = __link_get(&nodes->_rbt_links, node_name);
    if(_links == NULL){
        return -1;
    }
    // remove link from source node
    rb_tree_erase_speed(&nodes->_rbt_links, &_links->rb_node);


    node_name = nodes->node_name;
    if(node_name == NULL){
        return -1;
    }
    struct graph_node_link_t * _linkd = __link_get(&noded->_rbt_links, node_name);
    if(_linkd == NULL){
        return -1;
    }
    // remove link from dest node
    rb_tree_erase_speed(&noded->_rbt_links, &_linkd->rb_node);

    _links->free(_links);
    _linkd->free(_linkd);

    return 0;
}


// get link with link to node_name
static inline struct graph_node_link_t * __link_get(
    struct rb_root * root, char *node_name
){
    struct rb_node *node = root->rb_node;

    while (node) {
        struct graph_node_link_t * data = (struct graph_node_link_t*)CONTAINER_OF(
            node, struct graph_node_link_t, rb_node
        );

        int c = strcmp(node_name, data->node_name);
        if (c < 0)
            node = node->rb_left;
        else if (c > 0)
            node = node->rb_right;
        else
            return data;
    }

    return NULL;
}


static inline int __link_insert(
    struct rb_root *root, struct graph_node_link_t *data
){

    struct rb_node **tmp = &(root->rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        struct graph_node_link_t *this = (struct graph_node_link_t *)CONTAINER_OF(
            *tmp, struct graph_node_link_t, rb_node
        );

        parent = *tmp;
        int c = strcmp(data->node_name, this->node_name);
        if(c < 0)
            tmp = &((*tmp)->rb_left);
        else if (c > 0)
            tmp = &((*tmp)->rb_right);
        else
            return -1;
    }

    /* Add new node and rebalance tree. */
    rb_link_node(&data->rb_node, parent, tmp);
    rb_insert_color(&data->rb_node, root);

    return 0;
}


static void __rb_link_tree_iterate(
    struct graph_node_t * node,
    struct rb_node * start,
    void (* callback)(struct graph_node_link_t *node)
){
    if(!start){
        return;
    }
    if(start->rb_left) {
        __rb_link_tree_iterate(node, start->rb_left, callback);
    }
    if(start->rb_right) {
        __rb_link_tree_iterate(node, start->rb_right, callback);
    }

    struct graph_node_link_t * link = (struct graph_node_link_t *)CONTAINER_OF(
        start, struct graph_node_link_t, rb_node
    );

    if(callback != NULL){
        callback(link);
    }
}


void rb_link_tree_iterate(
    struct graph_node_t * node,
    void (* callback)(struct graph_node_link_t *node)
){
    return __rb_link_tree_iterate(node, node->_rbt_links.rb_node, callback);
}

