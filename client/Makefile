include ../common.make

ODIR=../obj/client

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/main.o $(ODIR)/Controller.o $(ODIR)/GameLoop.o $(ODIR)/Hud.o
	+$(MAKE) -C network
