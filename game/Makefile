include ../common.make

ODIR=../obj/game

$(ODIR)/%.o: ./%.c 
	@ mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(ODIR)/Projectile.o $(ODIR)/Player.o $(ODIR)/Map.o $(ODIR)/Game.o $(ODIR)/Vector2d.o $(ODIR)/HitBox.o $(ODIR)/Item.o
