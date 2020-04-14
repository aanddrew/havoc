include ../common.make

ODIR=../obj/server

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/main.o $(ODIR)/Server_Receiver.o $(ODIR)/Server_Sender.o $(ODIR)/Server_Pool.o
