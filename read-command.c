// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include <ctype.h>
#include "command-internals.h"
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>

///////////BEGIN STACK DEFINITION////////////////

#define STACK_SIZE 100

//Definition of operator stack
typedef struct operator_stack *operator_stack_t;

struct operator_stack
{
  char* data[STACK_SIZE];
  int top;
};

//Constructor
void op_init(operator_stack_t stack)
{
  int i = 0;
  while(i != STACK_SIZE)
    {
      stack->data[i] = '\0';
      i++;
    }
  stack->top = 0;
}

//Pop
char* op_pop(operator_stack_t stack)
{

  if (stack->top == 0)
    {
      return NULL;
    }
  else
    {
      char* return_val = (char*)malloc(sizeof(char));
      return_val = stack->data[stack->top -1];
      stack->data[stack->top - 1] = NULL;
      (stack->top)--;
      return return_val;
    }

}

//Push
int op_push(operator_stack_t stack, char* val)
{
  if(stack->top == STACK_SIZE)
    {
      return 0;
    }
  else
    {
      stack->data[stack->top] = (char*)malloc(sizeof(char));
      stack->data[stack->top] = val;
      (stack->top)++;
    }
  return 1;
}

//IsEmpty
int op_empty(operator_stack_t stack)
{
  if(stack->top == 0)
    {
      return 1;
    }
  return 0;
}
//end operator stack

//start command stack


//Command Stack Definition
typedef struct command_stack *command_stack_t;

struct command_stack
{
  command_t data[STACK_SIZE];
  int top;
};


//Constructor
void com_init(command_stack_t stack)
{
  int i = 0;
  while (i != STACK_SIZE)
    {
      stack->data[i] = (command_t)malloc(sizeof(struct command));
      stack->data[i] = '\0';
      i++;
    }
  stack->top = 0;
}

//Pop
command_t com_pop(command_stack_t stack)
{
  if (stack->top == 0)
    {
      return NULL;
    }
  else
    {
      command_t  return_val = (command_t)malloc(sizeof(struct command));
      return_val = stack->data[stack->top -1];
      stack->data[stack->top - 1] = NULL;
      (stack->top)--;
      return return_val;
    }

}

//Push
int com_push(command_stack_t stack, command_t val)
{
  if(stack->top == STACK_SIZE)
    {
      return 0;
    }
  else
    {
      stack->data[stack->top] = val;
      (stack->top)++;
    }
  return 1;
}

//IsEmpty
int com_empty(command_stack_t stack)
{
  if(stack->top == 0){
    return 1;
  }
  return 0;
}
//end command stack

//////////END STACK DEFINITION////////////
//Reference: groups.csail.mit.edu/graphics/classes/6.837/F04/cpp_notes/stack1.html


//Get precedence of character
int get_precedence(char* op)
{
  //|, ||/&&, ;/\n

  if(op == NULL)
    {
      return -1;
    }
  if(*op == '|')
    {
      return 3;
    }

  if(*op == '&' || *op == '{')
    {
      return 2;
    }

  if(*op == ';')
    {
      return 1;
    }

  else
    {
      return 0;
    }

}

//Create the command
//Since it's a union, you can only alter the variable that is not null
command_t create_command(enum command_type type, command_t child_1, command_t child_2, char** word, command_t subshell)
{
  command_t return_val = (command_t)malloc(sizeof(struct command));
  return_val->type = type;

  if (child_1 != NULL && child_2 != NULL)
    {
      return_val->u.command[0] = (command_t)malloc(sizeof(struct command));
      return_val->u.command[1] = (command_t)malloc(sizeof(struct command));
      return_val->u.command[0] = child_1;
      return_val->u.command[1] = child_2;
    }

  if (word != NULL)
    {
      int size = 0;
      int j = 0;

      while ( word[j] != NULL )
	{
	  size++;
	  j++;
	}

      return_val->u.word = (char**)malloc(sizeof(char*)*size);

      j = 0;
      int i = 0;
      int k = 0;
      while ( j < size)
	{
	  i = 0;
	  while (word[j][i]!= '\0')
	    {
	      i++;
	    }
	  return_val->u.word[j] = (char*)malloc(sizeof(char)*i);

	  k=0;
	  while (k <= i)
	    {
	      if (k == i)
		{
		  return_val->u.word[j][k] = '\0';
		}
	      return_val->u.word[j][k]=word[j][k];
	      k++;
	    }

	  j++;
	}
      return_val->u.word[j] = NULL;
    }

  if (subshell != NULL)
    {
      return_val->u.subshell_command = (command_t)malloc(sizeof(struct command));
      return_val->u.subshell_command = subshell;
    }

  return_val->status = -1;
  return return_val;

}

