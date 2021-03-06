/*
    * file: graph.h
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

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "all.h"

#define GRAPH_NODE_NAME_LEN 256
#define GRAPH graph()


struct graph_node_t {
    struct rb_root    _rbt_links;
    struct rb_node    rb_node;    // a member as node of red-black tree on graph
    struct list_head  node_queue;
    char              node_name[GRAPH_NODE_NAME_LEN];
    uint64_t          link_num;
    void              *data;
    uint64_t          data_len;
    void (* free)(struct graph_node_t *);
};


struct graph_node_link_t {
    struct rb_node      rb_node;
    struct list_head    list_node;
    char                node_name[GRAPH_NODE_NAME_LEN];
    struct graph_node_t *link;
    uint64_t            weight;
    void (* free)(struct graph_node_link_t * _link);
};


struct graph_t {
    struct rb_root      node_root;
    uint64_t            node_num;
    int (* add_node)(struct graph_t *graph, struct graph_node_t *node);
    int (* rm_node)(
        struct graph_t *graph,
        char *name,
        void (* callback)(struct graph_node_t * node)
    );
    void (* free)(struct graph_t *);
};


struct graph_t * graph();
/* create a graph struct
 * @arg:
 *      empty
 * graph with method ->
 * @1.
 *      add_node   [+] add a node to this graph
 * @2.
 *      rm_node    [+] remove a node from this graph
 * @3.
 *      free       [+] free this graph
 */


void iterate_graph_nodes(
    struct graph_t * graph,
    struct graph_node_t * start,
    uint64_t deep,
    void (* callback)(struct graph_node_t * node)
);
/*
 * iterate nodes on this graph,
 * it only for random nodes
 * @arg:
 *      graph    [+] point of the graph
 *      start    [+] which node you want to iterate
 *      deep     [+] how deep you want
 *      callback [+] the callback deal with the *node point
 * */


struct graph_node_t * create_node(char *name, void *data, uint64_t data_len);
/* create a graph node with name and data
 * @arg:
 *      name     [+] this node name
 *      data     [+] point data of this node
 *      data_len [+] data length
 */


struct graph_node_t * get_node(
    struct graph_t * graph, char *node_name
);
/* create a graph node with name and data
 * @arg:
 *      name     [+] this node name
 *      data     [+] point data of this node
 *      data_len [+] data length
 */

void delete_node(struct graph_node_t * pg);
/* delete an graph node
 * @arg:
 *      pg       [+] point to this graph
 */

int build_link(
    struct graph_node_t *nodes,
    struct graph_node_t * noded,
    uint64_t weight
);
/* build an link point to noded
 * @arg:
 *      nodes    [+] node source
 *      noded    [+] node dest
 *      weight   [+] weight of this link
 *
 * @return:
 *      0        [+] success
 *      !0       [+] failed
 */

int destory_link(struct graph_node_t * nodes, struct graph_node_t * noded);
/* destory the link point to noded
 * @arg:
 *      nodes    [+] node source
 *      noded    [+] node dest
 *
 * @return:
 *      0        [+] success
 *      !0       [+] failed
 */

int build_double_link(
    struct graph_node_t *nodes,
    struct graph_node_t *noded,
    uint64_t weight1,
    uint64_t weight2
);
/* build the link between nodes and noded
 * @arg:
 *      nodes    [+] node source
 *      noded    [+] node dest
 *      weight1  [+] weight of source to dest
 *      weight2  [+] weight of dest to source
 *
 * @return:
 *      0        [+] success
 *      !0       [+] failed
 */

int destory_double_link(
    struct graph_node_t * nodes,
    struct graph_node_t * noded
);
/* destory the link between nodes and noded
 * @arg:
 *      nodes    [+] node source
 *      noded    [+] node dest
 *
 * @return:
 *      0        [+] success
 *      !0       [+] failed
 */

void rb_link_tree_iterate(
    struct graph_node_t * node,
    void (* callback)(struct graph_node_link_t *link)
);
/* iterate the link tree
 * @arg:
 *     node      [+] this node
 *     callback  [+] function to deal with the link
 */

void rb_link_tree_iterate_deep(
    struct graph_node_t * node,
    uint64_t deep,
    void (* callback)(struct graph_node_link_t *link)
);

/* iterate the link tree
 * @arg:
 *     node      [+] this node
 *     deep      [+] max deep of iterate
 *     callback  [+] function to deal with the link
 */

void links_iterate(
    struct graph_node_t * node,
    uint64_t len,
    void (* callback)(struct graph_node_link_t *link)
);
#endif

