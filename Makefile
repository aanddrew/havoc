all:
	gcc client/*.c client/*/*.c utils/*.c gui/*.c gui/renderers/*.c game/*.c SDL_FontCache/*.c -lSDL2 -lSDL_net -lSDL2_ttf -lSDL2_image -o havoc -g -lm
