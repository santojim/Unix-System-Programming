all:
	clear
	gcc -o server jobExecutorServer.c serverFunctions.c
	gcc -o commander jobCommander.c


server :
	gcc -o server jobExecutorServer.c
comander :
	gcc -o commander jobCommander.c

clean :
	rm -f server commander existance.txt fifo.1 fifo.2 userlist
