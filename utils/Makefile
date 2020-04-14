include ../common.make

ODIR=../obj/utils

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/Network_utils.o $(ODIR)/Vector.o 
