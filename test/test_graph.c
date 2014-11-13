#include "../src/all.h"
#include <assert.h>

void __callback(struct graph_node_link_t *link){
    printf("[DEBUG]link %s \n", link->node_name);
}

int main(){
    char node_1_name[] = "FORTEST1";
    char node_2_name[] = "FORTEST2";
    char node_3_name[] = "FORTEST3";
    char node_4_name[] = "FORTEST4";

    struct graph_node_t * node1 = create_node(node_1_name, NULL, 0);
    struct graph_node_t * node2 = create_node(node_2_name, NULL, 0);
    struct graph_node_t * node3 = create_node(node_3_name, NULL, 0);
    struct graph_node_t * node4 = create_node(node_4_name, NULL, 0);

    assert(node1->link_num == 0);

    LOG_STD("[DEBUG] create link with between two node\n");

    build_link(node1, node2, 1);
    build_link(node1, node3, 2);
    build_link(node1, node4, 3);

    assert(node1->link_num == 3);

    LOG_STD("[DEBUG] node %s with link %ld\n", node1->node_name, node1->link_num);
    rb_link_tree_iterate(node1, __callback);

    LOG_STD("[DEBUG] destory link between %s <--> %s\n",
        node1->node_name, node2->node_name
    );

    destory_link(node1, node2);

    assert(node1->link_num == 2);

    rb_link_tree_iterate(node1, __callback);
    LOG_STD("[DEBUG] node %s with link %ld\n", node1->node_name, node1->link_num);
    LOG_STD("[DEBUG] destory link between %s <--> %s\n",
        node1->node_name, node3->node_name
    );
    destory_link(node1, node3);

    assert(node1->link_num == 1);

    rb_link_tree_iterate(node1, __callback);
    LOG_STD("[DEBUG] node %s with link %ld\n", node1->node_name, node1->link_num);


    ///////////////////////////////////////////////////////
    ////////test graph///////

    struct graph_t * _G = GRAPH;
    LOG_STD("[DEBUG] add node to graph\n");
    LOG_STD("[DEBUG] insert to graph %d\n",_G->add_node(_G, node1));
    LOG_STD("[DEBUG] insert to graph %d\n",_G->add_node(_G, node1));
    LOG_STD("[DEBUG] insert to graph %d\n",_G->add_node(_G, node2));
    LOG_STD("[DEBUG] insert to graph %d\n",_G->add_node(_G, node3));
    LOG_STD("[DEBUG] insert to graph %d\n",_G->add_node(_G, node4));
    LOG_STD("[DEBUG] graph with node %ld\n", _G->node_num);
    _G->rm_node(_G, node1->node_name, node1->free);
    LOG_STD("[DEBUG] after delete graph with node %ld\n", _G->node_num);
    LOG_STD("[DEBUG] free nodes graph\n");

    char buf[1024];;
    int limit = 100000;
    struct graph_node_t * nodes[limit];
    int i = 0;
    for(; i < limit; i++){
        sprintf(buf, "node_%d", i);
        nodes[i] = create_node(buf, NULL, 0);
        memset(buf, 0x0, 1024);
        _G->add_node(_G, nodes[i]);
    }
    LOG_STD("[DEBUG] after cycle graph with node %ld\n", _G->node_num);
    _G->free(_G);
    return 0;
}
