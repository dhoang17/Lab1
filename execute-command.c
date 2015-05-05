// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <fcntl.h>
//Added directives
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>

//TODO: i/o cases
int
command_status (command_t c)
{
  return c->status;
}

void execute_and(command_t c)
{
  pid_t p = fork();

  if (p < 0)
    {
      error(1,0,"fork error.");
    }

  //Child process
  if (p == 0)
    {
      //Execute command on the left
      execute_command(c->u.command[0], false);

      //Set status of internal comand
      _exit(WEXITSTATUS(c->u.command[0]->status));
     
    }

  //Parent process, need to wait for child's completion
  else
    {

      int status;
      waitpid(p, &status, 0);
      int exit_status = WEXITSTATUS(status);

      
      //If left command fails, no need to check right
      if (exit_status != 0)
	{
	  c->status = WEXITSTATUS(c->u.command[0]->status);
	}

      //Otherwise, need to execute right command
      else
	{
	 	  execute_command(c->u.command[1], false);
		  //_exit(c->u.command[1]->status);
	      c->status = WEXITSTATUS(c->u.command[1]->status);
	 }	
    }
}

void execute_sequence(command_t c)
{
  pid_t p = fork();

  if ( p < 0)
    {
      error(1,0,"fork error.");
    }

  //If in child process, execute child -> executes command on left
  if (p == 0)
    {
      //Execute command on the left
      execute_command(c->u.command[0], false);

      //Set status of internal command
      _exit(c->u.command[0]->status);
    }

  //This block is executed by parent process, which
  //executes the command on right
  else
    {
       
      int status;
      waitpid(p, &status, 0);
      int exit_status = WEXITSTATUS(status);

       execute_command(c->u.command[1], false);
       _exit(c->u.command[1]->status);
       c->status =WEXITSTATUS( c->u.command[1]->status);
    }
}

void execute_or(command_t c)
{
  pid_t p = fork();

  if (p < 0)
    {
      error(1,0,"fork error.");
    }

  //Child process
  if (p == 0)
    {
      //Execute command on the left
      execute_command(c->u.command[0], false);

      //Set status of internal comand
      _exit(WEXITSTATUS(c->u.command[0]->status));

    }
  //Parent process, need to wait for child's completion
  else
    {

      int status;
      waitpid(p, &status, 0);
      int exit_status = WEXITSTATUS(status);


      //If left command fails, no need to check right
      if (exit_status == 0)
	{
	  c->status = WEXITSTATUS(c->u.command[0]->status);
	}

      //Otherwise, need to execute right command
      else
	{
	  execute_command(c->u.command[1], false);
	  _exit(c->u.command[1]->status);
	  c->status = WEXITSTATUS(c->u.command[1]->status);
	}
    }
}

void execute_pipe(command_t c)
{
  int fd[2];
  if (pipe(fd)<0)
    {
      error(1, 0, "pipe fail");
    }

  pid_t first_pid =fork();
  if (first_pid<0)
    {
      error(1, 0, "fork fail");
    }

  if (first_pid == 0)
    {
      close(fd[1]);
      if (dup2(fd[0],0) < 0)
	{
	  error(1, 0, "dup2 fail");
	}
      execute_command(c->u.command[1], false);
      _exit(c->u.command[1]->status);
    }
  else
    {
      
      pid_t second_pid = fork();
      if (second_pid < 0)
	{
	  error(1, 0, "fork fail");
	}

      if (second_pid == 0)
	{
	  close(fd[0]);
	  if (dup2(fd[1], 1) < 0)
	    {
	      error (1, 0, "dup2 fail");
	      
	    }
	  execute_command(c->u.command[0],false);
	  _exit(c->u.command[0]->status);
	}
      
      else
	{
	  
	  close(fd[0]);
	  close(fd[1]);
	  int status;
	  pid_t return_pid = waitpid(-1, &status, 0);

	  if (second_pid == return_pid)
	    {
	      waitpid(first_pid, &status, 0);
	    }
	  if (first_pid == return_pid)
	    {
	      waitpid(second_pid, &status, 0);
	    }
	  c->status = WEXITSTATUS(status);
	}
      
      
    }
}

void execute_simple(command_t c)
{
  
  pid_t p = fork();

  if (p < 0 )
    {
      error(1,0, "Fork error.");
    }

  if (p == 0)
    {
      	
    	if (c->input != NULL)
		{
		
			int fd = open( c->input , O_RDONLY, 0644); 
			 
			if(fd < 0)
			{
				error(1, 0, "file i/o error"); 
			}

			dup2(fd, STDIN_FILENO);  	
		}
		 if(c->output != NULL)
		 {
		 	int fd = open (c->output , O_CREAT|O_TRUNC|O_WRONLY, 0644); 
		 	if(fd < 0)
		 	{
		 		error(1,0,"file i/o error"); 
		 	}

		 	dup2(fd, STDOUT_FILENO); 
		 }

      	if ( strcmp(c->u.word[0], "exec") == 0)
		{
	  		execvp(c->u.word[1],&c->u.word[1]);
		}
      	else
		{
	  		execvp(c->u.word[0],c->u.word);
		}

    }

  else
    {
      int status;
      waitpid(p, &status, 0);
      c->status = WEXITSTATUS(status);
    }
}

