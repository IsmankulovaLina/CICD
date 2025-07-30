CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11

all: s21_cat

s21_cat:
	$(CC) $(FLAGS) s21_cat.c cat_func.c -o s21_cat

clean:
	rm -rf s21_cat

rebuild: clean s21_cat

tests:
	$(CC) $(FLAGS) s21_cat.c cat_func.c -o s21_cat
	bash test_func_cat.sh