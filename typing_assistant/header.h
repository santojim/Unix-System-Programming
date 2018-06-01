#ifndef _LIST_
#define _LIST_

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// T R E E //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct Tree_Node *ptr_tree;
////////////////////////////////////////////////////////////////////////////////
typedef struct Tree_Node
{
	ptr_tree *suggestions;
	ptr_tree *children;
	ptr_tree parent;
	char character;
	int counter;
	int active_children;
}Tree_Node;
////////////////////////////////////////////////////////////////////////////////
/////////////////////////// L I S T ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Kombos *ptr_kombou_listas;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Info *ptr_listas;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Kombos
{	char character;
	char word[50];
	ptr_kombou_listas next;
	ptr_kombou_listas previous;
	ptr_tree tree;
}Lista_Kombos;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Info
{
	ptr_kombou_listas head;
	ptr_kombou_listas tail;
	ptr_kombou_listas current;
	int komboi;
}Lista_Info;
////////////////////////////////////////////////////////////////////////////////
/////////////////// L I S T  F U N C T I O N S//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_listas List_Creation();
////////////////////////////////////////////////////////////////////////////////
void List_Delete(ptr_listas * );
////////////////////////////////////////////////////////////////////////////////
void List_Insert(ptr_listas * ,char );
////////////////////////////////////////////////////////////////////////////////
int List_Keni(ptr_listas);
////////////////////////////////////////////////////////////////////////////////
void List_Print(ptr_listas,int);
////////////////////////////////////////////////////////////////////////////////
int List_Check(ptr_listas,char);
////////////////////////////////////////////////////////////////////////////////
void List_2_Insert(ptr_listas * ,char *,int);
////////////////////////////////////////////////////////////////////////////////
void List_Sort(ptr_listas *,ptr_kombou_listas *);
////////////////////////////////////////////////////////////////////////////////
int getchar_silent();
////////////////////////////////////////////////////////////////////////////////
void List_2_Delete(ptr_listas * );

////////////////////////////////////////////////////////////////////////////////
///////////////////T R E E  F U N C T I O N S //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_tree Tree_Return_Root(ptr_listas ,char);
////////////////////////////////////////////////////////////////////////////////
ptr_tree Tree_Node_Creation(int ,int );
////////////////////////////////////////////////////////////////////////////////
ptr_tree Tree_Link_Root(ptr_listas ,ptr_tree ,char );
////////////////////////////////////////////////////////////////////////////////
void Tree_Delete_Node(ptr_tree ,int ,int);
////////////////////////////////////////////////////////////////////////////////
void Tree_Link_Node(ptr_tree,char*,int,int,int,char *);
////////////////////////////////////////////////////////////////////////////////
void Tree_Find_Delete(ptr_tree,int,int);
////////////////////////////////////////////////////////////////////////////////
void Tree_Refresh(ptr_tree ,int );
////////////////////////////////////////////////////////////////////////////////
void Tree_Search_Node(ptr_tree ,char* ,int, ptr_listas *);
////////////////////////////////////////////////////////////////////////////////

#endif
////////////////////////////////////////////////////////////////////////////////
