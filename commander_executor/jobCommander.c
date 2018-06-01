////////////////////////////////////////////////////////////////////////////////
//////////////////////// J O B  C O M M A N D E R  /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/errno.h>

#define SERVER "existance.txt" /* file that shows server existance*/
#define PERMS   0666
#define FIFO   "fifo.1"
#define FIFO2   "fifo.2"

extern int errno;

int main(int argc,char *argv[])
{
	char argument_a[64];
	char buff[64];
	char bf[64];
	char output[256];
	int i,concurrency=0,fd,serverpid=0;
	int jobid=0;
	int writefd,readfd;
	int flag=0;/*flag==0 means don't read through fifo2*/
	pid_t pid;
 	struct stat statbuff;

	
	for(i=0;i<64;i++)
	{
		argument_a[i]='\0';
		buff[i]='\0';
		bf[i]='\0';
	}
	for(i=0;i<256;i++)
		output[i]='\0';	
////////////////////////////////////////////////////////////////////////////////
/////////////////// A R G U M E N T S  C H E C K ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	if (argc==1)
	{
		printf("missing argument\n");
		return 0;
	}
	else
	{
		if(!strcmp(argv[1],"issuejob"))
		{
			strcpy(argument_a,argv[1]);
			strcat(argument_a," ");
			for(i=2;i<argc;i++)/*save any possible parameters*/
			{
				strcat(argument_a,argv[i]);
				strcat(argument_a," ");			
			}
			flag=1;
		}
		else if(!strcmp(argv[1],"setConcurrency"))
		{
			strcpy(argument_a,argv[1]);			
			if(argv[2]!=NULL)/*if you gave a number, ->save it*/					
			{	strcat(argument_a," ");
				strcat(argument_a,argv[2]);
				if ((concurrency=atoi(argv[2]))==0)
				{
					printf("wrong value inserted\n");
					return;
				}			

			}			
			else 				/*else you forgot the number -> error!*/
			{
				printf("missing value,Concurrency is set 1 by default\n");
				concurrency=1;
			}				
		}
		else if(!strcmp(argv[1],"stop"))
		{
			strcpy(argument_a,argv[1]);			
			if(argv[2]!=NULL)		
			{	strcat(argument_a," ");
				strcat(argument_a,argv[2]);
				if ((jobid=atoi(argv[2]))==0)
				{
					printf("wrong value inserted\n");
					return;
				}			
			}		
			else
			{
				printf("missing value\n");
				return;
			}
		
		}
		else if(!strcmp(argv[1],"poll"))
		{
			strcpy(argument_a,argv[1]);			
			if(argv[2]!=NULL)
				if(!strcmp(argv[2],"running") || !strcmp(argv[2],"queued"))
				{	strcat(argument_a," ");
					strcat(argument_a,argv[2]);
				}
				else
				{
					printf("arguments should be either [running] or [queued] \n");
					return;
				}									
			else
			{
				printf("missing parameter\n");
				return;
			}
			flag=1;									
		}
		else if(!strcmp(argv[1],"exit"))
		{
			strcpy(argument_a,argv[1]);			
		}
		else
		{	
			printf("option doesn't exist \n");
			return;
		}
	}
////////////////////////////////////////////////////////////////////////////////
///////////// C H E C K  E X I S T A N C E  O F  S E R V E R  //////////////////
////////////////////////////////////////////////////////////////////////////////
		if(stat(SERVER,&statbuff)==0)/*the file exist so server exists*/
			;
		else 							 /*else create server*/
		{
			printf("COMMANDER about to create the server \n");
			if ( (pid=fork())==-1)
			{
				perror("COMMANDER fork"); 
				exit(1);
			}
			if ( pid == 0 )
			{
				execl("server","./server",NULL);
				perror("COMMANDER execl");
				exit(1);
			}
		}
		while(stat(SERVER,&statbuff)!=0)/*wait for the server to prepare the file & fifo*/		
		{;}

///////////////////////////////////////////////////////////////////////////////
//////////// S E N D  A  S I G N A L  T O  S E R V E R /////////////////////////
/////////// A B O U T  T O  W R I T E  T O  P I P E ////////////////////////////
////////////////////////////////////////////////////////////////////////////////
sleep(1);
/////////////// R E A D  S E R V E R 'S  P I D  F R O M  F I L E ///////////////
if((fd=open(SERVER,O_RDONLY,PERMS))==-1)
	perror("COMMANDER open for pid read");

if ((read(fd,buff,5))==-1)
	perror("COMMANDER read");
else
	serverpid=atoi(buff);

if ( (close(fd))==-1 )
	perror("COMMANDER close");
////////// S I G N A L  H I M  /////////////////////////////////////////////////
if(!strcmp(argument_a,"exit"))
{
	kill(serverpid,SIGUSR1);
	return;
}
kill(serverpid,SIGTERM);
///////////////////////// O P E N  P I P E  F O R  W R I T I N G ///////////////
if ( (writefd = open(FIFO, O_WRONLY ))  < 0)  
{
	perror("COMMANDER: can't open read fifo \n");
}
//////////////////////// W R I T E  T O  P I P E ///////////////////////////////
write(writefd,argument_a,strlen(argument_a));
//////////////////////// C L O S E  P I P E ////////////////////////////////////
close(writefd);

////////////////////////////////////////////////////////////////////////////////
///////////////////////// F I F O  2  R E A D  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
if (flag==1)
{
	if ( (readfd = open(FIFO2,O_RDONLY))  < 0)  
	{
		perror("<--COMMANDER-->: can't open read fifo.2");
	}
	if(!strcmp(argv[1],"issuejob"))
		usleep(10);
	else
		sleep(1);
	read(readfd,output,256);	
/////////////////////// P R I N T  O U T P U T  R E S U L T ////////////////////
	printf("%s\n",output);
//////////////////////////// C L O S E  F I F O ////////////////////////////////	
	close(readfd);
////////////////////////////////////////////////////////////////////////////////
}
return 0;
}

