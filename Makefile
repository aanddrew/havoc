include common.make
LFLAGS=-lSDL2 -lSDL_net -lSDL2_ttf -lSDL2_image -lm 
SERVER_LFLAGS=-lSDL2 -lSDL_net -lm

CLIENT_OUT=havoc
SERVER_OUT=havoc_server
EDITOR_OUT=havoc_editor

all: client server editor

client:
	+$(MAKE) -C client
	+$(MAKE) -C renderers
	+$(MAKE) -C gui
	+$(MAKE) -C game
	+$(MAKE) -C utils
	+$(MAKE) -C SDL_FontCache
	$(CC) $(CFLAGS) $(LFLAGS) obj/client/*.o obj/client/*/*.o obj/renderers/*.o obj/gui/*.o obj/game/*.o obj/utils/*.o obj/SDL_FontCache/*.o -o $(CLIENT_OUT)

server:
	+$(MAKE) -C server 
	+$(MAKE) -C game
	+$(MAKE) -C utils
	$(CC) $(CFLAGS) $(LFLAGS) obj/server/*.o obj/game/*.o obj/utils/*.o -o $(SERVER_OUT)

editor:
	+$(MAKE) -C editor
	+$(MAKE) -C renderers
	+$(MAKE) -C gui
	+$(MAKE) -C game
	+$(MAKE) -C utils
	+$(MAKE) -C SDL_FontCache
	$(CC) $(CFLAGS) $(LFLAGS) obj/editor/*.o obj/renderers/*.o obj/gui/*.o obj/game/*.o obj/utils/*.o obj/SDL_FontCache/*.o -o $(EDITOR_OUT)

style:
	clang-format -i $(shell find . -name "*.[ch]" -type f)

clean:
	rm $(CLIENT_OUT) $(SERVER_OUT) $(EDITOR_OUT) -r obj

.PHONY: server client editor
