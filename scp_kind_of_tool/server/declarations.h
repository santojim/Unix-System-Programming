/////////////////////// G L O B A L  V A R I A B L E S /////////////////////////
int gl_exit_flag;

int port;

pthread_mutex_t mtx;/* Mutex for synchronization */

pthread_cond_t cond;/* Condition varialbe */

////////////////////// F U N C T I O N S ///////////////////////////////////////
void perror_exit(char *);

void Read_Directory(char *,int);

int Count_Entities(char *,int*);

///////////////////// T H R E A D  F U N C T I O N S ///////////////////////////
void * Connection(void *);

void * Worker_Thread_F(void *);

////////////////////////////////////////////////////////////////////////////////
void exit_handler(int);/*SIGINT handler*/
////////////////////////////////////////////////////////////////////////////////
/////////////////////////// L I S T ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Kombos *ptr_kombou_listas;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Info *ptr_listas;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Kombos
{
	char path[512];/*path to be send*/
	int sockfd;/*for client 's socket*/
	ptr_kombou_listas next;
	ptr_kombou_listas previous;
}Lista_Kombos;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Info
{
	ptr_kombou_listas head;
	ptr_kombou_listas tail;
	ptr_kombou_listas current;
	int komboi;
	int max_nodes;
}Lista_Info;
////////////////////////////////////////////////////////////////////////////////

ptr_listas list_queue;/*a global list for queued paths*/

////////////////////////////////////////////////////////////////////////////////
/////////////////// L I S T  F U N C T I O N S /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_listas List_Creation(int);
////////////////////////////////////////////////////////////////////////////////
void List_Delete(ptr_listas * );
////////////////////////////////////////////////////////////////////////////////
void List_Insert(ptr_listas *,char *,int );
////////////////////////////////////////////////////////////////////////////////
int List_Empty(ptr_listas);
////////////////////////////////////////////////////////////////////////////////
int List_Full(ptr_listas );
////////////////////////////////////////////////////////////////////////////////
void List_Remove(ptr_listas *,char * ,int );
////////////////////////////////////////////////////////////////////////////////
void List_Print(ptr_listas );
////////////////////////////////////////////////////////////////////////////////


