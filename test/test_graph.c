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

    printf("[DEBUG] create link with between two node\n");

    build_link(node1, node2, 1);
    build_link(node1, node3, 2);
    build_link(node1, node4, 3);

    assert(node1->link_num == 3);

    printf("[DEBUG] node %s with link %ld\n", node1->node_name, node1->link_num);
    rb_link_tree_iterate(node1, __callback);

    printf("[DEBUG] destory link between %s <--> %s\n",
        node1->node_name, node2->node_name
    );

    destory_link(node1, node2);

    assert(node1->link_num == 2);

    rb_link_tree_iterate(node1, __callback);
    printf("[DEBUG] node %s with link %ld\n", node1->node_name, node1->link_num);
    printf("[DEBUG] destory link between %s <--> %s\n",
        node1->node_name, node3->node_name
    );
    destory_link(node1, node3);

    assert(node1->link_num == 1);

    rb_link_tree_iterate(node1, __callback);
    printf("[DEBUG] node %s with link %ld\n", node1->node_name, node1->link_num);

    node1->free(node1);
    node2->free(node2);
    node3->free(node3);
    node4->free(node4);
    return 0;
}
