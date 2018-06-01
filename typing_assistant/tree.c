#include<stdio.h>
#include"header.h"
#include<stdlib.h>
#include<string.h>
////////////////////////////////////////////////////////////////////////////////
//////////////////// D I M I O U R G I A  T R E E //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_tree Tree_Node_Creation(int child,int N)
{		int i;
		ptr_tree tinfo;
		tinfo =(Tree_Node *) malloc(sizeof(Tree_Node));
		if ( tinfo == NULL )
		{   printf("ERROR : Could not allocate memory!\n");
			return;
		}
		tinfo->counter=0;
		tinfo->suggestions=(ptr_tree *)malloc(N*sizeof(ptr_tree));
		if ( tinfo->suggestions == NULL )
		{   printf("ERROR : Could not allocate memory!\n");
			return;
		}
		for (i=0; i<N; i++)
			tinfo->suggestions[i] = NULL;

		tinfo->children=(ptr_tree *)malloc(child*sizeof(ptr_tree));
		if ( tinfo->children == NULL )
		{   printf("ERROR : Could not allocate memory!\n");
			return;
		}
		for (i=0; i<child; i++)
			tinfo->children[i] = NULL;
		tinfo->parent=NULL;
		tinfo->active_children=0;
		return tinfo;
}
////////////////////////////////////////////////////////////////////////////////
////////////////// L I N K  L I S T --> T R E E ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_tree Tree_Link_Root(ptr_listas linfo,ptr_tree tinfo,char stoixeio)
{
	ptr_kombou_listas list_temp;
	list_temp=(linfo)->tail;
//	printf("to tail stoixeio einai :%c\n",list_temp->character);
////////// F I N D  T H E  C H A R A C T E R  I N  T H E  L I S T //////////////
	while(list_temp->character!=stoixeio)/*while character not found*/
	{
		list_temp=list_temp->next;/*move to next list node*/
	//	printf("skalwnei\n");
	}
/////////// A N D  A T T A C H  T H E  T R E E  T O  T H E  L I S T ////////////
	if(list_temp->character==stoixeio)
	{
//		printf("Bre8hke !!!\n");
		list_temp->tree=tinfo;/*h lista na deixnei sto dentro*/
		tinfo->character=stoixeio;
//		printf("eishx8ei sto dentro to %c ",list_temp->tree->character);
	}
	return list_temp->tree;	//or tinfo;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////// R E T U R N  R O O T ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_tree Tree_Return_Root(ptr_listas linfo,char stoixeio)
{
	ptr_kombou_listas list_temp;
	list_temp=(linfo)->tail;
////////// F I N D  T H E  C H A R A C T E R  I N  T H E  L I S T //////////////
	while(list_temp->character!=stoixeio)/*while character not found*/
	{
		list_temp=list_temp->next;/*move to next list node*/
	}
/////////// A N D  A T T A C H  T H E  T R E E  T O  T H E  L I S T ////////////
	if(list_temp->character==stoixeio)
	{
		return list_temp->tree;	//or tinfo;
	}

}
////////////////////////////////////////////////////////////////////////////////
////////////////////// D E L E T E  N O D E ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Tree_Delete_Node(ptr_tree node,int child,int N)
{
	if(node==NULL)
		return;
//	printf("deleting %c \n",node->character);
	free((node->suggestions));
	node->suggestions=NULL;
	free(node->children);
	node->children=NULL;
	free(node);
	node=NULL;

}
////////////////////////////////////////////////////////////////////////////////
////////////// F I N D  N O D E  T O  B E  D E L E T E D ///////////////////////
////////////////////////////////////////////////////////////////////////////////
void Tree_Find_Delete(ptr_tree root,int CHILD,int N)
{	int i;
	ptr_tree temp2;
	ptr_tree temp = root;
if(temp!=NULL)
	if(temp->character=='@')
	{	//printf("1kanei to %c delete\n",temp->character);
		temp2=temp;
		Tree_Delete_Node(temp2,CHILD,N);
		return;
	}
if(temp!=NULL)
	if(temp->active_children==0)
	{	temp2=temp;
	//	printf("2kanei to %c delete\n",temp->character);
	//	Tree_Delete_Node(temp2,CHILD,N);
		return;
	}
	for(i=0;i<temp->active_children;i++)
	{//	printf("------->going to %c \n",temp->children[i]->character);
		Tree_Find_Delete(temp->children[i],CHILD,N);
		if(temp!=NULL)
		if(i==temp->active_children-1)
		{	//printf("3kanei to %c delete\n",temp->character);
			temp2=temp;
			Tree_Delete_Node(temp2,CHILD,N);
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////// E I S A G W G H  T R E E  ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Tree_Link_Node(ptr_tree tinfo,char *word,int CHILD,int N,int flag,char *path)
{
FILE *fp;
if(flag==1)
{
	fp=fopen(path,"a");
}
	ptr_tree tree_temp,new;
	tree_temp=tinfo;
	int letters,i,j,found=0,flag_doubles=0;
	char write_to_file[50];
	for (i=0;i<50;i++)
		write_to_file[i]='\0';

	letters=strlen(word);
	for(i=0;i<letters;i++)/*this loop is representing each word we insert*/
	{	found=0;
		if(tree_temp->character==word[i] && flag_doubles==0)/*for words with double characters like triccs*/
		{
			found=1;
			flag_doubles=1;
		}
		else
		{
			for(j=0;j<tree_temp->active_children;j++)/*and this loop represents each word in the tree*/
			{
				if(tree_temp->children[j]->character==word[i])
				{/*if the previous character is the same with father*/
					if(word[i-1]==tree_temp->character)
					{	/*go to the child*/
						tree_temp=tree_temp->children[j];
						found=1;
					}
				}

			}
		}
		if(found==0)/*edited 14/5 @17:41*/
			break;
	}
	if (found==0)
	{	//printf("mphke gia ton %s me root %c\n",word,tinfo->character);
		new=Tree_Node_Creation(CHILD,N);
//		printf("Dimiourgw node me patera %c \n",tree_temp->character);
		tree_temp->children[tree_temp->active_children]=new;
//		printf("desmeuw to child{%d} \n",tree_temp->active_children);
		tree_temp->active_children++;
//		printf("au3anw to active_children\n");
		new->parent=tree_temp;
		new->character=word[letters-1];
//		printf("o parent tou %c einai o %c \n",new->character,new->parent->character);
		if(word[letters-1]=='@')/*for every new node that was not found*/
		{	//printf("kalw thn refresh me kombo ton %c kai word: %s \n",new->character,word);
			Tree_Refresh(new,N);
			strcpy(write_to_file,word);
			letters=strlen(write_to_file);
			write_to_file[letters-1]='\0';
			if (flag==1)
			{
				//printf("insert %s to file\n",write_to_file);
				fprintf(fp,"%s\n",write_to_file);
			}
		}
	}
	else if(word[letters-1]=='@' )/*for every complete word that is shown again*/
	{
//		printf("found the same word :%s \n",word);
//		printf("++++++++++++ counter on %c node ",tree_temp->parent->character);
		tree_temp->counter++;
//		printf("%d \n",tree_temp->counter);
//		printf("DO REFRESH! \n");
		Tree_Refresh(tree_temp,N);
	}
if(flag==1)
	fclose(fp);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////// R E F R E S H   T R E E   I N F O S ////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Tree_Refresh(ptr_tree leaf,int N)
{
	int i, saved=0;

	ptr_tree node = leaf;
	while(node->parent!=NULL)/*until reach the root*/
	{
		node=node->parent;
		saved=0;
		for(i=0;i<N;i++)/*search for available suggestion pointer*/
		{	if(node->suggestions[i]==leaf)
			{	//printf("already showing that leaf\n");
				break;
			}
			if(node->suggestions[i]==NULL)
			/*if there is a free pointer on the father*/
			{
				node->suggestions[i]=leaf;
		//		printf("node %c is pointing to leaf %c with counter %d \n",node->character,leaf->character,leaf->counter);
				saved=1;
				break;
			}
		}
		if(saved==0)/*if not found search for a leaf with smaller frequent number*/
			for(i=0;i<N;i++)/*search in suggestion pointer*/
			{				/*if there is a suggestion with smaller counter */
				if(node->suggestions[i]->counter<=leaf->counter)
				{			/*forget it and show to our new leaf*/
					node->suggestions[i]=leaf;
					//printf("2 | node %c is pointing to leaf %c with counter %d \n",node->character,leaf->character,leaf->counter);
					saved=1;
					break;
				}
			}
	}/*do it one more time for the root node*/
		saved=0;
		for(i=0;i<N;i++)/*search for available suggestion pointer*/
		{	if(node->suggestions[i]==leaf)
			{	//printf("already showing that leaf\n");
				break;
			}
			if(node->suggestions[i]==NULL)
			/*if there is a free pointer on the father*/
			{
				node->suggestions[i]=leaf;
		//		printf("node %c is pointing to leaf %c with counter %d \n",node->character,leaf->character,leaf->counter);
				saved=1;
				break;
			}
		}
		if(saved==0)/*if not found search for a leaf with smaller frequent number*/
			for(i=0;i<N;i++)/*search in suggestion pointer*/
			{				/*if there is a suggestion with smaller counter */
				if(node->suggestions[i]->counter<=leaf->counter)
				{			/*forget it and show to our new leaf*/
					node->suggestions[i]=leaf;
					//printf("2 | node %c is pointing to leaf %c with counter %d \n",node->character,leaf->character,leaf->counter);
					saved=1;
					break;
				}
			}

}
////////////////////////////////////////////////////////////////////////////////
////// S E A R C H  F O R  C O M M O N  W O R D S  I N  T H E  T R E E  ////////
////////////////////////////////////////////////////////////////////////////////
void Tree_Search_Node(ptr_tree tinfo,char *word,int N,ptr_listas * sugg_abc)
{
	char buf[50],tempbuff[50];
	ptr_tree tree_temp,new;
	tree_temp=tinfo;
	int letters,i,j,k,t,found=0;
	int flag_doubles=0;/*for words with double characters like triccs*/

	for(i=0;i<50;i++)/*initialize buff value*/
		buf[i]='\0';

	letters=strlen(word);
	for(i=0;i<letters;i++)/*this loop is representing each word we insert*/
	{	found=0;
		if(tree_temp->character==word[i] && flag_doubles==0)
		{
			found=1;
			flag_doubles=1;
		}
		else
		{
			for(j=0;j<tree_temp->active_children;j++)/*and this loop represents each word in the tree*/
			{
				if(tree_temp->children[j]->character==word[i])
				{/*if the previous character is the same with father*/
					if(word[i-1]==tree_temp->character)
					{	/*go to the child*/
						tree_temp=tree_temp->children[j];
						found=1;
					}
				}

			}
		}
		if(found==0)/*edited 14/5 @17:41*/
		{// 	printf("\nnothing was found\n");
			break;
		}
		else if(i==letters-1)
		{
//			printf("the last node i reached is %c so im going to check the N pointers!\n",tree_temp->character);
			for(k=0;k<N;k++)
			{//	printf("going to suggestion %d\n",k);
				for(t=0;t<50;t++)
					buf[t]='\0';
				for(t=0;t<50;t++)
					tempbuff[t]='\0';
				t=0;
				if(tree_temp->suggestions[k]!=NULL)
					new=tree_temp->suggestions[k];
				else
					break;
				while(new->parent!=NULL)
				{
					new=new->parent;
					if(new->character!='@')
					{
						buf[t]=new->character;
						t++;
					}
				}
////////////////////// R E V E R S E  T H E  W O R D ///////////////////////////
				letters=strlen(buf);
				for(t=letters-1;t>=0;t--)
				{//	printf("i: %d letters-i : %d \n",t,letters-1-t);
					tempbuff[t]=buf[letters-1-t];
				}
				//printf("reversed ! %s \n",tempbuff);
				List_2_Insert(sugg_abc,tempbuff,1);/*change the value 0->1 for sorted suggestions*/
			}
			//printf("that 's all i found\n");

		}
	}
}

