CC=gcc
LFLAGS=-lm
CFLAGS=-Wall -Werror -Wextra

ODIR=../obj/gui

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(LFLAGS) $(CFLAGS)


all: $(ODIR)/Button.o $(ODIR)/Camera.o $(ODIR)/Dolly.o $(ODIR)/Fonts.o $(ODIR)/Menu.o $(ODIR)/TextBox.o $(ODIR)/Window.o
