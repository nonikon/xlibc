MAKE_VERSION = debug

CC = gcc
CFLAGS = -Wall
LDFLAGS = -Wall

ifeq ($(MAKE_VERSION), debug)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

TARGET = \
	test_xlist test_xarray \
	test_xrb_tree test_xrb_map \
	test_xstring test_xhash

all : $(TARGET)

test_xlist : xlist.o test_xlist.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xarray : xarray.o test_xarray.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xrb_tree : xrbtree.o test_xrb_tree.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xrb_map : xrbtree.o test_xrb_map.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xstring : xstring.o test_xstring.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xhash : xhash.o test_xhash.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	@echo "\tCC $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

.PHONY : clean

clean :
	@echo "\tRM *.o $(TARGET)"
	@$(RM) *.o $(TARGET)
