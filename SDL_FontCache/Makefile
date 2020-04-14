ODIR=../obj/SDL_FontCache

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $<

all: $(ODIR)/SDL_FontCache.o
