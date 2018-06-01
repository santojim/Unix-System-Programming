#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <signal.h>
#include <malloc.h>
#include "header.h"
#include <string.h>

#define FIFO   "fifo.1"
#define FIFO2   "fifo.2"
#define SENTINEL -1


////////////////////////////////////////////////////////////////////////////////
///////////////////// S I G C H L D  H A N D L E R /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void child_handler(int sig)
{
	pid_t pid;
	pid=wait(NULL);
	printf("child with pid : %d exited \n",pid);
	/*remove process from running list*/
//	printf("kanw remove ton %d\n",pid);
	List_Remove(&list_run,pid,1);/*1 means pid is the real one*/
}
////////////////////////////////////////////////////////////////////////////////
///////////////////// S I G U S R 1  H A N D L E R /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void exit_handler(int sig)
{
	gl_exit_flag=1;
}
////////////////////////////////////////////////////////////////////////////////
//////////////////// S I G T E R M   H A N D L E R /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void handler(int sig)
{
	int i;
	int readfd;
	int stop_jobid;
	char temp[64];
	char temp2[64];
/////////// I N I T I A L I Z E  B U F F ///////////////////////////////////////
	for(i=0;i<64;i++)
	{
		buff[i]='\0';
		temp[i]='\0';
		temp2[i]='\0';
	}
///////////// O P E N  F I F O  F O R  R E A D I N G ///////////////////////////
	if ( (readfd = open(FIFO,O_RDONLY))  < 0)  
	{
		perror("<--SERVER-->server: can't open read fifo");
	}
	read(readfd,buff,64);
//////////////////////////// C L O S E  F I F O ////////////////////////////////	
	close(readfd);
/////////////// I N S E R T  C O M M A N D  I N  L I S T ///////////////////////
	sscanf(buff,"%s",temp);
	if(!strcmp(temp,"issuejob"))
	{
//		printf("\n++++++ CONCCURRENCY IS %d\n",gl_temp_conc);		
		Executor(buff,0,1);/*zero means process doesn't have fakeid*/
	}
	else if(!strcmp(temp,"setConcurrency"))
	{
		sscanf(buff,"%s %d",temp,&gl_concurrency);
		if(gl_temp_reminder==0)/*for the first time only take the real concurrency*/
			gl_temp_reminder=gl_concurrency;
		printf("conc done ->%d\n",gl_concurrency);

		if(gl_concurrency<gl_temp_reminder)
			gl_temp_conc=gl_temp_conc-gl_concurrency;
		else if(gl_temp_conc==0)
			gl_temp_conc=gl_concurrency-gl_temp_reminder;
		else if(gl_concurrency>gl_temp_reminder)
			gl_temp_conc=gl_concurrency-gl_temp_conc;
		else
			gl_temp_conc=gl_concurrency;
/*save gl_concurrency 's value for next change*/
		gl_temp_reminder=gl_concurrency;
	}
	else if(!strcmp(temp,"stop"))
	{	
		sscanf(buff,"%s %d",temp,&stop_jobid);
		
		if ( List_Search(list_run,stop_jobid,1)==0 )/*not found in list_run*/
			if ( List_Search(list_queue,stop_jobid,2)==0 )/*not found in list_queue*/
				printf("<--SERVER--> Given pid not found !\n");
	}
	else if(!strcmp(temp,"poll"))
	{
		sscanf(buff,"%s %s",temp,temp2);
		if( !strcmp(temp2,"running") )
			List_Print(list_run);
		else
			List_Print(list_queue);
	}
	return;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////////  E X E C U T O R  ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Executor(char *buff,int fakeid,int pipe)/*3rd arg means write to pipe if 1*/
{
	pid_t pid; 
	int fd=-1;
	int counter_1st=0;
	int i,j,x;
	int counter=0;
	char **array;
	int writefd;
	char input[64];
////////////////////////////////////////////////////////////////////////////////
//////////////////// C O N V E R T   A R G U M E N T S /////////////////////////
/////////////////////// I N T O  2 D  A R R A Y ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

	for(i=0;i<64;i++)
	{
		input[i]='\0';
	}
////////////////  find the length of the first word ////////////////////////////
	for(i=0;i<strlen(buff);i++)
	{
		counter_1st++;
		if((buff[i])==' ')
			break;
	}
///////////////// find number of arguments //////////////////////////////////////
	for(i=0;i<strlen(buff);i++)
	{
		if((buff[i])==' ')
			counter++;
	}
	counter--;/*for the last " "*/
//////////////// allocate the 2d array /////////////////////////////////////////
	array=malloc(counter+1 *10* ( sizeof(char*) ));
	for(i=0;i< counter;i++)
		array[i]=malloc(20*sizeof(char));
////////////////  initiallize array ////////////////////////////////////////////
	for (i = 0; i < counter; i++)
	{
	  for (j = 0; j < 20 ; j++)
	    array[i][j] = '\0';
	}
//////////////////  copy to array from buff ////////////////////////////////////
	j=0;
	x=0;
	for(i=counter_1st;i<strlen(buff);i++)
	{
		if((buff[i])==' ' )
		{
			j++;
			x=0;
			continue;
		}
		array[j][x]=buff[i];
		x++;
	}
////////////// null termination to last element ////////////////////////////////
	array[counter]=NULL;

	if(gl_temp_conc<=0)
	{	/*++ processes random id*/
		gl_random_id++;
//		printf("job %s insert to list_queue with tempid %d\n",array[0],gl_random_id);
		List_Insert(&list_queue,buff,0,gl_random_id,array[0]);
		if(pipe==1)
		{
/////////////////////////// P I P E 2 //////////////////////////////////////////
///////////////////////// O P E N  P I P E  F O R  W R I T I N G ///////////////
			if ( (writefd = open(FIFO2, O_WRONLY ))  < 0)  
			{
				perror("SERVER: can't open read fifo \n");
			}
			sprintf(input,"< %d , %s > -- QUEUED",gl_random_id,array[0]);

//////////////////////// W R I T E  T O  P I P E ///////////////////////////////
			write(writefd,input,strlen(input));
/////////////////////// C L O S E  P I P E /////////////////////////////////////
			close(writefd);
////////////////////////////////////////////////////////////////////////////////
		}
	return;
	}
	else if(fakeid==0)		
		gl_random_id++;/*++ random id for the running process*/
///////////////// F O R K //////////////////////////////////////////////////////
	if ( (pid=fork())== SENTINEL)
	{
		perror("fork"); 
		exit(1);
	}
	if(pid!=0)
	{
////////////// insert the process to running list //////////////////////////////
//		printf("mpainei sto list run to %d fakeid %d \n",pid,gl_random_id);		
		if(fakeid==0)
		{	
			List_Insert(&list_run,buff,pid,gl_random_id,array[0]);
			if(pipe==1)
			{
/////////////////////////// P I P E 2 //////////////////////////////////////////
///////////////////////// O P E N  P I P E  F O R  W R I T I N G ///////////////
				if ( (writefd = open(FIFO2, O_WRONLY ))  < 0)  
				{
					perror("SERVER: can't open read fifo \n");
				}
				sprintf(input,"< %d , %s > -- RUNNING",gl_random_id,array[0]);
//////////////////////// W R I T E  T O  P I P E ///////////////////////////////
				write(writefd,input,strlen(input));
/////////////////////// C L O S E  P I P E /////////////////////////////////////
				close(writefd);
////////////////////////////////////////////////////////////////////////////////
			}	
		}	
		else
		{
			List_Insert(&list_run,buff,pid,fakeid,array[0]);
			if(pipe==1)
			{
/////////////////////////// P I P E 2 //////////////////////////////////////////
///////////////////////// O P E N  P I P E  F O R  W R I T I N G ///////////////
				if ( (writefd = open(FIFO2, O_WRONLY ))  < 0)  
				{
					perror("SERVER: can't open read fifo \n");
				}
				sprintf(input,"< %d , %s > -- RUNNING",fakeid,array[0]);
//////////////////////// W R I T E  T O  P I P E ///////////////////////////////
				write(writefd,input,strlen(input));
/////////////////////// C L O S E  P I P E /////////////////////////////////////
				close(writefd);
////////////////////////////////////////////////////////////////////////////////
			}
			else
				printf("%d)\n",pid);/*complete print from List_Remove_Head*/

		}

/////// R E D U C E  - 1  GL_TEMP_CONC V A L U E ///////////////////////////////
		gl_temp_conc--;

/////////////// free allocated space for char** array //////////////////////////
		for (i = 0; i <= counter; i++)
		{
   			free(array[i]);
		}
		free(array);
	}
	else
	{
		{
			close(1);
			fd=creat("userlist", 0644);	
			if( ( execvp(array[0],array) )==SENTINEL )/*if execvp fails ,*/
				execv(array[0],array);/* then it not a terminal command*/
			perror("exec ");
			exit(1);
		}
	}
////////////////////////////////////////////////////////////////////////////////
return;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// D I M I O U R G I A  L I S T //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_listas List_Creation()
{
    ptr_listas linfo;
    linfo = malloc(sizeof(Lista_Info));
    linfo->komboi = 0;
	linfo->head = NULL;
    linfo->tail=NULL;
	linfo->current=NULL;
	return linfo;
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////D E L E T E  L I S T ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_Delete(ptr_listas * linfo)
{
	ptr_kombou_listas todel,todel2;
	todel= (*linfo)->tail;
	int SIZE;
	SIZE=(*linfo)->komboi;	
//	printf("List nodes :%d\n",(*linfo)->komboi);
	while(SIZE!=0)
	{   todel2=todel;
		if(todel->next!=NULL)
			todel=(todel)->next;
//		printf("Deleting List node :%c\n",todel2->character);
		free(todel2);
		SIZE--;
	}
	(*linfo)->head= NULL;
	(*linfo)->tail= NULL;
	(*linfo)->current= NULL;
    free(*linfo);
    (*linfo)=NULL;
//	printf("\n------------list successfully deleted------------- \n");
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////// L I S T  K E N I /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int List_Keni(ptr_listas linfo)
{	
	return (linfo->head == NULL );
}
////////////////////////////////////////////////////////////////////////////////
////////////////// E I S A G W G H  L I S T/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_Insert(ptr_listas * linfo,char * word,int pid ,int random_id,char * job)
{	int i;
	ptr_kombou_listas prosorinos; 
//	printf("the word : %s , %d->%d ,%s was just inserted\n",word,pid,random_id,job);
	prosorinos = malloc(sizeof(Lista_Kombos));
	if ( prosorinos == NULL )
	{   printf("ERROR : Could not allocate memory!\n");
		return;
	}
	if (List_Keni(*linfo))
	{
		(*linfo)->head=prosorinos;	
		(*linfo)->tail=prosorinos;
	}
////////////// S E T  T H E  E L E M E N T  T O  T H E  N O D E ////////////////
	strcpy(prosorinos->word,word);
	prosorinos->jobid=pid;
	prosorinos->random=random_id;
	strcpy(prosorinos->job,job);
//	printf("\n\n\neishx8ei to stoixeio , %s me pid %d \n",prosorinos->word,pid);
    prosorinos->next = (*linfo)->head;
    prosorinos->previous=(*linfo)->tail;
	(*linfo)->tail->next=prosorinos;
	(*linfo)->head->previous=prosorinos;
//	(*linfo)->current=prosorinos;
	(*linfo)->tail=prosorinos;
	(*linfo)->komboi++;
//	printf("mphke ??????\n");
/*	printf("\n\n");*/
/*	List_Print((*linfo),1);*/
/*	printf("\n\n");*/
}

////////////////////////////////////////////////////////////////////////////////
////////////////// P R I N T  T H E  L I S T////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void List_Print(ptr_listas linfo)
{   ptr_kombou_listas temp;
	temp=(linfo)->head;
	int SIZE;
	int writefd;
	int i;
	char input[256];
	SIZE=linfo->komboi;

	for(i=0;i<256;i++)
		input[i]='\0';
	if(List_Keni(linfo))
	{
/////////////////////////// P I P E 2 //////////////////////////////////////////
///////////////////////// O P E N  P I P E  F O R  W R I T I N G ///////////////
		if ( (writefd = open(FIFO2, O_WRONLY ))  < 0)  
		{
			perror("SERVER: can't open read fifo \n");
		}
		sprintf(input," The List is empty \n");
//////////////////////// W R I T E  T O  P I P E ///////////////////////////////
		write(writefd,input,strlen(input));
/////////////////////// C L O S E  P I P E /////////////////////////////////////
		close(writefd);
////////////////////////////////////////////////////////////////////////////////
		return;
	}
/////////////////////////// P I P E 2 //////////////////////////////////////////
///////////////////////// O P E N  P I P E  F O R  W R I T I N G ///////////////
	if ( (writefd = open(FIFO2, O_WRONLY ))  < 0)  
	{
		perror("SERVER: can't open read fifo \n");
	}
	while(SIZE!=0)
	{
		sprintf(input,"\n< %d , %s >\n",temp->random,temp->job);
//////////////////////// W R I T E  T O  P I P E ///////////////////////////////
		write(writefd,input,strlen(input));
				
		temp=temp->next;
		SIZE--;
	}
/////////////////////// C L O S E  P I P E /////////////////////////////////////
		close(writefd);
////////////////////////////////////////////////////////////////////////////////
	return;

}
////////////////////////////////////////////////////////////////////////////////
////////////////// L I S T  R E M O V E ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void List_Remove(ptr_listas * linfo,int rpid,int caze)
{	int flag=0;
	int SIZE;
	SIZE=(*linfo)->komboi;
	ptr_kombou_listas temp,current,previous;
	current=(*linfo)->head;

	if ( List_Keni(*linfo) ) 
	{	
	    return;
	} 
/*	printf("\nbefore remove\n");*/
/*	List_Print((*linfo),1);*/
/*	printf("\n\n");*/

////////// F I N D  T H E  N O D E  T O  B E  R E M O V E D ////////////////////
	if(caze==0)
	{
		while(SIZE!=0)
		{
			if(current->random==rpid)
			{
				flag=1;
				break;
			}

			current=current->next;
			SIZE--;			
		}

	}
	else
	{
		while(SIZE!=0)
		{
//			printf("size : %d | %d->%d\n",SIZE,current->jobid,rpid);
			if( (current->jobid) == rpid )	
			{	
				flag=1;
				break;			
			}
			current=current->next;
			SIZE--;			
		}
/// + + G L _ T E M P _ C O N C  O N L Y  F O R  R U N N I N G  P R O C E S S //
		gl_temp_conc++;
	}
	if(flag==0)
	{//	printf("shit\n");
		return;
	}
//	printf("node is about to be removed realpid:%d jobname:%s \n",current->jobid,current->job);
//////////////// R E M O V I N G   P R O C E S S ///////////////////////////////
	if ( (*linfo)->head == current )  //an einai o protos kombos tis listas
	{  
		temp=current->next;
		(*linfo)->head=temp;
		(*linfo)->tail->next=temp;
		temp->previous=(*linfo)->tail;	
//		printf("1 remove %s\n",current->job);
		free(current);
		current=NULL;		
	}	
	else 
	{  
		temp=current->previous;
		temp->next=current->next;
		(temp->next)->previous=current->previous;
//		printf("2 remove %s\n",current->job);
		(*linfo)->tail=temp;	
		free(current);
		current=NULL;
	}

	(*linfo)->komboi--;
	if( (*linfo)->komboi==0 )
	{
		(*linfo)->head=NULL;
		(*linfo)->tail=NULL;
	}

/*	printf("\nafter remove\n");*/
/*	List_Print((*linfo),1);*/
/*	printf("\n\n");*/

	return;
}
////////////////////////////////////////////////////////////////////////////////
////////////////// S E A R C H  T H E  L I S T /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int List_Search(ptr_listas linfo,int randpid,int list)	/*list 1=running*/
{   ptr_kombou_listas temp;								/*list 2=queued*/
	temp=(linfo)->tail;
	int SIZE;
	int flag=0;
	SIZE=linfo->komboi;
//	printf("serching\n");
	if(List_Keni(linfo))
		return 0;
	while(temp->random!=randpid && SIZE!=0)
	{
		temp=temp->next;
		SIZE--;
	}
	if (temp->random==randpid)
		flag=1;
//	printf("jobid %d rand %d==%d\n",temp->jobid,temp->random,randpid);
	if(list==2 && flag==1)	
	{	
		if (temp->random==randpid)
		{//	printf("kalw remove %d \n",randpid);
			List_Remove(&linfo,randpid,0);
			return 1;/*bre8hke*/
		}
		else 
			return 0;
	}
	else if (list==1 && flag==1)
	{	
//		printf("KILL to %d \n",temp->jobid);
		kill(temp->jobid,SIGKILL);
		return 1;
	}
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////////
////////////////// L I S T  E X E C U T E  H E A D /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_Execute_Head(ptr_listas * linfo )
{		int SIZE;
		SIZE=(*linfo)->komboi;
		ptr_kombou_listas temp,current;
		current=(*linfo)->head;
		if(List_Keni(*linfo))
			return;
////////// R E M O V E  H E A D  F R O M  L I S T _ Q U E U E ////////////////// 	
		temp=current->next;
		(*linfo)->head=temp;
		(*linfo)->tail->next=temp;
		temp->previous=(*linfo)->tail;	
//		printf("1 remove %s\n",current->job);
////////////////////////////////////////////////////////////////////////////////
		printf("execute from queue list : < %d , %s > (PID : ",current->random,current->job);				
		Executor( current->word ,current->random,0);/*0 means dont write to pipe*/
////////////////////////// F R E E  N O D E ////////////////////////////////////
		free(current);
		current=NULL;		
		(*linfo)->komboi--;
		if( (*linfo)->komboi==0 )
		{
			(*linfo)->head=NULL;
			(*linfo)->tail=NULL;
		}	
		return;
}	


