#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "declarations.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

////////////////////////////////////////////////////////////////////////////////
///////////////////// S I G U S R 1  H A N D L E R /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void exit_handler(int sig)
{
	gl_exit_flag=1;
	int sock;
	char server_ip[]="127.0.0.1";
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct hostent *rem;
//////////////////////// C R E A T  S O C K E T ////////////////////////////////
	if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0)
		perror_exit("signal socket");
////////////////////// F I N D  S E R V E R  A D D R E S S /////////////////////
//	serverlen=sizeof(server);
	if ( ( rem = gethostbyname(server_ip) ) == NULL )
	{
		herror("gethostbyname");
		return;
	}
/////////////// F I L L  S E R V E R 'S  S T R U C T ///////////////////////////
	server.sin_family = AF_INET;	/* Internet domain */
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);
//////////////////// I N I T I A T E  C O N N E C T I O N //////////////////////
	if (connect(sock, serverptr, sizeof(server)) < 0)
		perror_exit("connect");
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////// P E R R O R _ E X I T  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void perror_exit( char * message )
{
    perror( message );
    exit( EXIT_FAILURE );
}
////////////////////////////////////////////////////////////////////////////////
////////////// W O R K E R  T H R E A D  F U N C T I O N ///////////////////////
////////////////////////////////////////////////////////////////////////////////
/*inspired by sync_by_mutex ->code-6-set*/
/*also inspired by book marc j rochkind system programming page 373*/
void * Worker_Thread_F(void * n)
{	int err;
	int length;
	int fd,i,counter=0;
	int page=getpagesize();
	char buff[page];/*the buff to be send to socked file descriptor*/
	int buff_length;/* buff string's length*/
	int file_size;/*file's size */
	int repeat;/*repeat the read-write process (file_size/pagesize)*/
	int read_bytes;/*stores read's bytes  */
	struct stat statbuf;

	while(gl_exit_flag==0)
	{
//////////// M U T E X  F O R  S O C K E T  P R O T E C T I O N ////////////////
		if (err = pthread_mutex_lock(&mtx))
		{ /* Lock mutex */
			perror("pthread_mutex_lock");
			exit(1);
		}
		length=0;
		while(List_Empty(list_queue) && gl_exit_flag==0)
		{
			pthread_cond_wait(&cond,&mtx);
		}
		if(gl_exit_flag==1)
		{
			pthread_mutex_unlock(&mtx);
			pthread_cond_signal(&cond);
			printf("[Thread: %lu] -> EXITED \n",pthread_self());
			pthread_exit(0);
		}
		printf("[Thread: %lu] : Received task:<%s,%d> \n",pthread_self(),list_queue->head->path,list_queue->head->sockfd);
		/*find size of path to be send*/
		length=strlen(list_queue->head->path);
		/*send it to client*/
		while (write(list_queue->head->sockfd,&length,sizeof(int))==-1);
		/*now send the path */
		while (write(list_queue->head->sockfd,list_queue->head->path,length)==-1);
		printf("[Thread: %lu] : About to read file %s \n",pthread_self(),list_queue->head->path);
		/*open the file for reading*/
		fd=open(list_queue->head->path,O_RDONLY);
		/*get file's size*/
		if ( stat(list_queue->head->path,&statbuf)== -1)
		{	perror ("stat");
			exit (1) ;
		}
		file_size=statbuf.st_size;
		if(file_size==0)
			repeat=0;
		else
			repeat=(file_size/page)+1;/*+1 for the float loss in each devision*/

///////////------------------------------------------------------///////////////
			/*__1__send repeat number to socket*/
			while( write (list_queue->head->sockfd,&repeat,sizeof(int) ) == -1 );
			if(repeat>0)
			{ 	/*read from file until page size and repeat*/
				while( ( read_bytes=read(fd,buff,page) ) > 0  )
				{
					/*__2__size of buff to be send to socket*/
					while( write (list_queue->head->sockfd,&read_bytes,sizeof(int) ) == -1 );
					/*__3__send the buff to socket*/
					while ( write(list_queue->head->sockfd,buff,read_bytes)==-1 );
					/*__4__initiallise buff for the next file*/
					for(i=0;i<page;i++)
						buff[i]='\0';
				}
			}
///////////------------------------------------------------------///////////////
		/*close file descriptor*/
		close(fd);
////////////// L I S T  R E M O V E  P R O T E C T I O N ///////////////////////

		/*remove it from list_queue*/
		List_Remove(&list_queue,list_queue->head->path,list_queue->head->sockfd);
///////////// U N L O C K  S O C K E T  P R O T E C T I O N ////////////////////
		if (err = pthread_mutex_unlock(&mtx))
		{ /* Unlock mutex */
			perror("pthread_mutex_unlock");
			exit(1);
		}
	}

}
////////////////////////////////////////////////////////////////////////////////
/////////////// ( T H R E A D ) C O N N E C T I O N ////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void * Connection(void * fd)
{	int counter=0;
	int sock= *((int *) fd);
	int length=0;
	char buff[10];
	int i,data;
	int err;
	ssize_t x;
////////////////////// R E A D  L E N G T H  O F  P A T H //////////////////////

	while (read(sock, buff, 10) == -1);

////////////////////// C O N V E R T  I T  T O  I N T E G E R //////////////////
	length=atoi(buff);

	char path[length];

	for(i=0;i<=length;i++)
		path[i]='\0';

 ///////////////////// R E A D  T H E  P A T H //////////////////////////////////

	while (read(sock, path, length) == -1);

	counter=Count_Entities(path,&counter);

	while ( write(sock,&counter,sizeof(int)) == -1 );/*send the number to client*/

	Read_Directory(path,sock);

////////////////// C L O S E  S O C K E T  D E S C R I P T O R /////////////////
/*inspired by :*/
/*http://stackoverflow.com/questions/17705239/is-there-a-way-to-detect-that-tcp-socket-has-been-closed-by-the-remote-peer-wit*/
	while(1)
	{
		x = recv(sock, &data, 1, MSG_PEEK);
		if (x == 0)
		{
			printf("Closing connection.\n");
			close(sock);
			break;
		}
	}
///////////////// E X I T  T H R E A D /////////////////////////////////////////
/*thread created as detached so any resources are freed*/
	pthread_exit(0);
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// C O U N T   E N T I T I E S ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int Count_Entities( char dirname [],int *counter)
{
	int val=0;
	char newdirname[256];
	DIR * dir_ptr ;
	struct dirent   prev_dir;
	struct dirent * direntp;

	if ( ( dir_ptr = opendir ( dirname ) ) == NULL )
		fprintf (stderr , " cannot open %s \n " , dirname ) ;

	if ((val = readdir_r(dir_ptr, &prev_dir,&direntp))!=0)
	{
		fprintf (stderr , " readdir_r error \n ") ;
		exit(1);
	}
	while( val==0 && direntp!=NULL)
	{
//////////////////C A S E  I S  D I R E C T O R Y //////////////////////////////
		if (prev_dir.d_type==DT_DIR)
		{

/////////////////G O  I N  T H E  D I R E C T O R Y ////////////////////////////
			if(strcmp((prev_dir.d_name),".")!=0  && strcmp((prev_dir.d_name),"..")!=0)
			{
				strcpy(newdirname,dirname);
				strcat(newdirname,"/");
				strcat(newdirname,prev_dir.d_name);
/////////////////C A L L  R E C U R S I V E ////////////////////////////////////
				Count_Entities(newdirname,counter);
			}
		}
		else if (prev_dir.d_type==DT_REG)
			(*counter)++;
		if ((val = readdir_r(dir_ptr, &prev_dir,&direntp))!=0)
		{
			fprintf (stderr , " readdir_r error \n ") ;
			break;
		}
	}
	closedir (dir_ptr);
	return *counter;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////// R E A D  D I R E C T O R Y /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*http://stackoverflow.com/questions/11531245/reading-directories-using-readdir-r*/
void Read_Directory(char *path,int sock)
{	int err;
	char newdirname[256];
	char current_work_dir[256];
	char temp[256];
	int length;
	int val=0;
	int i,j;
	DIR * dir_ptr ;
	struct dirent   prev_dir;
	struct dirent * direntp;
////////////////// I N I T I A L I S E  A R R A Y S ////////////////////////////
	for(i=0;i<256;i++)
	{
		current_work_dir[i]='\0';
		temp[i]='\0';
		newdirname[i]='\0';
	}
////////////// O P E N  D I R E C T O R Y //////////////////////////////////////
	if ( ( dir_ptr = opendir ( path ) ) == NULL )
		fprintf (stderr , " cannot open %s \n " , path ) ;
	if ((val = readdir_r(dir_ptr, &prev_dir,&direntp))!=0)
	{
		fprintf (stderr , " readdir_r error \n ") ;
		exit(1);
	}
////////////// W H I L E  R E A C H  E N D  O N  F O L D E R S /////////////////
	while( val==0 && direntp!=NULL)
	{
////////////////// C A S E  I S  D I R E C T O R Y /////////////////////////////
		if (prev_dir.d_type==DT_DIR)
		{
/////////////////G O  I N  T H E  D I R E C T O R Y ////////////////////////////
			if(strcmp((prev_dir.d_name),".")!=0  && strcmp((prev_dir.d_name),"..")!=0)
			{
				strcpy(newdirname,path);
				strcat(newdirname,"/");
				strcat(newdirname,prev_dir.d_name);
/////////////////C A L L  R E C U R S I V E ////////////////////////////////////
				Read_Directory(newdirname,sock);
			}
		}
		else if (prev_dir.d_type==DT_REG)
		{
			strcpy(newdirname,path);
			strcat(newdirname,"/");
			strcat(newdirname,prev_dir.d_name);
			printf("[Thread: %lu] : Adding file %s to the queue...\n",pthread_self(),newdirname);
			while (List_Full(list_queue))	/*wait untill a worker thread free space*/
			{//	printf(" LIST FULL \n");
				sleep(1);
			}
//////// L O C K  M U T E X  B E F O R E  I N S E R T  T O  L I S T ////////////
			if (err = pthread_mutex_lock(&mtx))
			{ 	/*mtx lock*/
				perror("pthread_mutex_lock");
				exit(1);
			}
///////////// F O R  A B S O L U T E  P A T H S ////////////////////////////////
		if(newdirname[0]=='/')
		{	/*get current work directory*/
			getcwd(current_work_dir,256);
			length=strlen(current_work_dir);
			i=length;
			for(j=0;j<256;j++)
			{
				i++;
				temp[j]=newdirname[i];
				temp[j+1]='\0';
				if(i>=256)
					break;

			}
			List_Insert(&list_queue,temp,sock);
		}
		else
///////////// F O R  R E L E V A N T   P A T H S ///////////////////////////////
			List_Insert(&list_queue,newdirname,sock);
/////////////// S I G N A L  W O R E R  T H R E A D S //////////////////////////
			pthread_cond_signal(&cond);

///////////// A F T E R  L I S T  I N S E R T  U N L O C K  M T X //////////////
			if (err = pthread_mutex_unlock(&mtx))
			{ /* mtx unlock */
				perror("pthread_mutex_unlock");
				exit(1);
			}
		}
		if ((val = readdir_r(dir_ptr, &prev_dir,&direntp))!=0)
		{
			fprintf (stderr , " readdir_r error \n ") ;
			break;
		}
	}
	closedir (dir_ptr);
	return;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////// L I S T  E M P T Y ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int List_Empty(ptr_listas linfo)
{
	return (linfo->head == NULL );
}


////////////////////////////////////////////////////////////////////////////////
///////////////////////// L I S T  F U L L /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int List_Full(ptr_listas linfo)
{
	return ( linfo->max_nodes == linfo->komboi );
}


////////////////////////////////////////////////////////////////////////////////
//////////////////// D I M I O U R G I A  L I S T //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_listas List_Creation(int max)
{
    ptr_listas linfo;
    linfo = malloc(sizeof(Lista_Info));
    linfo->komboi = 0;
	printf("list creation %d\n",max);
	linfo->max_nodes=max;
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
////////////////// E I S A G W G H  L I S T/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_Insert(ptr_listas * linfo,char * path,int sockfd)
{	int i;
	ptr_kombou_listas prosorinos;
//	printf("the path : %s , %d->%d ,%s was just inserted\n",path,pid,random_id,job);
	prosorinos = malloc(sizeof(Lista_Kombos));
	if ( prosorinos == NULL )
	{   printf("ERROR : Could not allocate memory!\n");
		return;
	}
	if (List_Empty(*linfo))
	{
		(*linfo)->head=prosorinos;
		(*linfo)->tail=prosorinos;
	}
////////////// S E T  T H E  E L E M E N T  T O  T H E  N O D E ////////////////
	strcpy(prosorinos->path,path);
	prosorinos->sockfd=sockfd;
//	printf("\n\n\neishx8ei to stoixeio , %s me sockfd %d \n",prosorinos->path,sockfd);
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
//////////////////// R E M O V E  F R O M  L I S T /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void List_Remove(ptr_listas * linfo,char * path,int sockfd)
{
	ptr_kombou_listas temp,current,previous;
	current=(*linfo)->head;
	int SIZE;
	SIZE=(*linfo)->komboi;
	if(List_Empty(*linfo))
		return;
/////////////// F I N D  N O D E  T O  B E  R E M O V E D //////////////////////
	while(SIZE!=0)
	{
		if(!strcmp(current->path,path))
			if(sockfd==current->sockfd)
				break;
		current=current->next;
		SIZE--;
	}
//	printf("node to be removed %s : %d \n",current->path,current->sockfd);
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
	return;
}

////////////////////////////////////////////////////////////////////////////////
////////////////// P R I N T  T H E  L I S T////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void List_Print(ptr_listas linfo)
{   ptr_kombou_listas temp;
	temp=(linfo)->head;
	int SIZE;
	SIZE=linfo->komboi;
	if(List_Empty(linfo))
	{	//printf("The List is empty\n");
		return;
	}
		fprintf(stderr,"\n");
	while(SIZE!=0)
	{
		fprintf(stderr,"%s \n",temp->path);

		temp=temp->next;
		SIZE--;
	}

}















