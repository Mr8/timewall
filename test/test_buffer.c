#include "../src/all.h"

struct reactor *REACTOR;


/*static int _readCb(char *buf, int n, void *args){*/
    /*LOG_DEBUG("Here read callback running");*/
    /*LOG_DEBUG("Buffer exists %d, readSplit = %d", n, ((struct buffer *)args)->readSplitN);*/

    /*int nNeed = ((struct buffer *)args)->readSplitN;*/
    /*bufferWrite((struct buffer *)args, buf, nNeed);*/
    /*bufferReadContinue((struct buffer *)args, 5);*/
    /*LOG_DEBUG("Buffer exists %d, readSplit = %d", n, ((struct buffer *)args)->readSplitN);*/
    /*return nNeed;*/
/*}*/

static int _readCb(char *buf, int n, void *args){
    LOG_DEBUG("Here read callback running");
    LOG_DEBUG("Buffer exists %d, readSplit = %s", n, ((struct buffer *)args)->splitC);

    bufferWrite((struct buffer *)args, buf, n);
    LOG_DEBUG("After write to buffer exists %d", n - n);
    return n;
}


 static int accept_callback(struct event * _ev, int mode, void *arg){
     struct sockaddr_in lsa;
     int addrlen = sizeof(struct sockaddr);
     char _split[] = "__SPLIT__";
     int fd = accept(_ev->fd, (struct sockaddr *)&lsa, &addrlen);
     LOG_DEBUG("New Connection with fd:%d", fd);

     struct buffer * _B = createBuffer(REACTOR, fd);
     bufferReadSplit(_B, _split, strlen(_split), (readCb *)&_readCb, _B);

     return fd;
 }

/*static int accept_callback(struct event * _ev, int mode, void *arg){*/
    /*struct sockaddr_in lsa;*/
    /*int addrlen = sizeof(struct sockaddr);*/
    /*int nNeed  = 3;*/
    /*int fd = accept(_ev->fd, (struct sockaddr *)&lsa, &addrlen);*/
    /*printf("New Connection %d\n", fd);*/

    /*struct buffer * _B = createBuffer(REACTOR, fd);*/
    /*//bufferReadSplit(_B, _split, strlen(_split), (readCb *)&_readCb, _B);*/
    /*bufferReadUntil(_B, nNeed, (readCb *)&_readCb, _B);*/
    /*return fd;*/
/*}*/




int main()
{

    int fd = 0;
    struct sockaddr_in sa;

    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_port = htons(1115);
    sa.sin_family = AF_INET;


    REACTOR = (struct reactor *)reactorInit(9999);
    if( REACTOR == NULL ){
        perror("");
    }


    int addrlen = sizeof(struct sockaddr);

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    setreuseaddr(fd);
    setnonblock(fd);
    bind(fd, (struct sockaddr *)&sa, addrlen);


    struct event ACCEPTEV ;
    eventSelfInit(&ACCEPTEV, fd, EV_READ, (evCallback *)&accept_callback, NULL, NULL);
    eventAddListen(REACTOR, &ACCEPTEV);

    listen(fd, 10);

    REACTOR->run(REACTOR);
    LOG_DEBUG("reactor listen on port 1115");

    close(fd);

    return 0;
}
