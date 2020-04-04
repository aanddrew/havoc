all:
	gcc client/*.c client/*/*.c utils/*.c -lSDL2 -lSDL_net -o havoc -g -lm
