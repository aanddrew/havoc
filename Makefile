all:
	gcc client/*.c client/*/*.c server/*.c utils/*.c -lSDL2 -o havoc -g -lm
