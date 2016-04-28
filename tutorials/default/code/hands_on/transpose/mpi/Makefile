#MPI_DIR=/apps/all/impi/5.0.3.048-iccifort-2015.3.187
#CC = $(MPI_DIR)/bin64/mpiicc

#MPI_DIR=/export/opt/intel2015/impi/5.0.1.035/mic
#CC = $(MPI_DIR)/bin64/mpiicc

#MPI_DIR=$(HOME)/mvapich2-2.1a_enable-threads-multiple
#CC = $(MPI_DIR)/bin/mpicc

MPI_DIR=/apps/all/MVAPICH2/2.1-iccifort-2015.3.187-GNU-5.1.0-2.25
CC=$(MPI_DIR)/bin/mpicc

NAME += Pipelined_transpose_MT

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
#CFLAGS += -O3 -debug -mmic
CFLAGS += -O3 -debug
#CFLAGS += -openmp 
CFLAGS += -openmp
CFLAGS += -std=c99
CFLAGS += -DUSE_ALIGNMENT
#CFLAGS += -DDEBUG

###############################################################################

INCLUDE_DIR += $(MPI_DIR)/include 
INCLUDE_DIR += ..

LIBRARY_DIR += $(MPI_DIR)/lib
#LIBRARY_DIR += $(MPI_DIR)/lib64
#LIBRARY_DIR += $(MPI_DIR)/lib64/mic

LDFLAGS += $(addprefix -L,$(LIBRARY_DIR))

ifeq "$(CC)" "mpiicc"
  LDFLAGS += -openmp-link static
endif

CFLAGS += $(addprefix -I,$(INCLUDE_DIR))


OBJ += ../data
OBJ += ../data_global
OBJ += ../threads
OBJ += ../now

LIB += m
#LIB += mpi_mt
LIB += mpich

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
