TOPROOT = $(PWD)

GASPI_DIR = /home/harry/gaspi/trunk/gaspi-gpi-wrapper
BIN_DIR = $(PWD)/bin/

CC =  gcc
CPP = g++

#gcc
CFLAGS = -std=gnu99 -Wall

SOURCES = helloworld.c

LIBS    = -L$(GASPI_DIR)/lib -lgaspi_gpi
INCLUDE = -I$(GASPI_DIR)/ 

OBJS = $(SOURCES:%.c=%.o)

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBS)
	cp $@ $(BIN_DIR)

%.o:%.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIBS)
	cp $@ $(BIN_DIR)

all : $(OBJS)

clean:
	rm -rf *~ \#_* *.o
