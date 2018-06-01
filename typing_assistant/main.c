#include <stdio.h>
#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>

#define N 9 /*Number of suggestions*/
#define CHILD 26/*Number of children per treenode */


int main(int argc,char *argv[])
{
char path[50];
int i=0,j=0,k=0;
int flag=0;
int flag2=0;/*for each new line try to list_insert*/
int answer;
ptr_listas lista;
lista=List_Creation();
ptr_tree root,node;
FILE *fp;
for(i=0;i<50;i++)
	path[i]='\0';
////////////////////////////////////////////////////////////////////////////////
//////////////////////// A R G U M E N T S  C O N T R O L //////////////////////
////////////////////////////////////////////////////////////////////////////////
if(argc==3)
{
	if(!strcmp(argv[1],"-d"))
		fp=fopen(argv[2],"r");
	else
	{
		printf("Wrong arguments, please try [./typing-assistant -d path]\n");
		return;
	}
	if(fp==NULL)
	{
		printf("Cannot open the file\n");
		return;
	}
	strcpy(path,argv[2]);
}
else if(argc==1)
{
	fp=fopen("/home/.dict","r");
	if(fp==NULL)
	{
		printf("Cannot open the file\n");
		return;
	}
	strcpy(path,"/home/.dict");
}
else
{
	printf("Wrong arguments, please try [./typing-assistant -d path\n");
	return;
}

char c;
char buffer[50];/*apo8hkeuei gia ka8e le3h mexri poio shmeio exei diabastei*/
int letters;
for (i=0;i<50;i++)
{
	buffer[i]='\0';
}
////////////////////////////////////////////////////////////////////////////////
//////////// F I L L  T H E  S T R U C T S  F R O M  T H E  .T X T /////////////
////////////////////////////////////////////////////////////////////////////////
while((c = fgetc(fp)) != EOF )/*read the first letter of a word*/
{
	if((List_Check(lista,c)==0 || flag==0) && flag2==0)/*to flag gia thn prwth fora parakampte thn List_Check*/
	{	flag=1;
		List_Insert(&lista,c);/*put the character in the list*/

		root=Tree_Node_Creation(CHILD,N);/*make a new root*/
		root=Tree_Link_Root(lista,root,c);	/*link the root treenode with the list*/
	}
	if (c=='\n')
	{
		flag2=0;
//		printf("%c",c);
		letters=strlen(buffer);
		buffer[letters]='@';
		/*carefull check root value*/
//		printf("root value is %c\n",root->character);
		Tree_Link_Node(root,buffer,CHILD,N,0,path);/*put char "@" at end of each word*/
			for (i=0;i<50;i++)
			{
				buffer[i]='\0';
			}
			j=0;
	}
	else
	{	buffer[j]=c;
		j++;
		if(j==1)/*for the first letter of every word*/
		{
			root=Tree_Return_Root(lista,c);
		}
		Tree_Link_Node(root,buffer,CHILD,N,0,path);
//		printf("%s \n",buffer);
		flag2=1;
	}
}
fclose(fp);
printf("\n Read from file finished \n");
sleep(1);
printf("\n\n");
//////////////// C L E A R  T H E  S C R E E N /////////////////////////////////
system("clear");
sleep(1);
printf("Ready for typing , give ctr+D for exit\n");
////////////////////////////////////////////////////////////////////////////////
////////////// R E A D  F R O M  C O M A N D  L I N E  /////////////////////////
////////////////////////////////////////////////////////////////////////////////
{
	flag2=0;
	j=0;
    int next;
	int upper;
	char ubuffer[50];
	for(i=0;i<50;i++)
		buffer[i]='\0';
	for(i=0;i<50;i++)
		ubuffer[i]='\0';
ptr_listas list;
ptr_listas sugg_abc;
ptr_kombou_listas lnode_tempor;
list=List_Creation();
    /* Keep reading one character at a time */
    while ((next = getchar_silent()) != EOF)
    {	//printf("stacked here\n");
        /* Print normal characters */
//		printf("you gave |%c| \n",next);
        if (isalpha(next) || ispunct(next) || next == ' ' || next == '\n')
		{	putchar(next);
			upper=0;
			if(isupper(next))
			{
			//	printf("kefalaio\n");
			//	putchar(next+32);
				upper=next;
				next=next+32;
			}
			if((List_Check(lista,next)==0) && flag2==0 && next!=' ' && next!='	')/*to flag gia thn prwth fora parakampte thn List_Check*/
			{
				List_Insert(&lista,next);/*put the character in the list with tree roots*/

				root=Tree_Node_Creation(CHILD,N);/*make a new root*/
				root=Tree_Link_Root(lista,root,next);	/*link the root treenode with the list*/
			}
			if (next=='\n' || next==' ')/*for every new word*/
			{
				flag2=0;

				/*Insert the word (without the '@' in the end to the local list)*/

				letters=strlen(ubuffer);
				ubuffer[letters]=next;
				List_2_Insert(&list,ubuffer,0);/*so the list takes ' ' and '\n'*/

				letters=strlen(buffer);
				if (letters==0)/*case of ' ' or '\n' don't insert on tree'*/
				{	for (i=0;i<50;i++)
					{
						buffer[i]='\0';
					}
					for (i=0;i<50;i++)
					{
						ubuffer[i]='\0';
					}
					j=0;
					continue;
				}
				else
				{
					buffer[letters]='@';/*and the tree takes @*/
			//		printf("root value is %c\n",root->character);
					Tree_Link_Node(root,buffer,CHILD,N,1,path);/*put char "@" at end of each word*/
				}
					for (i=0;i<50;i++)
					{
						buffer[i]='\0';
					}
					for (i=0;i<50;i++)
					{
						ubuffer[i]='\0';
					}
					j=0;
			}
			else
			{	//printf("%d\n",j);
				buffer[j]=next;
				if(upper!=0)
					ubuffer[j]=upper;
				else
					ubuffer[j]=next;
				j++;
				if(j==1)/*for the first letter of every word*/
				{
					root=Tree_Return_Root(lista,next);
				}
				Tree_Link_Node(root,buffer,CHILD,N,1,path);
		//		printf("%s \n",buffer);
				flag2=1;
			}

		}
        /* Treat special characters differently */
        switch(next) {

        case '\t':              /* Just read a tab */
 		//	List_Print(list);/*print the stuff user have typed*/
			sugg_abc=List_Creation();/*make a list for the suggestions*/
			Tree_Search_Node(root,buffer,N,&sugg_abc);
			if((sugg_abc->komboi)!=1)/*don't print the suggestions when only one word'*/
				List_Print(sugg_abc,1);
			if((sugg_abc->komboi)==0)
			{	printf("\nnothing was found continue writing\n");
				List_Print(list,0);
				if(upper!=0)
					printf("%s",ubuffer);
				else
					printf("%s",buffer);
				break;
			}
			else if((sugg_abc->komboi)==1)/*if only one suggestion complete without asking*/
			{
					lnode_tempor=sugg_abc->head;
//					printf("auto-> %s\n",lnode_tempor->word);
/*--------------->*/List_2_Insert(&list,lnode_tempor->word,0);/*put the complete word in templist*/
					letters=strlen(lnode_tempor->word);
					strcpy(buffer,lnode_tempor->word);
					buffer[letters]='@';/*add the '@' in the end of the word*/
					Tree_Link_Node(root,buffer,CHILD,N,1,path);/*insert the word in the tree*/

					List_Print(list,0);/*print the stuff user have typed*/

					for(i=0;i<50;i++)/*initialize the buffer for the next word*/
						buffer[i]='\0';
					for(i=0;i<50;i++)/*initialize the upper-buffer for the next word*/
						ubuffer[i]='\0';
					j=0;/*buffer 's counter init to zero'*/
			}
			else
			{
			printf("\nuse numbers [1...%d] to choose an option :",sugg_abc->komboi);
			answer=getchar_silent();
			if(isdigit(answer))
			{
				printf("%d\n",answer-48);
				if(( answer-48>sugg_abc->komboi) || (answer-48==0))
				{
					printf("\nthe number you gave doesn't exist in the options\n");
					List_Print(list,0);
					if(upper!=0)
						printf("%s",ubuffer);
					else
						printf("%s",buffer);
					break;
				}
				else
				{	lnode_tempor=sugg_abc->head;
					for(k=1;k<answer-48;k++)
						lnode_tempor=lnode_tempor->next;
//					printf("diale3es ton %s\n",lnode_tempor->word);

/*--------------->*/List_2_Insert(&list,lnode_tempor->word,0);/*put the complete word in templist*/
					letters=strlen(lnode_tempor->word);
					strcpy(buffer,lnode_tempor->word);
					buffer[letters]='@';/*add the '@' in the end of the word*/
					Tree_Link_Node(root,buffer,CHILD,N,1,path);/*insert the word in the tree*/

					List_Print(list,0);/*print the stuff user have typed*/

					for(i=0;i<50;i++)/*initialize the buffer for the next word*/
						buffer[i]='\0';
					for(i=0;i<50;i++)/*initialize the upper-buffer for the next word*/
						ubuffer[i]='\0';

					j=0;/*buffer 's counter init to zero'*/

				}
			}
			}
			List_2_Delete(&sugg_abc);/*remove the list,to be initialized for next word*/
            break;

	     case VEOF:              /* Just read EOF (Ctrl-D) */
            printf("\n\nExiting. Bye...");
            goto THE_END;
        default:
            continue;
        }
    }
THE_END:
    putchar('\n');
	List_Delete(&list);
}






/////////////////////////// D E L E T E ////////////////////////////////////////
int SIZE;
SIZE=lista->komboi;
ptr_kombou_listas temp_node;
temp_node=lista->tail;
while(SIZE>0)
{
	root=Tree_Return_Root(lista,temp_node->character);
//	printf("main node %c\n",root->character);
	Tree_Find_Delete(root,CHILD,N);
//	Tree_Delete_Node(root,CHILD,N,0);
	temp_node=temp_node->next;
	SIZE--;
}
////////////////// L A S T  T H I N G  T O  B E  D E L E T E D /////////////////
List_Delete(&lista);
return 0;
}
