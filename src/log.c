/*
    * file: log.c
    * author: zhangbo
    * time: 2014.11.11 (single day)
    * Copyright (c) 2014, zhangbo <zhangbolinux@sina.com>.

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

const char *LogLevelStr[] = {
    "Debug", "Info", "Error", "Fatal",
};

log_handler * init_log(const char *fpath, char level, const char type){
    if (fpath == NULL)
        return NULL;

    if (type != TEXT && type != BINARY) {
        assert(0);
        return NULL;
    }

    log_handler * _lh = (log_handler *)malloc(sizeof(log_handler));
    if(_lh == NULL)
        return NULL;

    int open_file = 0;

    _lh->_type  = type;
    _lh->_fpath = fpath;
    _lh->_level = level;

    if (type == TEXT) {
        _lh->_filehandle._fp = fopen(fpath, "a");
        if (_lh->_filehandle._fp != NULL) {
            open_file = 1;
        }else if (type == BINARY) {
            _lh->_filehandle._fd = open(
                fpath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
            );
            if (_lh->_filehandle._fd != -1) {
                open_file = 1;
            }
        }
    }
    if (!open_file) {
        free(_lh);
        _lh = NULL;
        assert(0);
    }

    return _lh;
}

int write_fmt_log(log_handler *log, char level, const char *fmt, ...) {
    if (log == NULL || fmt == NULL)
        return -1;
    if (log->_level > level)
        return -1;
    if (log->_type != TEXT)
        return -1;

    va_list args;
    int size;

    struct tm ts;
    time_t t = time(NULL);
    localtime_r(&t, &ts);

    fprintf(log->_filehandle._fp,
            "%04d/%02d/%02d %02d:%02d:%02d [%s] ",
            ts.tm_year + 1900,
            ts.tm_mon + 1,
            ts.tm_mday,
            ts.tm_hour,
            ts.tm_min,
            ts.tm_sec,
            LogLevelStr[(int)level]
    );

    va_start(args, fmt);
    size = vfprintf(log->_filehandle._fp, fmt, args);
    va_end(args);

    fprintf(log->_filehandle._fp, "\n");

    return size;
}

int set_log_buffer(log_handler *log, unsigned char *buf, unsigned int size) {
    if (log == NULL || buf == NULL)
        return -1;

    if (log->_type == TEXT) {
        setbuffer(log->_filehandle._fp, (char *)buf, size);
    }
    return 0;
}

int flush_log(log_handler *log) {
    if (log == NULL)
        return -1;

    int ret = -1;
    if (log->_type == TEXT) {
            ret = fflush(log->_filehandle._fp);
        } else if (log->_type == BINARY) {
                ret = 0;
            }
    return ret;
}

int fsync_log(log_handler *log) {
    if (log == NULL)
        return -1;

    int fd = -1;
    if (log->_type == TEXT) {
        fd = fileno(log->_filehandle._fp);
    } else if (log->_type == BINARY) {
        fd = log->_filehandle._fd;
    }

    return fsync(fd);
}

long long file_size(log_handler *log) {
    if (log == NULL)
        return -1;

    int fd = -1;
    if (log->_type == TEXT) {
        fd = fileno(log->_filehandle._fp);
    } else if (log->_type == BINARY) {
        fd = log->_filehandle._fd;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        return -2;
    }
    return st.st_size;
}

int write_log(log_handler *log, const unsigned char *data, unsigned int size) {
    if (log == NULL || data == NULL)
        return -1;

    int ret = -1;
    if (log->_type == TEXT) {
        if (size == 0) return 0;

        ret = fwrite(data, size, 1, log->_filehandle._fp);
        if (ret != 1) {
            return -1;
        }
        ret = size;
    } else if (log->_type == BINARY) {
        ret = write(log->_filehandle._fd, data, size);
    }

    return ret;
}


int fseek_log(log_handler *log, unsigned long long offset) {
    if (log == NULL)
        return -1;

    int fd = -1;
    if (log->_type == TEXT) {
        fd = fileno(log->_filehandle._fp);
    } else if (log->_type == BINARY) {
        fd = log->_filehandle._fd;
    }

    if (lseek(fd, offset, SEEK_SET) == -1) {
        return -2;
    }
    return 0;
}

int uinit_log(log_handler *log) {
    if (log == NULL)
        return -1;

    if (log->_type == TEXT) {
            fclose(log->_filehandle._fp);
        } else if (log->_type == BINARY) {
                close(log->_filehandle._fd);
            }
    free(log);
    return 0;
}
