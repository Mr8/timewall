#include "../src/all.h"

void _task_handler(void * task){
    struct task_t * _t = (struct task_t *)task;
    int * data = (int *)_t->task_data;
    printf("[DEBUG]Get task with id:%d\n", *data);
}

int main(){
    thread_pool_t * thp = thread_pool_ini(32, &_task_handler);
    struct task_t *Tasks = (struct task_t *)malloc(sizeof(struct task_t) * 100);
    int *datas = (int *)malloc(sizeof(int) * 100);
    int i = 0;
    for(; i < 100; i++){
        datas[i] = i;
        Tasks[i].task_data = &datas[i];
        printf("[DEBUG] Add to task queue with id:%d\n", datas[i]);
        thread_pool_add_task(&Tasks[i]);
    }
    sleep(1000);
    thread_pool_destory(thp);
}
