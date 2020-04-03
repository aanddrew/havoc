all:
	gcc client/*.c client/*/*.c utils/*.c -lSDL2 -o havoc -g -lm