//Char**'s intersect
bool intersect(char** a, char** b)
{
    if ( a == NULL || b == NULL)
    {
        return false;
    }
    
    int i = 0;
    int j = 0;
    
    while ( (a+i)!= NULL)
    {
        j = 0;
        
        while ( (b+j) != NULL)
        {
            if ( !strcmp(a+i, b+j))
            {
                return true;
            }
            j++;
        }
        i++;
    }
    
    return false;
    
}

//Appends string to array of strings (add a to b)
void append(char * a, char *** b)
{
    
    //Get number of elements of an array of strings (#words)
    int list_size = 0;
    
    while (b[0][list_size]!= NULL)
    {
        list_size++;
    }
    
    //Get number of characters in the word
    int word_size = strlen(a);
    
    if (!list_size)
    {
        *b = (char**)malloc(sizeof(char*));
    }
    
    //Size the string array to accept one more word
    *b = (char**)realloc(*b, sizeof(char*) * list_size);
    
    //Allocate that new word ptr to accomodate the amount of letters in the word to add
    b[0][list_size] = (char*)malloc(sizeof(char) * (word_size));
    
    //Copy over the word into the new allocated memory space
    int i = 0;
    
    while (i < word_size)
    {
        b[0][list_size][i] = a[i];
        i++;
    }
}

//Extracting Dependencies
void extract(command_t command, char ** read_list, char ** write_list)
{
    if (command == NULL)
    {
        return;
    }
    
    else if (command->type == SIMPLE_COMMAND)
    {
        //extract input into readlist
        if (command->input != NULL)
        {
            append(command->input, &read_list);
        }
        
        if (command->u.word != NULL)
        {
            int traverse = 1;
            while (command->u.word[traverse] != NULL)
            {
                append(command->u.word[traverse], &read_list);
            }
        }
        //extract ouput into writelist
        if (command->output != NULL)
        {
            append(command->output, &write_list);
        }
    }
    
    else if (command->type == SUBSHELL_COMMAND)
    {
        extract(command->u.subshell_command);
    }
    
    else
    {
        extract(command->u.command[0]);
        extract(command->u.command[1]);
    }
    
}

//TODO: Figure out how to add2before list
//TODO: Figure out QUEUE stuff


//Graph_Node Structure
//One per tree in command stream
typedef struct graph_node *graph_node_t;
struct graph_node
{
    graph_node_t *before_list;   //Denotes all graph nodes that must come before it
    pid_t pid;                  //Denotes which forked thread handles this node
    command_t command;          //Pointer to head of command tree
};

//Constructor for graph_node
graph_node_t create_graph_node(command_t s)
{
    graph_node_t return_val = (graph_node_t)malloc(sizeof(graph_node));
    return_val->command = s;
    
    return return_val;
}

//Add to b4list
void add2b4list(graph_node_t toAdd, graph_node_t receive)
{
    //Get number of elements in receive's beforelist
    int list_size = 0;
    while (receive->before_list[list_size] != NULL)
    {
        list_size++;
    }
    
    if (!list_size)
    {
        receive->before_list = (graph_node_t*)malloc(sizeof(graph_node_t));
    }
    
    //Size the before_list to accept one more graph_node
    receive->before_list = (graph_node_t*)malloc(sizeof(graph_node_t)*list_size);
    
}

//Linked List Node Structure
//One per tree in command stream ->
typedef struct ll_node *ll_node_t;
struct ll_node
{
    graph_node_t graph_node;
    char** read_list;
    char** write_list;
    ll_node_t next;
};

//Constructor for ll_node
//Input is root of command tree
ll_node_t create_ll_node(command_node_t s)
{
    ll_node_t return_val = (ll_node_t)malloc(sizeof(ll_node));
    return_val->next = (ll_node_t)malloc(sizeof(ll_node));
    
    return_val->graph_node = (graph_node_t)malloc(sizeof(graph_node));
    return_val->graph_node = create_graph_node(s->root);
    
    extract(s->root, return_val->read_list, return_val->write_list);
    
    return return_val;
}



//TODO: Implement Queue Data structure
typedef struct QNodeType
{
    ll_node_t    ch;
    struct QNodeType *next;
}QNode;
// List Function Prototypes
void InitQueue();                // Initialize the queue
void ClearQueue();               // Remove all items from the queue
ll_node_t Enqueue(ll_node_t ch);            // Enter an item in the queue
char Dequeue();                  // Remove an item from the queue
int isEmpty();                   // Return true if queue is empty
int isFull();                    // Return true if queue is full

//--------------------------------------------
// Function: InitQueue()
// Purpose: Initialize queue to empty.
// Returns: void
//--------------------------------------------
void InitQueue()
{
    head = tail = NULL;
}

