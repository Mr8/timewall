/*
    * file: log.h
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

#ifndef _LOG_H_
#define _LOG_H_

#include "all.h"

typedef enum {
    log_debug,
    log_info,
    log_error,
    log_fatal,
} log_level;

typedef union {
    FILE       *_fp;
    int         _fd;
} file_handler;

#define TEXT    't'
#define BINARY  'b'

typedef struct {
    const char   *_fpath;
    char         _type;      // 't' text or 'b' binary
    file_handler _filehandle;
    char         _level;
} log_handler;

extern const char *log_level_str[];

// fail return null
log_handler * init_log(const char *fpath, char level, const char type);

// fail return -1, success return write bytes
int write_log(log_handler *log, const unsigned char *data, unsigned int size);

// fail return -1, success return wrote param bytes, only for TEXT log
int write_fmt_log(log_handler *log, char level, const char *fmt, ...);

// success return 0
int set_log_buffer(log_handler *log, unsigned char *buf, unsigned int size);

// success return 0
int flush_log(log_handler *log);
int fsync_log(log_handler *log);

long long file_size(log_handler *log);

int fseek_log(log_handler *log, unsigned long long offset);

int uinit_log(log_handler *log);

#define BINLOGHANDLER(fpath)            init_log((fpath), log_debug, 'b')
#define TXTLOGHANDLER(fpath)            init_log((fpath), log_debug, 't')
#define SETLEVEL(log, level)            (log)->_level = (level)
#define DELETELOG(log)                  UninitLog((log))

#define LOG_DEBUG(log, fmt...)          write_fmt_log((log), log_debug, fmt)
#define LOG_INFO( log, fmt...)          write_fmt_log((log), log_info,  fmt)
#define LOG_ERROR(log, fmt...)          write_fmt_log((log), log_error, fmt)
#define LOG_FATAL(log, fmt...)          write_fmt_log((log), log_fatal, fmt)

#endif
