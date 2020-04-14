include ../common.make

%.o: ./%.c 
	@ mkdir -p ../obj/editor
	$(CC) -c -o ../obj/editor/$@ $< $(CFLAGS)

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

all: $(OBJS)
