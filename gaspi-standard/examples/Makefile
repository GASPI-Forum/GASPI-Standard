
CC = gcc

CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-unused-parameter
CFLAGS += -pedantic
CFLAGS += -I../include/
CFLAGS += -I.

.PHONY: default

OBJ += ../include/GASPI_DEFAULT.o
OBJ += success_or_die.o
OBJ += wait_if_queue_full.o
OBJ += hello_world.o
OBJ += signal_handler/signal.o
OBJ += all_to_all/alltoall_write.o
OBJ += ../tex/waitsome_select.o
OBJ += ../tex/notify_wait.o

default: $(OBJ)

.PHONY: objclean

RM = rm -rf

objclean:
	-$(RM) $(OBJ)
