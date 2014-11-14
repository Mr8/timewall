CC := gcc
CFLAGS := -O2 -Wall
AR := ar rcu
RANLIB := ranlib
LIBDIR=../lib
ALLLIB=../lib/base.a

OBJECTS := graph.o threadpool.o rbtree.o log.o utils.o config.o epoll.o buffer.o event.o

default: build

build:$(OBJECTS)
	test -d $(LIBDIR) || mkdir $(LIBDIR); \
 	$(AR) $(ALLLIB) $(OBJECTS)

clean:
	rm -rf $(OBJECTS) $(LIBDIR) $(ALLLIB)
