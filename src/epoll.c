#include "all.h"

#define MAX_FD 512

static int epoll_fd = 0;

static int epoll_init(void)
{
    if(!epoll_fd) {
        epoll_fd = epoll_create(MAX_FD);
        if(epoll_fd == -1) {
            LOG_INFO("Epoll init: error #%d\n", errno);
            return -1;
        }
    }
    LOG_DEBUG("Epoll init: fd #%d\n",       epoll_fd);
    LOG_DEBUG("Epoll init: EPOLLIN #%d\n",  EPOLLIN);
    LOG_DEBUG("Epoll init: EPOLLOUT #%d\n", EPOLLOUT);
    LOG_DEBUG("Epoll init: EPOLLERR #%d\n", EPOLLERR);
    LOG_DEBUG("Epoll init: EPOLLHUP #%d\n", EPOLLHUP);
    errno = 0;
    return 0;
}

static int epoll_add(struct event *ev)
{
    struct epoll_event epev ;
    epev.events = 0;

    if(ev->mode & EV_READ) {
        epev.events |= EPOLLIN;
    }
    if(ev->mode & EV_WRITE) {
        epev.events |= EPOLLOUT;
    }

    epev.data.fd = ev->fd;
    epev.data.ptr = ev;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev->fd, &epev) < 0) {
        perror("Epoll add: error");
    }
    return 0;
}

static int epoll_mod(struct event *ev)
{
    struct epoll_event epev;
    epev.events = 0;

    if(!ev->fd) {
        return -1;
    }
    if(ev->mode & EV_READ) {
        epev.events |= EPOLLIN;
    }
    if(ev->mode & EV_WRITE) {
        epev.events |= EPOLLOUT;
    }
    if(!(ev->mode & EV_READ)) {
        epev.events &= (~EPOLLIN);
    }
    if(!(ev->mode & EV_WRITE)) {
        epev.events &= (~EPOLLOUT);
    }

    epev.data.fd = ev->fd;
    epev.data.ptr = ev;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev->fd, &epev) < 0) {
        LOG_ERROR("epoll mode: %s", strerror(errno));
    }
    return 0;
}

static int epoll_del(struct event *ev)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev->fd, 0);
    return 0;
}

static int epoll_run(struct epollOpe * eo, int timeout)
{
    int i = 0;
    int err = 0;
    struct epoll_event epev[MAX_FD];
    struct event *ev;

    err = epoll_wait(epoll_fd, epev, MAX_FD, timeout > 0 ? -1 : timeout);
    if(err == -1) {
        LOG_FATAL("Epoll wait: %s", strerror(errno));
        goto out;
    }
    if(err == 0) {
        LOG_ERROR("Epoll: timeout, %s\n", strerror(errno));
        goto out;
    }
    for(i = 0; i < err; i++) {
        ev = (struct event *)epev[i].data.ptr;
        LOG_DEBUG("Epoll: ""Signal #%d on fd #%d.\n", epev[i].events, ev->fd);

        if(epev[i].events & EPOLLIN) {
            if(epev[i].events == (EPOLLIN | EPOLLERR | EPOLLHUP)) {
                LOG_DEBUG("Epoll: error #(EPOLLIN | EPOLLERR | EPOLLHUP) \n");
                ev->signal |= EV_ERR;
            }else
                ev->signal |= EV_READ;
        }
        if(epev[i].events & EPOLLOUT) {
            ev->signal |= EV_WRITE;
        }

        ev = (struct event *)ev;
        if(ev->status == INWAITQUEUE){
            eo->evRm(eo->waitQueue, ev);
        }
        ev->status = INSIGNALQUEUE ;
        eo->evAdd(eo->signalQueue, ev);
    }
out:
    return err;
}

struct eventOps epoll = {
    (char *)"GNU/Linux/EPOLL",
    &epoll_init,
    &epoll_add,
    &epoll_mod,
    &epoll_del,
    &epoll_run
};
