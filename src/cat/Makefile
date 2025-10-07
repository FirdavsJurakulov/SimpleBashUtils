CC = gcc
CFLAGS = -Wextra -Wall -Werror -std=c11
LDFLAGS = -lm
BUILD_DIR = ./
SRC_FILES = cat.c
OBJ_FILES = cat.o

all: s21_cat

s21_cat: Makefile cat.h cat.c
	gcc $(CFLAGS) cat.c -o s21_cat

rebuild:
	make clean
	make all

clean:
	rm -f s21_cat
	