//Postfix->Infix Algorithm
void handle_stack(operator_stack_t stacko, command_stack_t stackc)
{
  if(op_empty(stacko))
    {
      return;
    }

  if(stacko->top == 1)
    {
      return;
    }

  char* cur_op = (char*)malloc(sizeof(char));
  char* next_op = (char*)malloc(sizeof(char));
  command_t cur_com = (command_t)malloc(sizeof(struct command));
  command_t new_com = (command_t)malloc(sizeof(struct command)); //= (command_t*)malloc(sizeof(command_t));
  cur_op = op_pop(stacko);

  if(*cur_op == '(')
    {
      op_push(stacko, cur_op);
      return;
    }

  next_op = op_pop(stacko);

  if(get_precedence(next_op) < get_precedence(cur_op))
    {
      op_push(stacko, next_op);
      op_push(stacko, cur_op);
      return;
    }

  if(*next_op == '(' && *cur_op == ')')
    {
      new_com =  create_command(SUBSHELL_COMMAND, NULL, NULL, NULL, com_pop(stackc));
      com_push(stackc, new_com);
    }

  command_t com_1 = (command_t)malloc(sizeof(struct command));
  command_t com_2 = (command_t)malloc(sizeof(struct command));

  while(get_precedence(next_op) >= get_precedence(cur_op) && cur_op != NULL)
    {

      com_1 = com_pop(stackc);
      com_2 = com_pop(stackc);

      if(*next_op == '(' && *cur_op == ')')
	{
	  new_com =  create_command(SUBSHELL_COMMAND, NULL, NULL, NULL, com_1);
	  com_push(stackc, new_com);
	  next_op = NULL;
	  cur_op = NULL;
	  if(com_2 != NULL)
	    {
	      com_push(stackc, com_2);
	    }
	}

      if(next_op != NULL && *next_op == '|')
	{
	  new_com = create_command(PIPE_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}
      if(next_op != NULL && *next_op == '&')
	{
	  new_com = create_command(AND_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}
      if(next_op != NULL && *next_op == '{')
	{
	  new_com = create_command(OR_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}
      if(next_op != NULL && *next_op == ';')
	{
	  new_com = create_command(SEQUENCE_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}


      next_op = op_pop(stacko);

    }
  if(next_op != NULL)
    {
      op_push(stacko, next_op);
    }
  if(cur_op != NULL)
    {
      op_push(stacko, cur_op);
    }
}

//Postfix Infix Algorithm
void finish_stack(operator_stack_t stacko, command_stack_t stackc)
{
  char* cur_op = (char*)malloc(sizeof(char));
  command_t com_1 = (command_t)malloc(sizeof(struct command));
  command_t com_2 = (command_t)malloc(sizeof(struct command));
  command_t new_com = (command_t)malloc(sizeof(struct command));

  while(!op_empty(stacko))
    {
      cur_op = op_pop(stacko);
      com_1 = com_pop(stackc);
      com_2 = com_pop(stackc);

      if(*cur_op == '|')
	{
	  new_com = create_command(PIPE_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}
      if(*cur_op == '&')
	{
	  new_com = create_command(AND_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}
      if(*cur_op == '{')
	{
	  new_com = create_command(OR_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}
      if(*cur_op == ';')
	{
	  new_com = create_command(SEQUENCE_COMMAND, com_2, com_1, NULL, NULL);
	  com_push(stackc, new_com);
	}

    }
}

//Command_node Declaration (this is a command tree
typedef struct command_node *command_node_t;

struct command_node
{
  command_t root;
  command_node_t next;
};

//Constructor
void node_init(command_node_t node)
{
  node->root = NULL;
  node->next = NULL;
}


//linked list of command nodes declaration
struct command_stream
{
  command_node_t head;
  command_node_t tail;
  command_node_t cursor;
};

//constructor
void stream_init(command_stream_t stream)
{
  stream->head = NULL;
  stream->tail = NULL;
  stream->cursor = NULL;

}

//Helper functions for error checking

bool valid_char(char c)
{
  return ( isalnum(c) || c == '!' || c == '%' || c == '+'  || c == ',' || c == '-' || c == '.'  || c == '/' || c == ':' || c == '@' || c == '^' || c == '_' || c == ' '  || c == '\t' || c == ' ' || c == '\n' || c == '#' );

}

bool valid_token(char c)
{
  return (c == ';' || c == '|' || c == '&' || c == '(' || c == ')' || c == '<' || c == '>' || c == EOF);
}



command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{

  /*PART 1 -> ERROR CHECK & MAKE BUFFER */

  //Make the buffer
  char *a = (char*)malloc(sizeof(char) * 1000);
  int size = 0;
  char c;

  //take in all the input
  int line_count = 1;

  //Input Flags
  bool space = false;
  bool newline = false;
  bool and = false;
  bool or = false;
  bool lparen = false;
  bool rparen = false;
  bool semicolon = false;
  bool pipe = false;
  bool redirect = false;
  bool word = false;

  bool converted = false;
  bool initialword = false;
  int tempindex;
  int tempindex2;
  int leftcount = 0;
  int rightcount = 0;

  /*
     { is ||
     } is >>
     ~ is <&
     ` is >&
     [ is <>
     ] is >|
  */

        do
	  {
	    c = get_next_byte(get_next_byte_argument);

	    //Invalid characters produces error
	    if (!valid_char(c) && !valid_token(c))
	      {
		fprintf(stderr, "%d : Invalid character", line_count);
		exit(1);
	      }


	    //First character cnanot be any of the following
	    if (size == 0 && (c == ')' || c == ';' || c == '&' || c == '|' || c == '<'))
	      {
		fprintf(stderr, "%d : Invalid syntax at beginning of input", line_count);
		exit(1);
	      }

	    //If run into space, and a space has just been added to buffer, continue (so that final buffer doesn't have repeating spaces)
	    if (c == ' ' || c == '\t')
	      {
		if (space)
		  {
		    continue;
		  }

		else
		  {
		    c =  ' ';
		    space = true;
		  }
	      }

	    //If run into comment, keep getting input until \n to ignore the comment out of buffer. Also, use !space to check for unordinary token
	    else if (c == '#')
	      {
		if (!space && size != 0 && !newline)
		  {
		    fprintf(stderr, "%d: Not an ordinary token before #", line_count);
		    exit(1);
		  }
		if (newline)
		  {
		    a[tempindex2] = ' ';
		  }

		                  do
				    {
				      c = get_next_byte(get_next_byte_argument);
				    }
				  while ( c!= '\n');

				  if ( and || or || pipe || converted )
				    {
				      continue;
				    }

				  if (leftcount != rightcount)
				    {
				      fprintf(stderr, "%d: Mismatched Parentheses", line_count);
				      exit(1);
				    }

				  if (semicolon)
				    {
				      a[tempindex] = ' ';
				    }


				  if (newline)
				    {
				      a[tempindex2] = ' ';
				      if (initialword)
					{
					  c = '\n';
					}
				      else
					{
					  c = ' ';
					}

				      converted = true;
				    }

				  else
				    {
				      tempindex2 = size;
				      c = ';';
				      converted = false;
				    }

				  word = false;
				  space = false;
				  and = false;
				  or = false;
				  newline = true;
				  lparen = false;
				  rparen = false;
				  semicolon = false;
				  pipe = false;
				  redirect = false;
	      }

	    /*If run into new line, 
	          -increment line count
		      - if preceded by & or |, don't add to buffer
		          -parentheses need to match up if &/|/|| not preceding it
			      - can't be preceded by < or >
			          -if preceded by semicolon, remove it
				     -add a semicolon
				        -if just ran into newline, convert that semicolon to a newline char
					   -if just converted newline char, skip
					    
	    */
	    else if (c == '\n')
	      {
		if (redirect)
		  {
		    fprintf(stderr, "%d: Error near redirect", line_count);
		    exit(1);
		  }

		line_count++;

		if ( and || or || pipe || converted )
		  {
		    continue;
		  }

		if (leftcount != rightcount)
		  {
		    fprintf(stderr, "%d: Mismatched Parentheses", line_count);
		    exit(1);
		  }

		if (semicolon)
		  {
		    a[tempindex] = ' ';
		  }


		if (newline)
		  {
		    a[tempindex2] = ' ';
		    if (initialword)
		      {
			c = '\n';
		      }
		    else
		      {
			c = ' ';
		      }

		    converted = true;
		  }

		else
		  {
		    tempindex2 = size;
		    c = ';';
		    converted = false;
		  }



		word = false;
		space = false;
		and = false;
		or = false;
		lparen = false;
		rparen = false;
		redirect = false;
		newline = true;
		pipe = false;
		leftcount = 0;
		rightcount = 0;
	      }

	    //If run into ;, can't have ( or \n or & or | to left of it)
	    else if (c ==';')
	      {

		if ( (semicolon || lparen || and || or || newline || pipe || redirect) || !(word||rparen) )
		  {
		    fprintf(stderr, "%d : Invalid syntax near ;", line_count);
		    exit(1);
		  }

		tempindex = size;

		space = false;
		and = false;
		or = false;
		newline = false;
		lparen = false;
		rparen = false;
		semicolon = true;
		pipe = false;
		redirect = false;
		word = false;
		converted = false;
	      }

	    //If run into &, can't have ; or ( to left of it or | to left of it
	    //Need to convert && to & and can't have &&&
	    else if (c == '&')
	      {


		if ( (semicolon || lparen || and || or || pipe || newline) || !(word||rparen||redirect) )
		  {
		    fprintf(stderr, "%d : Invalid syntax near sequential command", line_count);
		    exit(1);
		  }

		if (redirect)
		  {
		    if (a[size-1] == '>')
		      {
			a[size-1] = '`';
		      }

		    else
		      {
			a[size-1] = '~';
		      }
		  
		    and = false;
		    or = false;
		    semicolon = false;
		    lparen = false;
		    rparen = false;
		    newline = false;
		    space = false;
		    pipe = false;
		    redirect = true;
		    word = false;
		    converted = false;

		    continue;
		  }


		c = get_next_byte(get_next_byte_argument);

		if (c != '&')
		  {
		    fprintf(stderr, "%d : Invalid syntax near sequential command", line_count);
		    exit(1);
		  }

		and = true;
		or = false;
		semicolon = false;
		lparen = false;
		rparen = false;
		newline = false;
		space = false;
		pipe = false;
		redirect = false;
		word = false;
		converted = false;
	      }

	    //If run into |, can't have ; or ( to left of it, makes || into {
	    else if (c == '|')
	      {
		if (semicolon || lparen || pipe || and  || newline)
		  {
		    fprintf(stderr, "%d : Invalid syntax near |", line_count);
		    exit(1);
		  }

		if (redirect && a[size-1] == '>')
		  {
		    a[size-1] = ']';
		    and = false;
		    converted = false;
		    semicolon = false;
		    lparen = false;
		    rparen = false;
		    newline = false;
		    space = false;
		    redirect = true;
		    pipe = false;
		    or = false;
		    word = false;

		    continue;
		  }

		if (or)
		  {
		    a[size-1] = '{';
		    pipe = true;
		    or = false;
		    continue;
		  }

		else
		  {
		    pipe = false;
		    or = true;
		  }

		and = false;
		converted = false;
		semicolon = false;
		lparen = false;
		rparen = false;
		newline = false;
		space = false;
		redirect = false;
		word = false;

	      }

	    //If run into >
	    else if ( c == '>')
	      {
		if (newline || semicolon || lparen || pipe || and || or)
		  {
		    fprintf(stderr, "%d : Invalid syntax near redirect", line_count);
		    exit(1);
		  }

		if (redirect)
		  {
		    if (a[size-1] == '>')
		      {
			a[size-1] = '}';
		      }

		    if(a[size-1] == '<')
		      {
			a[size-1] = '[';
		      }

		    and = false;
		    converted = false;
		    semicolon = false;
		    lparen = false;
		    rparen = false;
		    newline = false;
		    space = false;
		    redirect = true;
		    pipe = false;
		    or = false;
		    word = false;

		    continue;
		  }

		and = false;
		converted = false;
		semicolon = false;
		lparen = false;
		rparen = false;
		newline = false;
		space = false;
		redirect = true;
		pipe = false;
		or = false;
		word = false;
	      }

	    //If run into <
	    else if (c == '<')
	      {
		if (newline || semicolon || lparen || pipe || and || or || redirect)
		  {
		    fprintf(stderr, "%d : Invalid syntax near redirect", line_count);
		    exit(1);
		  }

		and = false;
		converted = false;
		semicolon = false;
		lparen = false;
		rparen = false;
		newline = false;
		space = false;
		redirect = true;
		pipe = false;
		or = false;
		word = false;
	      }


	    //If run into (
	    else if (c == '(')
	      {
		converted = false;
		leftcount++;
		lparen = true;
		and = false;
		semicolon = false;
		rparen = false;
		newline = false;
		space = false;
		redirect = false;
		pipe = false;
		or = false;
		word = false;
	      }

	    //If run into )
	    else if (c == ')')
	      {
		if (and || or || pipe || lparen || redirect)
		  {
		    fprintf(stderr, "%d : Invalid syntax near )", line_count);
		    exit(1);
		  }

		rightcount++;

		if (rightcount > leftcount)
		  {
		    fprintf(stderr, "%d : Invalid syntax near )", line_count);
		    exit(1);
		  }

		rparen = true;
		converted = false;
		and = false;
		semicolon = false;
		lparen = false;
		newline = false;
		space = false;
		redirect = false;
		pipe = false;
		or = false;
		word = false;
	      }

	    else if (c == EOF)
	      {
		if (semicolon)
		  {
		    a[tempindex] = ' ';
		  }

		if (newline)
		  {
		    a[tempindex2] = ' ';
		  }
		if (and || lparen || redirect || pipe || or )
		  {
		    fprintf(stderr, "%d : Invalid syntax near end of file", line_count);
		    exit(1);
		  }

		if (leftcount != rightcount)
		  {
		    fprintf(stderr, "%d : Mismatched Parentheses", line_count);
		    exit(1);
		  }


	      }
	    else
	      {
		converted = false;
		word = true;
		rparen = false;
		and = false;
		semicolon = false;
		lparen = false;
		newline = false;
		space = false;
		redirect = false;
		pipe = false;
		or = false;
		initialword = true;
	      }

	    a[size] = c;
	    size++;
	  }
	while (c!=EOF);


	/*PART 2 - MAKE LINKED LIST OF COMMAND TREES */

    //TODO: ACCOUNT FOR NEW COMMANDS!!!

    bool overwrite = true; 

	operator_stack_t op_stack = (operator_stack_t)malloc(sizeof(struct operator_stack));
	op_init(op_stack);

	command_stack_t com_stack = (command_stack_t)malloc(sizeof(struct command_stack));
	com_init(com_stack);


	command_stream_t new_stream = (command_stream_t)malloc(sizeof(struct command_stream));
	stream_init(new_stream);

	int x = 0;
	int y = 0;
	int n = 0;
	int m = 0;
	int q = 0;

	char** cur_word = (char**)malloc(sizeof(char*)*100);

	int zx = 0;
	while (zx < 100)
	  {
	    cur_word[zx] = (char*)malloc(sizeof(char)*100);
	    zx++;
	  }

	command_t temp_com = (command_t)malloc(sizeof(struct command));

	while(a[x] != -1 && a[x] != '\0')
	  {
	    if(a[x] == ' ')
	      {
			x++;
	      }

	    if(isalnum(a[x]) || (a[x] == '!') ||
	       (a[x] == '%') || (a[x] == '+')||
	       (a[x] == ',') || (a[x] == '-')||
	       (a[x] == '.') || (a[x] == '/')||
	       (a[x] == ':') || (a[x] == '@')||
	       (a[x] == '^'))
	      {
			y = x;

		while((a[y] != '&' ) &&
		      (a[y] != '{' ) &&
		      (a[y] != '|' ) &&
		      (a[y] != ';' ) &&
		      (a[y] != '(' ) &&
		      (a[y] != '>' ) &&
		      (a[y] != '<' ) &&
		      (a[y] != ')' ) &&
		      (a[y] != '}' ) &&
		      (a[y] != '~' ) &&
		      (a[y] != '`' ) &&
		      (a[y] != '[' ) &&
		      (a[y] != ']' ) && 
		      (a[y] != -1  ) &&
		      (a[y] != '\n') )
		  {
		    y++;
		  }

		while(x != y)
		  {
		    if(a[x] == ' ')
		      {

				while(a[x] == ' ')
			  	{
			    	x++;

			  	}
				n++;
				m = 0;
		      }
		    if(x != y)
		      {
				cur_word[n][m] = a[x];
				x++;
				m++;
		      }
		  }

		int size = 0;
		int j = 0;
		while (strlen(cur_word[j]) != 0)
		  {
		    size++;
		    j++;
		  }

		char **temp = (char**)malloc(sizeof(char*)*size);
		j = 0;
		int i = 0;
		int k = 0;
		while (j < size)
		  {
		    i = 0;
		    while (cur_word[j][i]!= '\0')
		      {
			i++;
		      }
		    temp[j] = (char*)malloc(sizeof(char)*i);

		    k = 0;
		    while ( k <=i)
		      {
			if ( k == i)
			  {
			    temp[j][k] = '\0';
			  }

			temp[j][k] = cur_word[j][k];
			k++;

		      }


		    j++;
		  }

		temp[j] = NULL;

		command_t tx = (command_t)malloc(sizeof(struct command));
		tx = create_command(SIMPLE_COMMAND, NULL, NULL, temp, NULL );
		com_push(com_stack, tx);
		n = 0;
		m = 0;

		int iter1 = 0;
		int iter2 = 0;
		while(strlen(cur_word[iter1]) != 0)
		  {
		    iter2=0;
		    while(cur_word[iter1][iter2] != '\0')
		      {
			cur_word[iter1][iter2] = '\0';
			iter2++;
		      }
		    iter1++;
		  }
	      }

	    if( a[x] == '&'||
		a[x] == '{'||
		a[x] == '|' ||
		a[x] == ';' ||
		a[x] == ')' ||
		a[x] == '('  )
	      {
			char *temp = (char*)malloc(sizeof(char));
			*temp = *(a+x);
			op_push(op_stack, temp);
			handle_stack(op_stack, com_stack);
			x++;
	      }

	    if(a[x] == '<')
	    {
			x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(a[x] != '&' &&
		        	a[x] != '|'  &&
		        	a[x] != '{' &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '>'  &&
		        	a[x] != '}'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}

			command_t  tz = (command_t)malloc(sizeof(struct command));
			tz = com_pop(com_stack);
			temp_com = tz;

			char* temp_word = (char*)malloc(sizeof(char));
			int Iter1 = 0;
			while(direct_word[Iter1] != '\0')
		  	{
		    	temp_word[Iter1] = direct_word[Iter1];
		    	direct_word[Iter1] = '\0';
		    	Iter1++;
		  	}

			temp_com->input = (char*)malloc(sizeof(char)*100);
			temp_com->input = temp_word;
			com_push(com_stack, temp_com);

			temp_word = NULL;
			q = 0;
	    }

	    if(a[x] == '}')
	    {
			x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(  a[x] != '&'  &&
		        	a[x] != '|'  &&
		        	a[x] != '{'  &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '<'  &&
		        	a[x] != '>'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}
				command_t  ta = (command_t)malloc(sizeof(struct command));
				ta = com_pop(com_stack);
				temp_com = ta;
				char* temp_word = (char*)malloc(sizeof(char));
				int Iter1 = 0;

		while(direct_word[Iter1] != '\0')
		  {
		    temp_word[Iter1] = direct_word[Iter1];
		    direct_word[Iter1] = '\0';
		    Iter1++;
		  }

		  	temp_com->append = (char*)malloc(sizeof(char)*100);
			temp_com->append = temp_word;
			com_push(com_stack, temp_com);

			temp_word = NULL;
			q = 0;

	    }

		if(a[x] == '>')
	    {
			x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(  a[x] != '&'  &&
		        	a[x] != '|'  &&
		        	a[x] != '{'  &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '<'  &&
		        	a[x] != '}'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}
				command_t  tz = (command_t)malloc(sizeof(struct command));
				tz = com_pop(com_stack);
				temp_com = tz;
				char* temp_word = (char*)malloc(sizeof(char));
				int Iter1 = 0;

		while(direct_word[Iter1] != '\0')
		  {
		    temp_word[Iter1] = direct_word[Iter1];
		    direct_word[Iter1] = '\0';
		    Iter1++;
		  }



			temp_com->output = (char*)malloc(sizeof(char)*100);
			temp_com->output = temp_word;
			com_push(com_stack, temp_com);

			temp_word = NULL;
			q = 0;

	    }

	    if(a[x] == '~')
	    {
	    	int target_fd; 
	    	command_t za = (command_t)malloc(sizeof(struct command)); 
	    	za = com_pop(com_stack); 
	    	if(isdigit(a[x-1]))
	    	{
	    		target_fd = a[x-1];
	    		za = com_pop(com_stack);  
	    	}
	    	else
	    	{
	    		target_fd = 0; 
	    	}
	    	
			x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(a[x] != '&' &&
		        	a[x] != '|'  &&
		        	a[x] != '{' &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '>'  &&
		        	a[x] != '}'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}

			
			char* temp_word = (char*)malloc(sizeof(char));
			int Iter1 = 0;
			while(direct_word[Iter1] != '\0')
		  	{
		    	temp_word[Iter1] = direct_word[Iter1];
		    	direct_word[Iter1] = '\0';
		    	Iter1++;
		  	}

			za->input = (char*)malloc(sizeof(char)*100);
			za->input = temp_word;
			za->duplicate_input = target_fd;
			za->print_this = true;  
			com_push(com_stack, za);

			temp_word = NULL;
			q = 0;	    	
	    }

   		if(a[x] == '`')
	    {
	    	int target_fd; 
	    	command_t zb = (command_t)malloc(sizeof(struct command)); 
	    	zb = com_pop(com_stack); 
	    	if(isdigit(a[x-1]))
	    	{
	    		target_fd = a[x-1];
	    		zb = com_pop(com_stack);  
	    	}
	    	else
	    	{
	    		target_fd = 0; 
	    	}
	    	
			x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(a[x] != '&' &&
		        	a[x] != '|'  &&
		        	a[x] != '{' &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '>'  &&
		        	a[x] != '}'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}

			
			char* temp_word = (char*)malloc(sizeof(char));
			int Iter1 = 0;
			while(direct_word[Iter1] != '\0')
		  	{
		    	temp_word[Iter1] = direct_word[Iter1];
		    	direct_word[Iter1] = '\0';
		    	Iter1++;
		  	}

			zb->output = (char*)malloc(sizeof(char)*100);
			zb->output = temp_word;
			zb->duplicate_output = target_fd;
			zb->print_this = true;  
			com_push(com_stack, zb);

			temp_word = NULL;
			q = 0;	    	
	    }	    

	    if(a[x] == '[')
	    {
	    	x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(  a[x] != '&'  &&
		        	a[x] != '|'  &&
		        	a[x] != '{'  &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '<'  &&
		        	a[x] != '}'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}
				command_t  zc = (command_t)malloc(sizeof(struct command));
				zc = com_pop(com_stack);
				temp_com = zc;
				char* temp_word = (char*)malloc(sizeof(char));
				int Iter1 = 0;

		while(direct_word[Iter1] != '\0')
		  {
		    temp_word[Iter1] = direct_word[Iter1];
		    direct_word[Iter1] = '\0';
		    Iter1++;
		  }



			temp_com->output = (char*)malloc(sizeof(char)*100);
			temp_com->output = temp_word;
			temp_com->input = (char*)malloc(sizeof(char)*100); 
			temp_com->input = temp_word; 
			com_push(com_stack, temp_com);

			temp_word = NULL;
			q = 0;
	    	
	    }

	    if(a[x] == ']')
	    {
	    	x++;
			char* direct_word=(char*)malloc(sizeof(char)*100);
			while(a[x] == ' ')
		  	{
		    	x++;
		  	}

			while(  a[x] != '&'  &&
		        	a[x] != '|'  &&
		        	a[x] != '{'  &&
		        	a[x] != ';'  &&
		        	a[x] != ' '  &&
		        	a[x] != ')'  &&
		        	a[x] != '('  &&
		        	a[x] != '<'  &&
		        	a[x] != '}'  &&
		        	a[x] != '~'  &&
		        	a[x] != '`'  &&
		        	a[x] != '['  &&
		        	a[x] != ']'  &&
 		            a[x] != '\n' &&
		            a[x] != '\t' &&
		      		a[x] != -1    )
		    {

		    	direct_word[q] = a[x];
		    	x++;
		    	q++;
		  	}
				command_t  zd = (command_t)malloc(sizeof(struct command));
				zd = com_pop(com_stack);
				temp_com = zd;
				char* temp_word = (char*)malloc(sizeof(char));
				int Iter1 = 0;

		while(direct_word[Iter1] != '\0')
		  {
		    temp_word[Iter1] = direct_word[Iter1];
		    direct_word[Iter1] = '\0';
		    Iter1++;
		  }



			temp_com->output = (char*)malloc(sizeof(char)*100);
			temp_com->output = temp_word;
			temp_com -> force_overwrite = true; 
			com_push(com_stack, temp_com);

			temp_word = NULL;
			q = 0;

	    }



	    if(a[x] == '\n' || a[x] == -1)
	      {

		finish_stack(op_stack, com_stack);


		command_node_t new_node = (command_node_t)malloc(sizeof(struct command_node));
		node_init(new_node);
		new_node->root = (command_t)malloc(sizeof(struct command));
		new_node -> root = com_pop(com_stack);

		if(new_stream->tail == NULL)
		  {
		    new_stream->tail = (command_node_t)malloc(sizeof(struct command_node));
		    node_init(new_stream->tail);
		    new_stream->tail = new_node;
		  }
		else
		  {
		    new_stream->tail->next = (command_node_t)malloc(sizeof(struct command_node));
		    node_init(new_stream->tail->next);
		    new_stream->tail->next = new_node;
		    new_stream->tail = new_stream->tail->next;
		  }


		if(new_stream->head == NULL)
		  {
		    new_stream->head = (command_node_t)malloc(sizeof(struct command_node));
		    node_init(new_stream->head);
		    new_stream->head = new_stream->tail;
		  }

		x++;

	      }
	    y = 0;
	  }

	new_stream->cursor = (command_node_t)malloc(sizeof(struct command_node));
	new_stream->cursor = new_stream->head;
	return new_stream;
}



command_t
read_command_stream (command_stream_t s)
{
  /* Traversal of linked list  */
  if(s->cursor == NULL)
    {
      return NULL;
    }
  else
    {

      command_t temp = (command_t)malloc(sizeof(struct command));
      temp = s->cursor->root;

      s->cursor = s->cursor->next;
      return temp;
    }
}


