CC=gcc
LFLAGS=-lm
CFLAGS=-Wall -Werror -Wextra

ODIR=../obj/renderers

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(LFLAGS) $(CFLAGS)


all: $(ODIR)/GameRenderer.o $(ODIR)/MapRenderer.o $(ODIR)/PlayerRenderer.o $(ODIR)/ProjectileRenderer.o
