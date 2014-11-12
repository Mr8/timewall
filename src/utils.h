#ifndef _UTILS_H_
#define _UTILS_H_

#define MAX_FILE_LINE_LENGTH  1024

int strsplit(char *str, const char *delim, char **result);
/* split a string which argument give
 * @arg:
 *      str          [+] the string to be split
 *      delim        [+] the delim of split string
 *      result       [-] result of strings been splited
 * @return:
 *      the number of result which indicated how many part
 *      the string be splited
 * */

int read_file_lines(char *path, char lines[][MAX_FILE_LINE_LENGTH], int *count);
/* read lines from file
 * @arg:
 *      path         [+] the file path
 *      lines        [-] the file lines string
 *      count        [-] lines count
 *
 * @return:
 *      0            [+] success
 *      !0           [-] failed
 * */

#endif

