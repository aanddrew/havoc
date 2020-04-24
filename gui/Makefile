include ../common.make

ODIR=../obj/gui

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)


all: $(ODIR)/Button.o $(ODIR)/Camera.o $(ODIR)/Dolly.o $(ODIR)/Fonts.o $(ODIR)/Menu.o $(ODIR)/TextBox.o $(ODIR)/Window.o $(ODIR)/Atlas.o
