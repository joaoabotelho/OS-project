run: exec
	./exec
exec: main.c checkers.c init.c queue_actions.c doctors.c
	gcc -g main.c checkers.c init.c queue_actions.c doctors.c -o exec
