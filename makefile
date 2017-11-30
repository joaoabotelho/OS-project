run: exec
	./exec
exec: main.c checkers.c init.c queue_actions.c doctors.c stats.c named_pipe.c triage.c
	gcc -g main.c triage.c checkers.c init.c queue_actions.c doctors.c stats.c named_pipe.c -lpthread -D_REENTRANT -o exec
