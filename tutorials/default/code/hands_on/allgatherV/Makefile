ifndef GPI2_HOME
#GPI2_HOME=$(HOME)/GPI2_intel64
GPI2_HOME=$(HOME)/GPI2_intel64_impi-5.1.2
endif

CC = mpiicc

BIN += allgatherV_MPI.exe
BIN += allgatherV_roundrobin.exe
BIN += allgatherV_roundrobin_N.exe

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
CFLAGS += -O3 
CFLAGS += -std=c99

###############################################################################

INCLUDE_DIR += $(GPI2_HOME)/include
INCLUDE_DIR += ..
LIBRARY_DIR += $(GPI2_HOME)/lib64

ifeq "$(CC)" "icc"
  LDFLAGS += -qopenmp-link=static
endif
LDFLAGS += $(addprefix -L,$(LIBRARY_DIR))
CFLAGS += $(addprefix -I,$(INCLUDE_DIR))

LIB += ibverbs
#LIB += GPI2-dbg
LIB += GPI2
LIB += m
LIB += pthread

OBJ += queue
OBJ += testsome
OBJ += now

###############################################################################

default: $(BIN)

%.exe: %.o $(addsuffix .o, $(OBJ))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(addprefix -l, $(LIB))

###############################################################################

.PHONY: clean objclean

objclean:
	rm -f *.o

clean: objclean
	rm -f $(BIN)
