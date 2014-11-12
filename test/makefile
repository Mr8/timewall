 CFLAGS:=-g -Wall -D_DEBUG_
 CFLAGS:=-g -Wall
 INCLUDE:=-I ../src
 MYLIB=../lib/base.a
 BINDIR=../bin

default: test

test:
	test -d $(BINDIR) || mkdir -p $(BINDIR); \
	cp -rf ./test.file /tmp ; \
	$(CC) $(CFLAGS) $(INCLUDE) test_graph.c $(MYLIB) -o $(BINDIR)/test_graph
	$(CC) $(CFLAGS) $(INCLUDE) test_threadpool.c $(MYLIB) -o $(BINDIR)/test_thread_pool -lpthread
	$(CC) $(CFLAGS) $(INCLUDE) test_log.c $(MYLIB) -o $(BINDIR)/test_log
	$(CC) $(CFLAGS) $(INCLUDE) test_utils.c $(MYLIB) -o $(BINDIR)/test_utils
	$(CC) $(CFLAGS) $(INCLUDE) test_config.c $(MYLIB) -o $(BINDIR)/test_config

clean:
	-$(RM) $(TARGET) $(MYLIB)
