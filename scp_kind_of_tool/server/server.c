#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "declarations.h"
#include <pthread.h>

#define MAXCONNECTIONS 10

int main(int argc, char *argv[])
{

signal(SIGINT,exit_handler);
////////////////////////////////////////////////////////////////////////////////
////////////// V A R I A B L E S  D E C L A R A T I O N ////////////////////////
////////////////////////////////////////////////////////////////////////////////
int err;
int queue_size=0;
int thread_pool_size=0;
int i,sock,newsock;
int flag_p=0;/* if "p"option appears more than once*/
int flag_s=0;/* if "s"option appears more than once*/
int flag_q=0;/* if "q"option appears more than once*/

pthread_t accept_thread;
pthread_attr_t attr;
pthread_t * worker_thread;
socklen_t clientlen;

struct sockaddr_in server, client;
struct sockaddr *serverptr=(struct sockaddr *)&server;
struct sockaddr *clientptr=(struct sockaddr *)&client;
struct hostent *rem;


////////////////////////////////////////////////////////////////////////////////
//////////////////////// A R G U M E N T S  C O N T R O L //////////////////////
////////////////////////////////////////////////////////////////////////////////
	if(argc==7)
	{
		for(i=1;i<argc;i++)
		{
			if(!strcmp(argv[i],"-p"))
			{
				port=atoi(argv[i+1]);
				i++;
				flag_p++;
			}
			else if(!strcmp(argv[i],"-s"))
			{
				thread_pool_size=atoi(argv[i+1]);
				i++;
				flag_s++;
			}
			else if(!strcmp(argv[i],"-q"))
			{
				queue_size=atoi(argv[i+1]);
				i++;
				flag_q++;
			}
			else
			{
				printf("option : %s not supported \n",argv[i]);
				exit(1);
			}
		}
	}
	else
	{	if (argc>7)
			printf("Error: you gave too many arguments\nplease try [./dataServer -p <port> -s <thread_pool_size> -q <queue_size>]\n");
		else
			printf("Error: you gave less arguments\nplease try [./dataServer -p <port> -s <thread_pool_size> -q <queue_size>]\n");
		return(1);
	}
	if(flag_p==1 && flag_q==1 && flag_s==1)
		printf("port : %d \npool_size : %d \nqueue : %d \n",port,thread_pool_size,queue_size);
	else
	{
		printf("ERROR : same option twice\n");
		exit(1);
	}
/////////////// C R E A T  P A T H  Q U E U E //////////////////////////////////

list_queue=List_Creation(queue_size);

//////////////////////// I N I T I A L I Z E  V A L U E S  /////////////////////

gl_exit_flag=0;
/*http://stackoverflow.com/questions/10788014/thread-created-detached-never-executed*/
pthread_attr_init(&attr);

pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

pthread_cond_init(&cond, 0);

pthread_mutex_init(&mtx,NULL);

printf("Server was successfully initialized...\n");
/////////////////////// C R E A T  T H R E A D  P O O L ////////////////////////

//////// A L L O C A T E  S P A C E  F O R  T H R E A D  P O I N T E R /////////
	if ((worker_thread = malloc(sizeof(pthread_t)*thread_pool_size)) == NULL)
	{
		perror("malloc");
		exit(0);
	}
////////////////////// C R E A T E  T H R E A D S //////////////////////////////
	for (i=0; i<thread_pool_size; i++)
	{
		pthread_create(worker_thread+i,&attr, Worker_Thread_F, NULL);
		usleep(3000);
	}
//////////////////////// C R E A T  S O C K E T ////////////////////////////////

	if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0)
		perror_exit("socket");

/////////////// F I L L  S E R V E R S  S T R U C T ////////////////////////////

	server.sin_family = AF_INET;	/* Internet domain */
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

////////////// A S S I G N  A D D R E S S  T O  S O C K E T ////////////////////

	if (bind(sock, serverptr, sizeof(server)) < 0)
		perror_exit("bind");

////////////// L I S T E N  F O R  C O N E C T I O N S  ////////////////////////

	if (listen(sock, MAXCONNECTIONS) < 0)
		perror_exit("listen");
	printf("Listening for connections to port %d\n", port);
	while(gl_exit_flag==0)
	{
		clientlen = sizeof(client);

////////////// A C C E P T  A  C O N N E C T I O N /////////////////////////////

   	if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
			perror_exit("accept");

////////////// F I N D  C L I E N T 'S  A D D R E S S //////////////////////////

		if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL)
		{
			herror("gethostbyaddr");
			exit(1);
		}
		if(gl_exit_flag==0)
			printf("Accepted connection from %s\n", rem->h_name);

////////////// A C C E P T _ T H R E A D  C R E A T I O N //////////////////////
		/*create detached threads*/
		pthread_create(&accept_thread,&attr, Connection,&newsock);

	}
////////////// D E S T R O Y  M U T E X ////////////////////////////////////////
	err = pthread_mutex_destroy(&mtx);
	if (!err)
	{
		perror("pthread_mutex_destroy");
		exit(1);
	}
	pthread_cond_destroy(&cond);
///////////////////// L I S T  D E L E T E /////////////////////////////////////
	if(gl_exit_flag==1)
	{
/////////////////// U N B L O C K  W O R K E R  T H R E A D S //////////////////
		for(i=0;i<1;i++)
		{
			pthread_cond_signal(&cond);
			usleep(1000000);
		}
	}
	List_Delete(&list_queue);
	printf("List removed\n");
	printf("Server OFF\n");

}


