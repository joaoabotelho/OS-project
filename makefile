run: exec
	./exec
exec: main.c checkers.c init.c
	gcc -Wall main.c checkers.c init.c -o exec
