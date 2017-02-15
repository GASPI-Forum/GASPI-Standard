ifndef GPI2_HOME
  GPI2_HOME=$(HOME)/GPI2_intel64_impi-5.1.2
endif

MPI_HOME=/apps/all/impi/5.1.2.150-iccifort-2016.1.150
CC=$(MPI_HOME)/bin64/mpiicc


NAME += Pipelined_transpose_WN

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
#CFLAGS += -O3 -debug -mmic
CFLAGS += -O3
CFLAGS += -qopenmp
CFLAGS += -std=c99
#CFLAGS += -DDEBUG
CFLAGS += -DUSE_ALIGNMENT

###############################################################################

INCLUDE_DIR += $(MPI_HOME)/include
INCLUDE_DIR += $(GPI2_HOME)/include 
INCLUDE_DIR += ..

LIBRARY_DIR += $(MPI_HOME)/lib64
LIBRARY_DIR += $(GPI2_HOME)/lib64


LDFLAGS += $(addprefix -L,$(LIBRARY_DIR))

ifeq "$(CC)" "mpiicc"
  LDFLAGS += -qopenmp-link=static
endif

CFLAGS += $(addprefix -I,$(INCLUDE_DIR))

LIB += ibverbs
LIB += GPI2
#LIB += GPI2-dbg
LIB += pthread
LIB += m
LIB += mpi_mt
#LIB += mpi
#LIB += mpich

OBJ += ../data_global
OBJ += ../data
OBJ += ../now
OBJ += ../threads
OBJ += queue



###############################################################################

BIN = $(addsuffix .exe, $(NAME))

default: $(BIN)

%.exe: %.o $(addsuffix .o, $(OBJ))
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(addprefix -l, $(LIB))

###############################################################################

%.run: %.exe
	@$(CMD_RUN) $^ | grep ^#

.PHONY: run

run: $(addprefix $(CURDIR)/, $(addsuffix .run, $(NAME)))

###############################################################################

.PHONY: clean objclean

objclean:
	rm -f ../*.o *.o

clean: objclean
	rm -f $(BIN)
