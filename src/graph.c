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

static void
rb_tree_erase_speed(struct rb_root *root, struct rb_node *node);


static inline int
__add_node(struct graph_t * _g, struct graph_node_t * _n);


static inline struct graph_node_t *
__node_get(struct rb_root * root, char *node_name);


static void __rb_node_tree_iterate_free(
    struct graph_t * graph,
    struct rb_node * start
){
    if(!start){
        return;
    }
    if(start->rb_left) {
        __rb_node_tree_iterate_free(graph, start->rb_left);
        start->rb_left = 0;
    }
    if(start->rb_right) {
        __rb_node_tree_iterate_free(graph, start->rb_right);
        start->rb_right = 0;
    }

    struct graph_node_t * node = (struct graph_node_t *)CONTAINER_OF(
        start, struct graph_node_t , rb_node
    );

    memset(start, 0x0, sizeof(struct rb_node));
    if(node->free) node->free(node);
}


void __free_graph_nodes(struct graph_t * graph){
    __rb_node_tree_iterate_free(graph, graph->node_root.rb_node);
    free(graph);
}


static void __rb_node_tree_iterate(
    struct graph_t * graph,
    struct rb_node * start,
    uint64_t deep,
    void (* callback)(struct graph_node_t * node)
){
    if(!start || deep <= 0){
        return;
    }
    if(start->rb_left) {
        __rb_node_tree_iterate(
            graph, start->rb_left, deep - 1, callback
        );
    }
    if(start->rb_right) {
        __rb_node_tree_iterate(
            graph, start->rb_right, deep - 1, callback
        );
    }

    struct graph_node_t * node = (struct graph_node_t *)CONTAINER_OF(
        start, struct graph_node_t , rb_node
    );

    if(callback) callback(node);
}


void iterate_graph_nodes(
    struct graph_t * graph,
    struct graph_node_t * start,
    uint64_t deep,
    void (* callback)(struct graph_node_t * node)
){
    if(__node_get(&graph->node_root, start->node_name) == NULL){
        return;
    }
    __rb_node_tree_iterate(
        graph, &start->rb_node, deep, callback
    );
}


static inline int __node_insert(
    struct rb_root *root, struct graph_node_t *data
){

    struct rb_node **tmp = &(root->rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        struct graph_node_t *this = (struct graph_node_t *)CONTAINER_OF(
            *tmp, struct graph_node_t, rb_node
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


static inline int __add_node(struct graph_t * _g, struct graph_node_t * _n){
    if(_g == NULL ||_n == NULL){
        return -1;
    }
    if(__node_insert(&_g->node_root, _n) == 0){
        _g->node_num += 1;
        return 0;
    }
    return -1;
}


static inline struct graph_node_t * __node_get(
    struct rb_root * root, char *node_name
){
    struct rb_node *node = root->rb_node;

    while (node) {
        struct graph_node_t * data = (struct graph_node_t *)CONTAINER_OF(
            node, struct graph_node_t, rb_node
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


struct graph_node_t * get_node(
    struct graph_t * graph, char *node_name
){
    if(graph == NULL || graph->node_num == 0){
        return NULL;
    }
    return __node_get(&graph->node_root, node_name);
}


static inline int __rm_node(
    struct graph_t * _g,
    char *name,
    void (* callback)(struct graph_node_t * ))
{
    if(name == NULL){
        return -1;
    }

    struct graph_node_t * _node = __node_get(&_g->node_root, name);
    if(_node == NULL){
        return -1;
    }
    rb_tree_erase_speed(&_g->node_root, &_node->rb_node);
    _g->node_num -= 1;
    if(callback) callback(_node);

    return 0;
}


struct graph_t * graph(){
    struct graph_t * _g = (struct graph_t *)malloc(sizeof(struct graph_t));
    if(_g == NULL){
        LOG_ERROR("create graph failed, malloc of memory error");
        return NULL;
    }
    _g->node_root = RB_ROOT;
    _g->node_num = 0;

    _g->add_node = &__add_node;
    _g->rm_node = &__rm_node;
    _g->free = &__free_graph_nodes;
    return _g;
}


static inline uint64_t __get_link_weight(struct graph_node_link_t * _l){
    return _l->weight;
}


static inline void __set_link_weight(
    struct graph_node_link_t * _l, uint64_t weight
){
    _l->weight = weight;
}


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
        (struct graph_node_link_t *)calloc(1, sizeof(struct graph_node_link_t));

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
        (struct graph_node_t *)calloc(1, sizeof(struct graph_node_t));

    if(_node == NULL)
        return NULL;

    _node->_rbt_links = RB_ROOT;
    init_queue_head(&_node->node_queue);
    _node->data = data;
    _node->data_len = data_len;
    _node->link_num = 0;
    _node->free = delete_node;
    strncpy(_node->node_name, node_name, strlen(node_name));

    return _node;
}


void delete_node(struct graph_node_t * pg){
    rb_link_tree_iterate(pg, __free_link);
    free(pg);
}
// create a link with point to node dest which argument give

int build_link(
    struct graph_node_t *nodes,
    struct graph_node_t *noded,
    uint64_t weight
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
    __set_link_weight(_rlink, weight);
    nodes->link_num += 1;

    queue_add_tail(&_rlink->list_node, &nodes->node_queue);
    return __link_insert(&(nodes->_rbt_links), _rlink);
}

// create an double linke between node source and node dest
int build_double_link(
    struct graph_node_t *nodes,
    struct graph_node_t *noded,
    uint64_t weight1,   // node source to node dest
    uint64_t weight2)   // node dest to node source
{
    if(build_link(nodes, noded, weight1) == 0 && \
       build_link(noded, nodes, weight2) == 0)
    {
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
        LOG_DEBUG("No found link %s in link tree", node_name);
        return -1;
    }
    rb_tree_erase_speed(&nodes->_rbt_links, &_link->rb_node);
    queue_del(&_link->list_node);
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
    void (* callback)(struct graph_node_link_t *link)
){
    return __rb_link_tree_iterate(node, node->_rbt_links.rb_node, callback);
}


static void __rb_link_tree_iterate_deep(
    struct graph_node_t * node,
    struct rb_node * start,
    uint64_t deep,
    void (* callback)(struct graph_node_link_t *node)
){
    if(!start || deep <= 0){
        return;
    }
    if(start->rb_left) {
        __rb_link_tree_iterate_deep(
            node, start->rb_left, deep - 1, callback
        );
    }
    if(start->rb_right) {
        __rb_link_tree_iterate_deep(
            node, start->rb_right, deep - 1, callback
        );
    }

    struct graph_node_link_t * link = (struct graph_node_link_t *)CONTAINER_OF(
        start, struct graph_node_link_t, rb_node
    );

    if(callback != NULL){
        callback(link);
    }
}

void rb_link_tree_iterate_deep(
    struct graph_node_t * node,
    uint64_t deep,
    void (* callback)(struct graph_node_link_t *link)
){
    return __rb_link_tree_iterate_deep(
        node, node->_rbt_links.rb_node, deep, callback
    );
}


void links_iterate(
    struct graph_node_t * node,
    uint64_t len,
    void (* callback)(struct graph_node_link_t *link)
){
    uint64_t i = 0;
    struct list_head *cursor;
    queue_for_each(cursor, &node->node_queue){
        if(cursor == NULL) break;
        struct graph_node_link_t * link = (struct graph_node_link_t *)CONTAINER_OF(
            cursor, struct graph_node_link_t, list_node
        );
        i++;
        if(i > len || link == NULL) break;
        callback(link);
    }
}
