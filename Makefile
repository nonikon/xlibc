MAKE_VERSION = release

CC = gcc
CXX = g++
CFLAGS = -Wall -std=c99
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -Wall

ifeq ($(MAKE_VERSION), debug)
CFLAGS += -g -DDEBUG
CXXFLAGS += -g -DDEBUG
else
CFLAGS += -O2
CXXFLAGS += -O2
endif

TARGET = stl_test \
	xlist_test xarray_test xrbtree_test \
	xstring_test xhash_test xvector_test

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
xstring_test : xstring.o xstring_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xhash_test : xhash.o xhash_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xvector_test : xvector.o xvector_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
stl_test : stl_test.cpp
	@echo "\tLD $@"
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

%.o : %.c
	@echo "\tCC $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

.PHONY : clean

clean :
	@echo "\tRM *.o $(TARGET)"
	@$(RM) *.o $(TARGET)
