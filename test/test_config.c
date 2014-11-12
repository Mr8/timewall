#include "../src/all.h"

int main(){
    config * C = init_config("/tmp/test.file");
    if(C == NULL){
        printf("[DEBUG] init config failed\n");
        return -1;
    }
    printf("[DEBUG] Get config file\n");
    char * log = C->get(C, "log");
    assert(strcmp(log, "log") == 0);
    printf("[DEBUG] Get segment log with value->%s\n", log);
    return 0;
}
