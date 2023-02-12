all: wordle

wordle: wordle.c
	gcc -lncurses wordle.c -o wordle

.PHONY : clean
clean :
	rm -f wordle
	rm -f *.out
	rm my_diff
