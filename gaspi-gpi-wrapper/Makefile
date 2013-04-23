TOPROOT = $(PWD)

#change me
GPI_DIR = /home/huenich/gpi/gpi-SDK

BIN_DIR = $(PWD)/bin/
LIB_DIR = $(PWD)/lib/
CC = gcc
CPP = g++

FLAGS = -O2 -g -fPIC -c -Wall
#gcc
CFLAGS = $(FLAGS) -std=gnu99
#g++
CPPFLAGS = $(FLAGS)

BUILDFLAGS = -shared -Wl,-soname

INCLUDE = -I$(GPI_DIR)/include -I./ 
SOURCES = $(wildcard *.cpp)

#gcc
LIBS    = -L$(GPI_DIR)/lib64 -libverbs15 -lGPI -Wl,-rpath=$(GPI_DIR)/lib64:

OBJS = $(SOURCES:%.cpp=%.o)

#%.o:%.c
#	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBS)
#	cp $@ $(BIN_DIR)

%.o:%.cpp
	echo $(SOURCES)
	$(CPP) $(CPPFLAGS) $(INCLUDE) -o $@ $< $(LIBS)
	cp $@ $(BIN_DIR)

libgaspi_gpi.so:gaspi_gpi.o
	$(CPP) $(BUILDFLAGS),libgaspi_gpi.so gaspi_gpi.o -o libgaspi_gpi.so $(LIBS)
	cp $@ $(LIB_DIR)
	cp gaspi_gpi.h ./include

libgaspi_gpi_profiler.so:gaspi_gpi_profiler.o
	$(CPP) $(BUILDFLAGS),libgaspi_gpi_profiler.so gaspi_gpi_profiler.o -o libgaspi_gpi_profiler.so $(LIBS) 
	cp $@ $(LIB_DIR)
	cp gaspi_gpi_profiler.h ./include

all : libgaspi_gpi.so libgaspi_gpi_profiler.so

clean:
	rm -rf *~ \#_* *.o
	rm -rf *~ \#_* *.so
