#include <stdio.h>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>	         /* gethostbyaddr */
#include <stdlib.h>	         /* exit */
#include <string.h>	         /* strlen */
#include "client_declarations.h"

int main(int argc,char *argv[])
{
int flag_i=0;/* if "i"option appears more than once*/
int flag_p=0;/* if "p"option appears more than once*/
int flag_d=0;/* if "d"option appears more than once*/
char server_ip[20];
int server_port;
char *path;
char *talk_path;
char buff[10];
int i,j,sock;
int path_length=0;
int counter	;/* Number of tasks to receive from server*/
int length=0;/*Length of each path arrives from server*/
struct sockaddr_in server;
struct sockaddr *serverptr = (struct sockaddr*)&server;
struct hostent *rem;


for(i=0;i<20;i++)
	server_ip[i]=0;
////////////////////////////////////////////////////////////////////////////////
//////////////////////// A R G U M E N T S  C O N T R O L //////////////////////
////////////////////////////////////////////////////////////////////////////////
	if(argc==7)
	{
		for(i=1;i<argc;i++)
		{
			if(!strcmp(argv[i],"-i"))
			{	flag_i++;
				strcpy(server_ip,argv[i+1]);
				i++;
			}
			else if(!strcmp(argv[i],"-p"))
			{	flag_p++;
				server_port=atoi(argv[i+1]);
				i++;
			}
			else if(!strcmp(argv[i],"-d"))
			{	flag_d++;
				path_length=strlen(argv[i+1]);
				path=malloc(path_length + 1);	/*1 is size of character*/
				strcpy(path,argv[i+1]);
				i++;
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
			printf("Error: you gave too many arguments\nplease try [./remoteClient -i <server_ip> -p <server_port> -d <directory>]\n");
		else
			printf("Error: you gave less arguments\nplease try [./remoteClient -i <server_ip> -p <server_port> -d <directory>]\n");
		exit(1);
	}
	if(flag_p==1 && flag_d==1 && flag_i==1)
		printf("port : %d \nserverIP : %s \ndirectory : %s \n",server_port,server_ip,path);
	else
	{
		printf("ERROR : same option twice\n");
		exit(1);
	}
//////////////////////// C R E A T  S O C K E T ////////////////////////////////

	if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0)
		perror_exit("socket");

////////////////////// F I N D  S E R V E R  A D D R E S S /////////////////////
	if ( ( rem = gethostbyname( server_ip ) ) == NULL )
	{
		herror("gethostbyname");
		exit(1);
	}
/////////////// F I L L  S E R V E R 'S  S T R U C T ///////////////////////////

	server.sin_family = AF_INET;	/* Internet domain */
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(server_port);

//////////////////// I N I T I A T E  C O N N E C T I O N //////////////////////

	if (connect(sock, serverptr, sizeof(server)) < 0)
		perror_exit("connect");
	printf("Connecting on %s port %d\n", server_ip,server_port);

///////////// S E N D  L E N G T H  O F  P A T H  //////////////////////////////
	sprintf(buff,"%d",path_length);

	while ( write(sock, buff , 10) == -1);	/* if fail try again */

///////////// S E N D  T H E  P A T H  /////////////////////////////////////////

	while (write(sock, path, path_length) == -1 );	/* if fail try again */

	while ( read(sock,&counter,sizeof(int)) == -1 );/*read number of paths about to come*/

	for(i=0;i<counter;i++)
	{	length=0;
		/*read length of path*/
		while(read(sock,&length,sizeof(int)) ==-1 );
//		printf("length=%d \n",length);
		/*allocate the exact space needed*/
		talk_path=malloc( length + 1 );

		for (j=0;j<=length;j++)
			talk_path[j]='\0';
		/*read the path*/
		while(read(sock,talk_path,length) == -1 );
		printf("Received : %s \n",talk_path);
		Break_Path(talk_path,sock);

		free(talk_path);
		talk_path=NULL;
	}

///////////// C L O S E  S O C K E T ///////////////////////////////////////////
	close(sock);
///////////// F R E E  A L L O C A T E D  P A T H //////////////////////////////
	free(path);
}

