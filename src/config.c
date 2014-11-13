#include "all.h"


static int __insert(struct rb_root *root, struct config_value *data){

    struct rb_node **tmp = &(root->rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp) {
        struct config_value *this = CONTAINER_OF(
            *tmp, struct config_value, rb_node
        );
        parent = *tmp;
        int c = strcmp(data->key, this->key);
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


static struct config_value *__search(struct rb_root *root, char *key)
{
    struct rb_node *node = root->rb_node;

    while (node) {
        struct config_value *this= CONTAINER_OF(
            node, struct config_value, rb_node
        );
        int c = strcmp(key, this->key);
        if (c < 0)
            node = node->rb_left;
        else if (c > 0)
            node = node->rb_right;
        else
            return this;
    }
    return NULL;
}


char * config_get(config * c, const char *key){
    struct config_value * cv = __search(&c->rb_root, key);
    if(cv == NULL){
        return NULL;
    }
    return cv->value;
}


config * init_config(const char *path){
    char lines[MAX_CONFIG_LINE][MAX_CONFIG_LINE_LEN];
    char *line_items[MAX_CONFIG_LINE_LEN];
    char *key_value[MAX_CONFIG_LINE_LEN];

    int count = 0;
    int status = read_file_lines(path, lines, &count);
    if (status != 0) {
        return NULL;
    }

    config * C = (config *)malloc(sizeof(config));
    if(C == NULL){
        assert(0);
        return NULL;
    }

    C->rb_root = RB_ROOT;
    int i;
    for (i = 0; i < count; i++) {
        strsplit(lines[i], " ", line_items);
        if(strsplit(line_items[0], ":", key_value) != 2){
            continue;
        }
        struct config_value * _cv = \
            (struct config_value *)calloc(1, sizeof(struct config_value));

        strncpy(_cv->key,   key_value[0], MAX_CONFIG_LINE_LEN);
        strncpy(_cv->value, key_value[1], MAX_CONFIG_LINE_LEN);

        __insert(&C->rb_root, _cv);
    }

    C->get = &config_get;
    return C;
}
