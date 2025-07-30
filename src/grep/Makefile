CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11

all: s21_grep

s21_grep:
	$(CC) $(FLAGS) s21_grep.c -o s21_grep -D_POSIX_C_SOURCE=201710L

clean:
	rm -rf s21_grep

rebuild: clean s21_grep

tests: 
	$(CC) $(FLAGS) s21_grep.c -o s21_grep -D_POSIX_C_SOURCE=201710L
	bash tests.sh 