//--------------------------------------------
// Function: ClearQueue()
// Purpose: Remove all items from the queue
// Returns: void
//--------------------------------------------
void ClearQueue()
{
    QNode *temp;
    while(head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
    
    head = tail = NULL; // Reset indices to start over
}

//--------------------------------------------
// Function: Enqueue()
// Purpose: Enqueue an item into the queue.
// Returns: TRUE if enqueue was successful
//        or FALSE if the enqueue failed.
//--------------------------------------------
int Enqueue(ll_node_t ch)
{
    QNode *temp;
    
    // Check to see if the Queue is full
    if(isFull()) return FALSE;
    
    // Create new node for the queue
    temp = (QNode *)malloc(sizeof(QNode));
    temp->ch = ch;
    temp->next = NULL;
    
    // Check for inserting first in the queue
    if(head == NULL)
        head = tail = temp;
    else
    {
        tail->next = temp; // Insert into the queue
        tail = temp;       // Set tail to new last node
    }
    
    return TRUE;
}

//--------------------------------------------
// Function: Dequeue()
// Purpose: Dequeue an item from the Queue.
// Returns: TRUE if dequeue was successful
//        or FALSE if the dequeue failed.
//--------------------------------------------
ll_node_t Dequeue()
{
    ll_node_t ch;
    QNode *temp;
    
    // Check for empty Queue
    if(isEmpty()) return '\0';  // Return null character if queue is empty
    else
    {
        ch = head->ch;        // Get character to return
        temp = head;
        head = head->next;    // Advance head pointer
        free(temp);            // Free old head
        // Check to see if queue is empty
        if(isEmpty())
        {
            head = tail = NULL;        // Reset everything to empty queue
        }
    }
    return ch;                // Return popped character
}

//--------------------------------------------
// Function: isEmpty()
// Purpose: Return true if the queue is empty
// Returns: TRUE if empty, otherwise FALSE
// Note: C has no boolean data type so we use
//    the defined int values for TRUE and FALSE
//    instead.
//--------------------------------------------
int isEmpty()
{
    return (head == NULL);
}

//--------------------------------------------
// Function: isFull()
// Purpose: Return true if the queue is full.
// Returns: TRUE if full, otherwise FALSE
//--------------------------------------------
int isFull()
{
    return FALSE;
}


/*----END OF QUEUE DECLARATION - http://www.cs.uah.edu/~rcoleman/Common/CodeVault/Code/Code136_Queue.html */

//Dependency Structure
typedef struct dependency_graph *dependency_struct_t;
struct dependency_struct
{
    QNode no_dependency;
    QNode dependency;
};


//Create graph structure with dependencies/intersections calculated
dependency_graph create_graph(command_stream_t s)
{
    dependency_graph_t return_val= (dependency_graph_t)malloc(sizeof(dependency_graph));
    
    head = (ll_node_t)malloc(sizeof(ll_node));
   head = NULL;
    
    //For each node in the command stream...
    command_node_t traverse = s->head;
    while (traverse != s->tail)
    {
        //construct a new ll_node
        ll_node_t temp = (ll_node_t)malloc(sizeof(ll_node));
        temp = create_ll_node(traverse);
        
        //insert it into head of linked list
        
        //For first list item
        temp->next = return_val->head;

        head = temp;
        //For each ll_node after head in linked list...
        ll_node_t traverse = (ll_node_t)malloc(sizeof(ll_node));
        traverse2 = return_val->head->next;
        
        while (traverse2 != NULL)
        {
            //Take intersection between head and ll_node
            bool RAW = intersect(traverse2->read_list, return_val->head->write_list);
            bool WAR = intersect(traverse2->write_list, return_val->head->read_list);
            bool WAW = intersect(traverse2->write_list, return_val->head->write_list);
            
            //If there's a dependency, add node to the beforelist of head
            if (RAW || WAR || WAW)
            {
                add2b4list(traverse2, return_val->head);
            }
            
            //If head->before not null, add to dependencies, otherwise add to no_dependencies
            if (return_val->head->graph_node->before_list != NULL)
            {
               //enqueue dep
            }
            
            else
            {
                //enque nodep
            }
            traverse2 = traverse2->next;
        }
        traverse = traverse->next;
    }
 
    return return_val;
}

void
execute_command (command_t c, bool time_travel)
{
  switch (c->type)
  {
  case AND_COMMAND:
    execute_and(c);
    break;
  case SEQUENCE_COMMAND:
    execute_sequence(c);
    break;
  case OR_COMMAND:
    execute_or(c);
    break;
  case PIPE_COMMAND:
    execute_pipe(c);
    break;
  case SIMPLE_COMMAND:
    execute_simple(c);
    break;
  case SUBSHELL_COMMAND:
  	if(c->input != NULL)
  	{
  		c->u.subshell_command->input = c->input; 
  	}
  	if(c->output != NULL)
  	{
  		c->u.subshell_command->output = c->output; 
  	}
    execute_command(c->u.subshell_command, false);
    break;
  default:
    break;
  }

    
    
    
    
}
