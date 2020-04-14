include ../../common.make

ODIR=../../obj/client/network

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/ByteQueue.o  $(ODIR)/Client_Pool.o  $(ODIR)/Client_Receiver.o  $(ODIR)/Client_Sender.o  $(ODIR)/Network.o

