/*
    * file: threadpoo.c
    * author: zhangbo
    * time: 2014.11.6
    * Copyright (c) 2014, zhangbo <zhangbolinux@sina.con>.

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

// global thread pool struct, one progress only have one pool
thread_pool_t g_threads_pool;


static void *thread_entry(void *args){

    // to avoid warning
    if(args == (void *)NULL){}

    for(;;){

        pthread_mutex_lock(&g_threads_pool.queue_lock);
        // with mutex lock context =>
            while(g_threads_pool.ready == 0){
                pthread_cond_wait(&g_threads_pool.task_cond,  \
                                &g_threads_pool.queue_lock);
            }

            if(queue_empty(&g_threads_pool.task_queue)){
                pthread_mutex_unlock(&g_threads_pool.queue_lock);
                continue;
            }

            struct list_head *cur = queue_get_first(
                &g_threads_pool.task_queue
            );

            queue_del_first(&g_threads_pool.task_queue);
            g_threads_pool.ready -= 1;

        // <= without mutex lock context
        pthread_mutex_unlock(&g_threads_pool.queue_lock);

        struct task_t *task = (struct task_t *)container_of(
            cur, struct task_t, task_node
        );

        g_threads_pool.fp_task_handler(task);

    }
}

thread_pool_t * thread_pool_ini(uint32_t workernum,
                               task_handler fp_handler){

    if(g_threads_pool.isalive == 1 || workernum <= 0){
        return NULL;
    }

    g_threads_pool.isalive = 1;
    g_threads_pool.workernum = workernum;
    if (pthread_mutex_init(&g_threads_pool.queue_lock, NULL) && \
        pthread_cond_init(&g_threads_pool.task_cond, NULL)) {
        return NULL;
    }

    init_queue_head(&g_threads_pool.task_queue);

    g_threads_pool.pthreads = (pthread_t *)malloc(
        sizeof(pthread_t) * workernum
    );

    if(g_threads_pool.pthreads == (pthread_t *)NULL){
        return NULL;
    }

    uint32_t i = 0;
    int err    = -1;
    for( ; i < workernum; i++ ){
        err = pthread_create(
            &g_threads_pool.pthreads[i], NULL, thread_entry, NULL
        );
        if( err == -1 ){
            free(g_threads_pool.pthreads);
            return NULL;
        }
    }

    g_threads_pool.fp_task_handler = fp_handler;
    g_threads_pool.ready = 0;

    return &g_threads_pool;
}

uint32_t thread_pool_add_task(struct task_t *task){

    pthread_mutex_lock(&g_threads_pool.queue_lock);
        queue_add_tail(&task->task_node, &g_threads_pool.task_queue);
        g_threads_pool.ready += 1;
    pthread_mutex_unlock(&g_threads_pool.queue_lock);

    pthread_cond_signal(&g_threads_pool.task_cond);

    return 0;
}

uint32_t thread_pool_destory(thread_pool_t * pp){
    if(pp->isalive != 1){
        return -1;
    }
    uint32_t i = 0;
    for(;;){
        if(pp->ready == 0){
            for(; i < pp->workernum; i++ ){
                pthread_join(pp->pthreads[i], NULL);
            }
            break;
        }
        sleep(1);
    }
    free(pp->pthreads);
    pthread_mutex_destroy(&pp->queue_lock);
    pp->isalive = 0;
    return 0;
}

