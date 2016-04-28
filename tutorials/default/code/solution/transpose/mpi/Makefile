CC=mpiicc

NAME += Pipelined_transpose_MT

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
CFLAGS += -O3 -debug
CFLAGS += -openmp
CFLAGS += -std=c99
CFLAGS += -DUSE_ALIGNMENT
#CFLAGS += -DDEBUG

###############################################################################

INCLUDE_DIR += $(MPI_DIR)/include 
INCLUDE_DIR += ..

LIBRARY_DIR += $(MPI_DIR)/lib
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
LIB += mpi_mt


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
