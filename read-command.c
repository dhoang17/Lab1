// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include <ctype.h>
#include "command-internals.h"
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */



///////////BEGIN STACK DEFINITION////////////////

#define STACK_SIZE 100

//start operator stack    
//typedef struct operator_stack *operator_stack_t;

typedef struct
{
	char* data[STACK_SIZE];
	int top;
}operator_stack_t;
//typedef struct operator_stack operator_stack_t; 

void op_init(operator_stack_t *stack)
{
	stack->top = 0; 
}


char* op_pop(operator_stack_t *stack)
{
	char* return_val; 
	if (stack->top == 0)
	{
		return NULL;
	}
	else
	{
		return_val = stack->data[stack->top -1];
		stack->data[stack->top - 1] = NULL;  
		(stack->top)--; 
	}
	return return_val; 
}

int op_push(operator_stack_t* stack, char* val)
{
	if(stack->top == STACK_SIZE)
	{
		//printf "Stack full";
		return 0; 
	}
	else
	{
		stack->data[stack->top] = val; 
		(stack->top)++;
	}
	return 1; 
}

int op_empty(operator_stack_t* stack)
{
	if(stack->top == 0)
	{
		return 1; 
	}
	return 0; 
}
//end operator stack

//start command stack
 
typedef struct 
{
	command_t* data[STACK_SIZE];
	int top;
}command_stack_t;

void com_init(command_stack_t* stack)
{
	stack->top = 0; 
}

command_t* com_pop(command_stack_t* stack)
{
	command_t* return_val; 
	if (stack->top == 0)
	{
		return NULL;
	}
	else
	{
		return_val = stack->data[stack->top -1];
		stack->data[stack->top - 1] = NULL;  
		(stack->top)--; 
	}
	return return_val; 
}

int com_push(command_stack_t* stack, command_t* val)
{
	if(stack->top == STACK_SIZE)
	{
		//printf "Stack full";
		return 0; 
	}
	else
	{
		stack->data[stack->top] = val; 
		(stack->top)++;
	}
	return 1; 
}

int com_empty(command_stack_t* stack)
{
	if(stack->top == 0){
		return 1;
	}
	return 0; 
}
//end command stack

//////////END STACK DEFINITION////////////
//Reference: groups.csail.mit.edu/graphics/classes/6.837/F04/cpp_notes/stack1.html



typedef enum parser_component *parser_component_t;

enum parser_component
{
  WORD,
  SEMICOLON,
  ANDOR,
  PIPE,
  POUND,
  NEWLINE,
  INPUT, //>
  OUTPUT, //<
  ERROR
};


typedef struct command_node *command_node_t;

//command tree
struct command_node
{
  command_t* root;
  command_node_t* next;
};

//linked list of command nodes
struct command_stream
{
  command_node_t *head;
  command_node_t *tail;
  command_node_t *cursor;
};
  
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
    	     void *get_next_byte_argument)
{
  //Make the buffer
  char *a = malloc(sizeof(char) * 1000);
  int size = 0;
  char c;

    do
      {
	c = get_next_byte(get_next_byte_argument);
	a[size] = c;
	size++;
      }
    while (c != EOF);

  //Construct the command nodes
    int index = 0;
    parser_component_t *enumerated_array = malloc(sizeof(parser_component_t)*size);
    int i = 0;
    //convert buffer into enumerated buffer
    while (i < size) 
      {

	if (i ==0)
	  {
	    if (( strcmp(a[i],'|')==0 && strcmp(a[i+1], '|') == 0) ||(strcmp(a[i],'&')==0 && strcmp(a[i+1], '&') == 0)) 
	      {
		enumerated_array[index] = ANDOR;
		i += 2;
		index++;
	      }

	      if (strcmp(a[i], '|' == 0) && strcmp(a[i+1], '|')!=0)
	      {
		enumerated_array[index] = PIPE;
		i++;
		index++;
	      }
	  }

	if (i == size)
	 {
	   if ((strcmp(a[i],'|')==0 && strcmp(a[i-1], '|') ==0) ||(strcmp(a[i],'&')==0 && strcmp(a[i-1], '&') == 0))
	  {
	    enumerated_array[index] = ANDOR;
	    i += 2;
	    index++;
	  }

	  if (strcmp(a[i], '|' == 0) && strcmp(a[i-1], '|')!=0)
	    {
	      enumerated_array[index] = PIPE;
	      i++;
	      index++;
	    }
	 }

	if(i != 0 && i != size)
	  {
	    if (  ( strcmp(a[i],'&')==0 && strcmp(a[i+1],'&')==0 ) || (strcmp(a[i], '|') == 0 && strcmp(a[i+1], '|')==0) )
	    {
	      enumerated_array[index] = ANDOR;
	      i+=2;
	      index++;
	    }
	    else if ( strcmp(a[i], '|') == 0 && strcmp(a[i-1], '|') != 0 && strcmp(a[i+1],'|') != 0 )
	    {
	      enumerated_array[index] = PIPE;
	      i++;
	      index++;
	    }
	    else if(strcmp(a[i], ';')==0)
	    {
	    enumerated_array[index] = SEMICOLON;
	    index++;
	    i++;
	    }
	    else if(strcmp(a[i], '#')==0)
	    {
	    enumerated_array[index] = POUND;
	    index++;
	    i++;
	    }
	    else if(strcmp(a[i], '\n')==0)
	    {
	    enumerated_array[index] = NEWLINE;
	    index++;
	    i++;
	    }
	    else if(strcmp(a[i], '<') == 0)
	    {
	    enumerated_array[index] = INPUT;
	    index++;
	    i++;
	    }
	    else if(strcmp(a[i], '>') == 0)
	    {
	    enumerated_array[index] = OUTPUT;
	    index++;
	    i++;
	    }
	    else if(isalnum(a[i])==0)
	    {
	      while (!isalnum(a[i]))
		{
		  i++;
		}
		enumerated_array[index] = WORD;
		index++;
	    }	      
	  }
      }
	    
	      
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
