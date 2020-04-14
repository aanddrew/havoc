include ../common.make

ODIR=../obj/renderers

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)


all: $(ODIR)/GameRenderer.o $(ODIR)/MapRenderer.o $(ODIR)/PlayerRenderer.o $(ODIR)/ProjectileRenderer.o
