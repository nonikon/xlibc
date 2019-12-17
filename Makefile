MAKE_VERSION = debug

CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -Wall

ifeq ($(MAKE_VERSION), debug)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

TARGET = \
	xlist_test xarray_test \
	xrbtree_test xrbmap_test \
	xstring_test xhash_test

all : $(TARGET)

xlist_test : xlist.o xlist_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xarray_test : xarray.o xarray_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xrbtree_test : xrbtree.o xrbtree_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xrbmap_test : xrbtree.o xrbmap_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xstring_test : xstring.o xstring_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xhash_test : xhash.o xhash_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	@echo "\tCC $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

.PHONY : clean

clean :
	@echo "\tRM *.o $(TARGET)"
	@$(RM) *.o $(TARGET)
