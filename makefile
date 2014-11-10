TARGETS=src
TESTDIR=test
CC := gcc
CFLAGS := -std=c99  -D_GNU_SOURCE -DNDEBUG -O2 -Wall
AR := ar rcu
BINDIR=bin
.PHONY: default all build install clean

default: build

all: clean build

build:
	@for d in $(TARGETS); do \
		$(MAKE) -C $$d build; \
	done

install:
	@for d in $(TARGETS); do \
		$(MAKE) -C $$d install; \
	done

clean:
	@for d in $(TARGETS); do \
		$(MAKE) -C $$d clean; \
	done
	rm -rf bin/

test: build
	@for d in $(TESTDIR); do \
		$(MAKE) -C $$d test; \
	done; \
