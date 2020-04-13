CC=gcc
LFLAGS=-lm
CFLAGS=-Wall -Werror -Wextra

%.o: ./%.c 
	@ mkdir -p ../obj/editor
	$(CC) -c -o ../obj/editor/$@ $< $(LFLAGS) $(CFLAGS)

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

all: $(OBJS)
