all:
	gcc client/*.c client/*/*.c utils/*.c graphics/*.c SDL_FontCache/*.c -lSDL2 -lSDL_net -lSDL2_ttf -o havoc -g -lm
