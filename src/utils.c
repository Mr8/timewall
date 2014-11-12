#include "all.h"

int strsplit(char *str, const char *delim, char **result) {
    int result_count = 0;
    char *s = strtok(str, delim);
    while (s != NULL) {
        *result++ = s;
        result_count++;
        s = strtok(NULL, delim);
    }
    return result_count;
}


int read_file_lines(char *path, char lines[][MAX_FILE_LINE_LENGTH], int *count) {
    char line[MAX_FILE_LINE_LENGTH];
    FILE *fp = NULL;
    if ((fp = fopen(path, "r")) == NULL) {
        return -1;
    }
    *count = 0;
    memset(line, 0, sizeof(line));
    while (fgets(line, MAX_FILE_LINE_LENGTH - 1, fp) != NULL) {
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
        }
        strcpy(lines[(*count)++], line);
        memset(line, 0, sizeof(line));
    }
    fclose(fp);
    return 0;
}

