#include <stdio.h>
#include <malloc.h>
#include "header.h"
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

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
void List_Insert(ptr_listas * linfo,char stoixeio)
{
	ptr_kombou_listas prosorinos; 

	prosorinos = malloc(sizeof(Lista_Kombos));
	if ( prosorinos == NULL )
	{   printf("ERROR : Could not allocate memory!\n");
		return;
	}
	if (List_Keni(*linfo))
	{
		(*linfo)->head=prosorinos;
		(*linfo)->current=prosorinos;
		(*linfo)->tail=prosorinos;
	}
////////////// S E T  T H E  E L E M E N T  T O  T H E  N O D E ////////////////
	prosorinos->character=stoixeio;
	prosorinos->tree=NULL;
//	printf("eishx8ei to stoixeio , %c \n",prosorinos->character);
    prosorinos->next = (*linfo)->current;
	(*linfo)->current=prosorinos;
	(*linfo)->tail=prosorinos;
	(*linfo)->komboi ++;
//	printf("komboi=%d\n",(*linfo)->komboi);
}
////////////////////////////////////////////////////////////////////////////////
////////////////// C H E C K  T H E  L I S T////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int List_Check(ptr_listas linfo,char c)
{   ptr_kombou_listas temp;
	temp=(linfo)->tail;
	int SIZE;
	SIZE=linfo->komboi;
	if(List_Keni(linfo))
		return 0;
	while(temp->character!=c && SIZE!=0)
	{
		temp=temp->next;
		SIZE--;
	}
	if (temp->character==c)
		return 1;/*bre8hke*/
	else 
		return 0;
}
////////////////////////////////////////////////////////////////////////////////
////////////////// P R I N T  T H E  L I S T////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void List_Print(ptr_listas linfo,int caze)/*caze defines if a space will be print*/
{   ptr_kombou_listas temp;
	temp=(linfo)->head;
	int SIZE;
	SIZE=linfo->komboi;
	if(List_Keni(linfo))
	{	//printf("The List is empty\n");
		return;
	}
		fprintf(stderr,"\n");
	while(SIZE!=0)
	{	if (caze==0)
			fprintf(stderr,"%s",temp->word);
		else
			fprintf(stderr,"%s ",temp->word);
		temp=temp->next;
		SIZE--;
	}

}
////////////////////////////////////////////////////////////////////////////////
////////////////// E I S A G W G H  2  L I S T//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_2_Insert(ptr_listas * linfo,char *word,int abc)
{	int i;
	ptr_kombou_listas prosorinos; 
//	printf("the word : %s , was just inserted\n",word);
	prosorinos = malloc(sizeof(Lista_Kombos));
	if ( prosorinos == NULL )
	{   printf("ERROR : Could not allocate memory!\n");
		return;
	}
	for(i=0;i<50;i++)
		prosorinos->word[i]='\0';
	if (List_Keni(*linfo))
	{
		(*linfo)->head=prosorinos;	
		(*linfo)->tail=prosorinos;
	}
////////////// S E T  T H E  E L E M E N T  T O  T H E  N O D E ////////////////
	strcpy(prosorinos->word,word);
//	printf("\n\n\neishx8ei to stoixeio , %s \n",prosorinos->word);
if(abc==0)
{
    prosorinos->next = (*linfo)->head;
    prosorinos->previous=(*linfo)->tail;
	(*linfo)->tail->next=prosorinos;
	(*linfo)->head->previous=prosorinos;
	(*linfo)->current=prosorinos;
	(*linfo)->tail=prosorinos;
	(*linfo)->komboi++;
}
else
{	//printf("kalw abc\n");
	List_Sort((linfo),&prosorinos);
	//printf("destuck\n");
}	
}
////////////////////////////////////////////////////////////////////////////////
////////////////// G E T C H A R _ S I L E N T /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int getchar_silent()
{
    int ch;
    struct termios oldt, newt;

    /* Retrieve old terminal settings */
    tcgetattr(STDIN_FILENO, &oldt);

    /* Disable canonical input mode, and input character echoing. */
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* Read next character, and then switch to old terminal settings. */
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
////////////////////////////////////////////////////////////////////////////////
/////////////////////////// L I S T  S O R T ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_Sort(ptr_listas * linfo,ptr_kombou_listas *new)
{		int SIZE;
		ptr_kombou_listas temp;
		temp=(*linfo)->head;
		SIZE=(*linfo)->komboi;
if((*linfo)->head==(*linfo)->tail && (*linfo)->komboi==0)
{	
		(*new)->next =(*linfo)->tail;
	    (*new)->previous=(*linfo)->head;
		(*linfo)->tail=(*new);
		(*linfo)->head=(*new);
		(*linfo)->komboi++;
		return;
}
//printf("%s ,%s \n",(*linfo)->head->word,(*new)->word);
if(strcmp((*linfo)->head->word,(*new)->word)>0)
{
	(*new)->next=(*linfo)->head;
	(*linfo)->head->previous=(*new);
	(*linfo)->head=(*new);
	(*new)->previous=NULL;
	(*linfo)->komboi++;	
}
else if(strcmp((*linfo)->head->word,(*new)->word)<0)
{
	temp=(*linfo)->head;
	while(strcmp(temp->word,(*new)->word)<0 && SIZE>=0)
	{	
		temp=temp->next;
		SIZE--;
	}
	(*new)->previous=temp->previous;

	(*new)->next=temp;

	temp->previous->next=(*new);
		
	temp->previous=(*new);

	(*linfo)->komboi++;

//	(*linfo)->head->previous=(*linfo)->tail;	
}
}
////////////////////////////////////////////////////////////////////////////////
////////////////// D E L E T E  2  L I S T /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void List_2_Delete(ptr_listas * linfo)
{
	ptr_kombou_listas todel,todel2;
	todel= (*linfo)->head;
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


