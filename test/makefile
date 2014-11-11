 CFLAGS:=-g -Wall -D_DEBUG_
 CFLAGS:=-g -Wall
 INCLUDE:=-I ../src
 MYFILE=../src/graph.c ../src/rbtree.c ../src/log.c
 MYLIB=../lib/base.a
 BINDIR=../bin

default: test

test:
	test -d $(BINDIR) || mkdir -p $(BINDIR); \
	$(CC) $(CFLAGS) $(INCLUDE) test_graph.c $(MYLIB) -o $(BINDIR)/test_graph
	$(CC) $(CFLAGS) $(INCLUDE) test_threadpool.c $(MYLIB) -o $(BINDIR)/test_thread_pool -lpthread
	$(CC) $(CFLAGS) $(INCLUDE) test_log.c $(MYLIB) -o $(BINDIR)/test_log

clean:
	-$(RM) $(TARGET) $(MYLIB)
