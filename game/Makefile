CC=gcc
LFLAGS=-lm
CFLAGS=-Wall -Werror -Wextra

ODIR=../obj/game

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/Projectile.o $(ODIR)/Player.o $(ODIR)/Map.o $(ODIR)/Game.o $(ODIR)/Vector2d.o
