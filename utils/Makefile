CC=gcc
LFLAGS=-lm
CFLAGS=-Wall -Werror -Wextra

ODIR=../obj/utils

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/Network_utils.o $(ODIR)/Vector.o 
