CC=gcc
LFLAGS=
CFLAGS=

ODIR=../obj/SDL_FontCache

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/SDL_FontCache.o
