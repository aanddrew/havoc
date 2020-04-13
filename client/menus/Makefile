CC=gcc
LFLAGS=-lm
CFLAGS=-Wall -Werror -Wextra

ODIR=../../obj/client/menus

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/MainMenu.o  $(ODIR)/ConnectMenu.o  

