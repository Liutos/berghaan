CC = gcc
CFLAGS = -Wall -Wextra -g -march=native -pedantic -std=gnu99

main: main.o

.PHONY: clean

clean:
	if [ -f main ]; then rm -vf main; fi
	find . -name '*.o' -exec rm -vf {} \;
