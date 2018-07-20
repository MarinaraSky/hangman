CC=gcc
CFLAGS = $(CF) -Wall -Wextra -Wpedantic -Wstack-usage=512 -Wwrite-strings -Wfloat-equal -Waggregate-return -Winline

hangman: hangman.c
