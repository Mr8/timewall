#include "../src/all.h"

int main(){
    char lines[100][1024];
    int count = 0;
    read_file_lines((char *)"/tmp/test.file", lines, &count);

    char *result[1024] = {0};
    int i = 0;
    for(; i < count; i++){
        printf("[DEBUG] Line ->%s\n", lines[i]);
        int n = strsplit(lines[i], ":", result);
        printf("[DEBUG] split %d part and result is:\n", n);
        int j = 0;
        for(; j < n; j++){
            printf("%s ", result[j]);
        }
        printf("\n");
    }
    return 0;
}
