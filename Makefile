MAKE_VERSION = debug

CC = gcc
CFLAGS = -Wall
LDFLAGS = -Wall

ifeq ($(MAKE_VERSION), debug)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

TARGET = test_xlist test_xarray test_xrb_tree

all : $(TARGET)

test_xlist : xlist.o test_xlist.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xarray : xarray.o test_xarray.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
test_xrb_tree : xrb_tree.o test_xrb_tree.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	@echo "\tCC $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

.PHONY : clean

clean :
	@echo "\tRM *.o $(TARGET)"
	@$(RM) *.o $(TARGET)
