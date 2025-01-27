CC = gcc
CFLAGS = -Wextra -Wall -Werror -std=c11
LDFLAGS = -lm
BUILD_DIR = ./
SRC_FILES = grep.c
OBJ_FILES = grep.o

all: s21_grep

build: s21_grep

s21_grep: grep.o
	$(CC) $(OBJ_FILES) $(CFLAGS) -o $(BUILD_DIR)s21_grep -lm

grep.o: grep.c
	$(CC) -c grep.c -o grep.o

clean:
	rm -rf *.o ./s21_grep

rebuild: clean build
