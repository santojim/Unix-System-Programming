#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "client_declarations.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>


////////////////////////////////////////////////////////////////////////////////
//////////////////////// P E R R O R _ E X I T  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void perror_exit( char * message )
{
    perror( message );
    exit( EXIT_FAILURE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////// B R E A K  P A T H ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Break_Path(char * path,int sock)
{
	int i,j,md;
	int length;

	length=strlen(path);

	char buffer_path[length];

	for(i=0;i<=length;i++)
	{
		if(path[i]=='/')
		{
			buffer_path[i]='\0';
			File_Creator(buffer_path,sock,0);/*zero means folder*/
		}
		buffer_path[i]=path[i];
		buffer_path[i+1]='\0';
	}
	File_Creator(buffer_path,sock,1);
	return;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////// F I L E  C R E A T O R //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void File_Creator(char * path,int sock,int end)	/*end == 1 means that path's last element*/
{
	struct stat statbuf;
	DIR * dir_ptr ;
	int md,fd,i;
	int length=strlen(path);
	int page;
	int repeat;
	page=getpagesize();
	char buff[page];
	if (end==0)
	{
		if( ( dir_ptr=opendir(path) ) ==NULL )	/*if fail to open directory*/
		{	/*create folder*/
			md=mkdir(path,0777);
			if (md==-1)
				perror("mkdir");
		}
	}
	else
	{
		stat(path,&statbuf);
		/*if file exists remove it*/
		if(statbuf.st_size!=0)
			remove(path);
		/*create it*/
		creat(path,0777);
		fd=open(path,O_WRONLY);
		/* __1__ read repeat number*/
		while( ( read(sock,&repeat,sizeof(int))) == -1);

		if(repeat>0)
		{
			do{
				/*__2__read buff's size from socket*/
				while( ( read(sock,&length,sizeof(int))) == -1);
				/*__3__read the buff from socket*/
				while( ( read(sock,buff,length) )==-1 );
				/*write buff to file*/
				write(fd,buff,length);
				repeat--;
				/*__4__initiallise buff for the next repeat*/
				for(i=0;i<page;i++)
					buff[i]='\0';
			}
			while(repeat>0);
		}
		close(fd);
	}
	return;
}















