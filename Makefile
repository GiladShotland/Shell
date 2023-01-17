.PHONY: all

all: myshell

myshell: shell2.c ifelse.h
	gcc -o myshell shell2.c && ./myshell