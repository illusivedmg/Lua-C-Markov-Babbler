all: babbler

run: babbler
	./babbler countofmontecristo.txt 200 4

babbler: babbler.c
	gcc -g -std=c99 -Wall -Werror -lm babbler.c -o babbler -llua5.2

clean:
	rm -rf babbler
	rm -rf *.o