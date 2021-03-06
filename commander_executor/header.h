////////////////////////////////////////////////////////////////////////////////
///////////////// G L O B A L  V A R I A B L E S ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

char buff[64];/*the command that comes through the pipe to handler*/

int gl_exit_flag;/*flag that terminates server's while(1) loop */

int gl_concurrency;/*concurrency variable*/

int gl_temp_conc;/*temporary concurrency variable*/

int gl_temp_reminder;/*2nd temporary concurrency variable*/

int gl_random_id;/*a random number id variable*/

////////////////////////////////////////////////////////////////////////////////
void child_handler(int);/*SIGCHLD handle*/
////////////////////////////////////////////////////////////////////////////////
void handler(int);/*SIGTERM handler*/
////////////////////////////////////////////////////////////////////////////////
void exit_handler(int);/*SIGUSR1 handler*/
////////////////////////////////////////////////////////////////////////////////
void Executor(char * ,int ,int);
////////////////////////////////////////////////////////////////////////////////
/////////////////////////// L I S T ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Kombos *ptr_kombou_listas;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Info *ptr_listas;
////////////////////////////////////////////////////////////////////////////////
typedef struct Lista_Kombos
{	
	char word[64];/*the exact string from pipe*/
	int jobid;/*jobid*/
	int random;/*given job number*/
	char job[32];/*job name*/
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
}Lista_Info;
////////////////////////////////////////////////////////////////////////////////

ptr_listas list_run;/*a global list for running proceses*/
	
ptr_listas list_queue;/*a global list for queued proceses*/

////////////////////////////////////////////////////////////////////////////////
/////////////////// L I S T  F U N C T I O N S /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ptr_listas List_Creation();
////////////////////////////////////////////////////////////////////////////////
void List_Delete(ptr_listas * );
////////////////////////////////////////////////////////////////////////////////
int List_Keni(ptr_listas);
////////////////////////////////////////////////////////////////////////////////
void List_Print(ptr_listas);
////////////////////////////////////////////////////////////////////////////////
void List_Insert(ptr_listas *,char *,int ,int ,char *);
////////////////////////////////////////////////////////////////////////////////
void List_Remove(ptr_listas * ,int,int);
////////////////////////////////////////////////////////////////////////////////
int List_Search(ptr_listas ,int ,int);
////////////////////////////////////////////////////////////////////////////////
void List_Execute_Head(ptr_listas * );






