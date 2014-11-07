#ifndef _H_THREAD_POOL_
#define _H_THREAD_POOL_

#include "all.h"

typedef void (* task_handler)(void *task);

typedef struct _thread_pool_t{
    struct list_head  task_queue;
    pthread_mutex_t   queue_lock;
    pthread_cond_t    task_cond ;
    pthread_t        *pthreads  ;
    uint8_t           isalive   ;
    uint32_t          workernum ;
    uint8_t           ready     ;
    task_handler      fp_task_handler;
}thread_pool_t;


struct task_t{
    struct list_head  task_node;
    void             *task_data;
};

thread_pool_t * thread_pool_ini(uint32_t workernum,
                         task_handler fp_handler);
/* initilize an thread pool
 * @workernum: worker number
 * @fp_handler: task_handler type function point
 */

uint32_t thread_pool_add_task(struct task_t *task);
/*
 * add an task to thread pool,
 * fp_handler will deal this task
 */

uint32_t thread_pool_destory(thread_pool_t * pp);
/*
 * destory an thread pool
 */

#endif

