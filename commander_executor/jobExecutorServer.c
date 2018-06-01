////////////////////////////////////////////////////////////////////////////////
///////////////// J O B  E X E C U T O R  S E R V E R //////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <signal.h>
#include "header.h"



#define SERVER "existance.txt"
#define PERMS   0666
#define FIFO   "fifo.1"
#define FIFO2   "fifo.2"

extern int errno;

int main(void)
{
int fd,i;
char bf[10];
char buffer[50];
int readfd;

gl_exit_flag=0;
struct stat statbuff;
/*struct sigaction act;*/

/*act.sa_handler=handler;*/
/*if(sigaction(SIGUSR1,&act,NULL))/*real time signal*/
/*	printf("<--SERVER-->ERROR signal\n");*/
signal(SIGTERM,handler);
signal(SIGUSR1,exit_handler);
signal(SIGCHLD,child_handler);
//0sigfillset(&(act.sa_mask));
//signal(SIGUSR1,handler);

for(i=0;i<50;i++)
	buffer[i]='\0';
for(i=0;i<10;i++)
	bf[i]='\0';
////////////////////////////////////////////////////////////////////////////////
////////////////////// F I F O  1  C R E A T I O N /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//printf("<--SERVER-->about to creat a pipe\n");
if ( (mkfifo(FIFO, PERMS) < 0) && (errno != EEXIST) ) 
{
	perror("<--SERVER-->can't create fifo.1");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////// F I F O  2  C R E A T I O N /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
if ( (mkfifo(FIFO2, PERMS) < 0) && (errno != EEXIST) ) 
{
	perror("<--SERVER-->can't create fifo.2");
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// L I S T  C R E A T I O N /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
list_run=List_Creation();
list_queue=List_Creation();
////////////////////////////////////////////////////////////////////////////////
//////////////////////// F I L E  C R E A T I O N  /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

if(stat(SERVER,&statbuff)!=0)/*the file doesn't exist*/
{	
	if((fd=open(SERVER,O_CREAT,PERMS))==-1)/*create it*/		
	{ 
		perror("<--SERVER-->pidfile create ");
		exit(1);
	}
	if ( (close(fd))==-1 )
		perror("<--SERVER-->pidfile close");


	fd=open(SERVER,O_WRONLY,PERMS);
/*convert process id number to string*/
//	printf("<--SERVER-->my pid is %d \n",getpid());

	sprintf(bf,"%d",getpid());
/*write it to fd (existance.txt )*/	
	if(write(fd,bf,10)==-1)
		perror("<--SERVER-->pidfile write");

	if ( (close(fd))==-1 )
		perror("<--SERVER-->pidfile close");
//	printf("<--SERVER-->pid file closed\n");
}
else
{
	printf("<--SERVER-->file exists?\n");
}

gl_random_id=0;

gl_temp_reminder=1;

gl_concurrency=1;

gl_temp_conc=gl_concurrency;


while(1)
{	
	if(gl_exit_flag==1)
	{
		if(List_Keni( list_run ) && List_Keni( list_queue ))/*wait for processes*/
			break;
	}
	if( !List_Keni(list_queue) )
		if( gl_temp_conc>0 )
		{	//write(1,"execute_head\n",14);
			List_Execute_Head( & list_queue );
		}	
}

printf("<--SERVER-->server off\n");
///////////////////// L I S T  D E L E T E /////////////////////////////////////
List_Delete(&list_run);
List_Delete(&list_queue);
//////////////////// R E M O V E  S E R V E R  F I L E /////////////////////////
if(remove(SERVER)==-1)
	perror("existance.txt");
else
	printf("<--SERVER--> %s removed \n",SERVER);
//////////////////// R E M O V E  F I F O  1  F I L E /////////////////////////////
if ( unlink(FIFO) < 0) 
{
	perror("client: can't unlink \n");
}
else
	printf("<--SERVER--> %s removed \n",FIFO);
//////////////////// R E M O V E  F I F O  2  F I L E /////////////////////////////
if ( unlink(FIFO2) < 0) 
{
	perror("client: can't unlink \n");
}
else
	printf("<--SERVER--> %s removed \n",FIFO2);

return;
}

