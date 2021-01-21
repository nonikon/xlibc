MAKE_VERSION = release

CC = gcc
CXX = g++
CFLAGS = -Wall -std=c99
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -Wall

CP = cp

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

xconfig.h:
	@echo "\tGEN $@"
	@$(CP) xconfig.default.h xconfig.h

xlist_test : xconfig.h xlist.o xlist_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xarray_test : xconfig.h xarray.o xarray_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xrbtree_test : xconfig.h xrbtree.o xrbtree_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xstring_test : xconfig.h xstring.o xstring_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xhash_test : xconfig.h xhash.o xhash_test.o
	@echo "\tLD $@"
	@$(CC) -o $@ $^ $(LDFLAGS)
xvector_test : xconfig.h xvector.o xvector_test.o
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
	@echo "\tRM *.o xconfig.h $(TARGET)"
	@$(RM) *.o xconfig.h $(TARGET)
