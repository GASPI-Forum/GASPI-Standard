TOPROOT = $(PWD)

GPI_DIR = /home/harry/gpilib
BIN_DIR = $(PWD)/bin/
LIB_DIR = $(PWD)/lib/
CC = gcc
CPP = g++

#gcc
CFLAGS = -O2 -g -std=gnu99 -fPIC -std=gnu99 -c -Wall

BUILDFLAGS = -shared -Wl,-soname,libgaspi_gpi.so

INCLUDE = -I$(GPI_DIR)/include -I/home/harry/zih/trunk/gaspi-standard/include
SOURCES = gaspi_gpi.c

#gcc
LIBS    = -L$(GPI_DIR)/lib64 -libverbs15 -lGPI -Wl,-rpath=$(GPI_DIR)/lib64:

OBJS = $(SOURCES:%.c=%.o)

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBS)
	cp $@ $(BIN_DIR)

%.o:%.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBS)
	cp $@ $(BIN_DIR)

libgaspi_gpi.so:gaspi_gpi.o
	$(CC) $(BUILDFLAGS) gaspi_gpi.o -o libgaspi_gpi.so $(LIBS)
	cp $@ $(LIB_DIR)
	cp gaspi_gpi.h ./include
all : $(OBJS)

clean:
	rm -rf *~ \#_* *.o
	rm -rf *~ \#_* *.so
