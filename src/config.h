#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "all.h"

#define CONFIG get_config
#define MAX_CONFIG_LINE 1024
#define MAX_CONFIG_LINE_LEN 1024


typedef struct config_t {
    struct rb_root rb_root;
    int            port;
    char           *log_path;
    uint64_t       max_client;
    uint64_t       max_nodes;
    uint64_t       max_graphs;
    char * (* get)(struct config_t *c, const char *key);
}config;


struct config_value {
    struct rb_node rb_node;
    char   key[MAX_CONFIG_LINE_LEN];
    char   value[MAX_CONFIG_LINE_LEN];
    int    index;
};


config * init_config(const char *path);

#endif

