CC = gcc
CFLAGS = -Wextra -Wall
HEADERS = word_sort.h

test:  word_sort.c $(HEADERS)
	$(CC) $(CFLAGS) -DRUN_TEST=1 word_sort.c -o test_word_sort
