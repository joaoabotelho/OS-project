run: exec
	./exec
exec: main.c checkers.c init.c queue_actions.c
	gcc -g main.c checkers.c init.c queue_actions.c -o exec
