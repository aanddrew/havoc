all:
	gcc client/*.c client/*/*.c utils/*.c -lSDL2 -lSDL2_image -o havoc -g -lm
