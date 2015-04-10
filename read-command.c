/*
 TODO
 
 deal with comments
 
 
 */






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

int get_precedence(char* op)
{
	//|, ||/&&, ;/\n

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

command_t create_command(enum command_type type, command_t* child_1, command_t* child_2, char** word, command_t* subshell)
{


        command_t return_val;
        return_val->type = type; 
	return_val->u.command[0] = child_1; 
	return_val->u.command[1] = child_2; 
	return_val->u.word = word;
	return_val->u.subshell_command; 
	return return_val;  
}



void handle_stack(operator_stack_t *stacko, command_stack_t *stackc)
{
	if(op_empty(stacko))
	{
		return; 
	}

	if(stacko->top == 1)
	{
		return; 
	}

	char* cur_op;
	char* next_op;  
	command_t* cur_com; 

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
		command_t* new_com = create_command(SUBSHELL_COMMAND, NULL, NULL, NULL, com_pop(stackc));
	}

	command_t* com_1; 
	command_t* com_2; 
	command_t* new_com; 
	while(get_precedence(next_op) >= get_precedence(cur_op))
	{
		com_1 = com_pop(stackc); 
		com_2 = com_pop(stackc); 

		if(*next_op == '|')
		{
			new_com = create_command(PIPE_COMMAND, com_1, com_2, NULL, NULL);
			com_push(stackc, &new_com); 			
		}
		if(*next_op == '&')
		{
			new_com = create_command(AND_COMMAND, com_1, com_2, NULL, NULL);
			com_push(stackc, &new_com);  
		}
		if(*next_op == '{')
		{
			new_com = create_command(OR_COMMAND, com_1, com_2, NULL, NULL);
			com_push(stackc, &new_com); 
		}
		if(*next_op == ';')
		{
			new_com = create_command(SEQUENCE_COMMAND, com_1, com_2, NULL, NULL); 
			com_push(stackc, &new_com); 
		}
		

		next_op = op_pop(stacko); 

	}

	op_push(stacko, next_op);
	op_push(stacko, cur_op); 

}

void finish_stack(operator_stack_t *stacko, command_stack_t *stackc)
{
	char* cur_op; 
	command_t com_1; 
	command_t com_2; 
	command_t new_com; 

	while(!op_empty(stacko))
	{
		cur_op = op_pop(stacko);
		com_1 = com_pop(stackc); 
		com_2 = com_pop(stackc);  

		if(*cur_op == '|')
		{
			new_com = create_command(PIPE_COMMAND, com_1, com_2, NULL, NULL);
			com_push(stackc, &new_com); 			
		}
		if(*cur_op == '&')
		{
			new_com = create_command(AND_COMMAND, com_1, com_2, NULL, NULL);
			com_push(stackc, &new_com);  
		}
		if(*cur_op == '{')
		{
			new_com = create_command(OR_COMMAND, com_1, com_2, NULL, NULL);
			com_push(stackc, &new_com); 
		}
		if(*cur_op == ';')
		{
			new_com = create_command(SEQUENCE_COMMAND, com_1, com_2, NULL, NULL); 
			com_push(stackc, &new_com); 
		}
	
	}
}

enum parser_component
{
  WORD,
  SEMICOLON,
  ANDOR,
  PIPE,
  POUND,
  NEWLINE,
  INPUT, // <
  OUTPUT, //>
  ERROR,
    LPAREN,
    RPAREN,
  SPACE,
};


typedef struct command_node *command_node_t;

//command tree
struct command_node
{
  command_t* root;
  command_node_t next;
};

void node_init(command_node_t node)
{
	node->root = NULL; 
	node->next = NULL; 
}


//linked list of command nodes
struct command_stream
{
  command_node_t head;
  command_node_t tail;
  command_node_t cursor;
};

void stream_init(command_stream_t stream)
{
	stream->head = NULL; 
	stream->tail = NULL; 
	stream->cursor = NULL; 
}


/*Process character buffer such that
 ->&& becomes &
 -> || becomes {
 -> comments # are removed
 -> The processed buffer follows the syntax Complete Command \n Complete Command \n Complete Command \n etc...
 -> removes ALL spaces
 */
void process(char* a, int size)
{
    int i = 0;
    int x = 0;
    int temp = 0;
    while ( i < size)
    {
        //Remove && and replace with
        if (a[i] == '&')
        {
            x = i;
            size--;
            do
            {
                x++;
                a[x-1] = a[x];
            }
            while (x < size);
        }
        
        //Remove || and replace with {, need to copy everything past it backwards
        if (a[i] == '|' && a[i+1] == '|')
        {
            x=i;
            size--;
            a[x+1] = '{';
            do
            {
                x++;
                a[x-1] = a[x];
            }
            while (x < size);
        }
        
        //Remove all characters from # to first newline character
        if (a[i] == '#')
        {
            
            //count number of characters of comment
            temp = 1;
            x = i;
            do
            {
                x++;
                temp++;
            }
            while (a[x] != '\n' || a[x] != EOF);
            
            //Overwrite that # characters effectively removing the comment
            while (temp > 0)
            {
                x= i;
                size--;
                do
                {
                    x++;
                    a[x-1] = a[x];
                }
                while (x < size);
                temp--;
            }
            
        }
        
        //Remove all consecutive newline characters
        if (a[i] == '\n')
        {
            //count number of consecutive newline characters
            temp = 0;
            x = i;
            do
            {
                x++;
                temp++;
            }
            while (a[x] == '\n');
            
            //Overwrite that # characters such that only one \n remains
            while (temp > 1)
            {
                x= i;
                size--;
                do
                {
                    x++;
                    a[x-1] = a[x];
                }
                while (x < size);
                temp--;
            }
        }
        
        //Remove all spaces
        if (a[i] == ' ')
        {
            //count number of consecutive newline characters
            temp = 1;
            x = i;
            do
            {
                x++;
                temp++;
            }
            while (a[x] == ' ');
            
            //Overwrite that # characters such that no space remains
            while (temp > 0)
            {
                x= i;
                size--;
                do
                {
                    x++;
                    a[x-1] = a[x];
                }
                while (x < size);
                temp--;
            }
        }
        
        i++;
    }
    
    i = 0;
    while (i < size)
    {
        //Handles the case where you have 'a &\n b' or 'a | \n b'
        if (a[i] == '{' || a[i] == '&' || a[i] == '|')
        {
            if (a[i+1] == '\n')
            {
                x = i;
                size--;
                do
                {
                    x++;
                    a[x-1] = a[x];
                }
                while (x < size);
                temp--;
            }
          
        }
        
        i++;
    }
    
    
    
}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
    	     void *get_next_byte_argument)
{
    //Make the buffer
    char *a = malloc(sizeof(char) * 1000);
    int size = 0;
    char c;

    //take in all the input
    do
      {
		c = get_next_byte(get_next_byte_argument);
		a[size] = c;
		size++;
      }
    while (c != EOF);

    //enumerate the buffer
    int index = 0;

    enum parser_component  *enumerated_array = malloc(sizeof(enum parser_component)*size);

    //parser_component_t *enumerated_array = malloc(sizeof(parser_component)*size);
    int z = 0;
    
    //convert buffer into enumerated buffer
    while (z < size) 
    {
		if (z ==0)
	  	{
	    	if ((a[z] =='|' && a[z+1] == '|') ||(a[z] =='&' && a[z+1] == '&')) 
	      	{
				enumerated_array[index] = ANDOR;
				z += 2;
				index++;
	      	}

	      	if (a[z] == '|' && a[z+1] != '|')
	      	{
				enumerated_array[index] = PIPE;
				z++;
				index++;
	      	}
	  	}

		if (z == size)
	 	{
	   		if ((a[z] =='|' && a[z-1] == '|') ||(a[z] =='&' && a[z-1] == '&'))
	  		{
	    		enumerated_array[index] = ANDOR;
	    		z += 2;
	    		index++;
	  		}

	  		if (a[z] == '|' && a[z-1] != '|')
	    	{
	      		enumerated_array[index] = PIPE;
	      		z++;
	      		index++;
	    	}
	 	}

		if(z != 0 && z != size)
	  	{
	    	if (  (a[z] == '&' && a[z+1] =='&' ) || (a[z] == '|' && a[z+1] == '|') )
	    	{
	      		enumerated_array[index] = ANDOR;
	      		z+=2;
	      		index++;
	    	}
	    	else if ( a[z] == '|' && a[z-1] != '|' && a[z+1] !='|' )
	    	{
	      		enumerated_array[index] = PIPE;
	      		z++;
	      		index++;
	    	}
	    	else if(a[z] == ';')
	    	{
	    		enumerated_array[index] = SEMICOLON;
	    		index++;
	    		z++;
	    	}
	    	else if(a[z] == '#')
	    	{
                if (a[z-1] != ' ' || a[z-1] != '\t' || a[z-1] != '\n')
                {
                    enumerated_array[index] = ERROR;
                }
                else
                {
                    enumerated_array[index] = POUND;
                }
	    		index++;
	    		z++;
	    	}
	    	else if(a[z] == '\n')
	    	{
	    		enumerated_array[index] = NEWLINE;
	    		index++;
	    		z++;
	    	}
	    	else if(a[z] == '<')
	    	{
	    		enumerated_array[index] = INPUT;
	    		index++;
	    		z++;
	    	}
	    	else if(a[z] == '>')
	    	{
	    		enumerated_array[index] = OUTPUT;
	    		index++;
	    		z++;
	    	}
        	else if(a[z] == '(')
        	{
            	enumerated_array[index] = INPUT;
            	index++;
           	 	z++;
        	}
        	else if(a[z] == ')')
        	{
            	enumerated_array[index] = OUTPUT;
            	index++;
            	z++;
        	}
          
	    	else if(isalnum(a[z])==0 || (a[z] == '!') || (a[z] == '%') || (a[z] == '+') || (a[z] == ',') || (a[z] == '-') || (a[z] == '.') || (a[z] == '/') || (a[z] == ':') || (a[z] == '@') || (a[z] == '^') || (a[z] == '_'))
	    	{
            	while (!(isalnum(a[z])==0 || (a[z] == '!') || (a[z] == '%') || (a[z] == '+') || (a[z] == ',') || (a[z] == '-') || (a[z] == '.') || (a[z] == '/') || (a[z] == ':') || (a[z] == '@') || (a[z] == '^') || (a[z] == '_')))
            	{
                	z++;
            	}
            
				enumerated_array[index] = WORD;
				index++;
	    	}
        	else if (a[z] == ' ' || a[z] == '\t' )
        	{
            	//converts any sequence of spaces into one space enumeration
            	while (a[z] == ' ' || a[z] == '\t' )
            	{
                	z++;
            	}
            	enumerated_array[index] = SPACE;
            	index++;
        	}
	  	}
    }
    
    
	//Implement Error Checks for Enumerated Buffer
    int line_count = 1;
    int i = 0;
    int leftcount = 0;
    int rightcount = 0;
    
    
    while (i < index)
    {
        if (rightcount > leftcount)
        {
            fprintf(stderr, "%d: No matching parentheses", line_count);
        }
        
        while (i < index)
        {
            if ( enumerated_array[i] == ERROR )
            {
                fprintf(stderr, "%d : Invalid command", line_count);
            }
            
            //First enumeration cannot be any of the following
            if (i == 0)
            {
                if ( enumerated_array[i] == RPAREN )
                {
                    fprintf(stderr, "%d : Command cannot begin with )", line_count);
                    exit(1);
                }
                
                if ( enumerated_array[i] == SEMICOLON )
                {
                    fprintf(stderr, "%d : Command cannot begin with ;", line_count);
                    exit(1);
                }
                
                if ( enumerated_array[i] == ANDOR )
                {
                    fprintf(stderr, "%d : Command cannot begin with && or || ", line_count);
                    exit(1);
                }
                
                if ( enumerated_array[i] == PIPE )
                {
                    fprintf(stderr, "%d : Command cannot begin with |", line_count);
                    exit(1);
                }

                
                if ( enumerated_array[i] == INPUT )
                {
                    fprintf(stderr, "%d : Command cannot begin with <", line_count);
                    exit(1);
                }
            }
            
            //Last enumeration cannot be any of the following
            if (i == index - 1)
            {
                
                //if (enumerated_array[i] == )
                
            }
            
        // Checks for Encountering ;
            /*  1. cannot be surrounded by &&, ||, |
                2. cannot have (, \n to the left of it
                    --note that if there are spaces need to keep checking for next command on either side
             */
        
        	enum parser_component left = ERROR;
        	enum parser_component right = ERROR;
        
        	if ( enumerated_array[i] == SEMICOLON)
        	{
            	//Get command to left of ;
            	int j = i;
            	while (j >= 0)
            	{
                	if (enumerated_array[j] != SPACE)
                	{
                    left = enumerated_array[j];
                    break;
                	}
                	j--;
            	}
            
            	j = i;
            
            	//Get command to right of ;
            	while (j < size)
            	{
                	if (enumerated_array[j] != SPACE   )
                	{
                    right = enumerated_array[j];
                    break;
                	}
                	j++;
            	}
            
            	//1
            	if ( left == LPAREN || left == ANDOR || left == PIPE || left == NEWLINE || left == ERROR)
            	{
                  	fprintf(stderr, "%d : Syntax error near ;", line_count);
                	exit(1);
            	}
            	//2
            	if ( right ==  ANDOR || right == PIPE || right == ERROR)
            	{
                  	fprintf(stderr, "%d : Syntax error near ;", line_count);
                	exit(1);
            	}	
            
            
        	}
        
        
        	left = ERROR;
        	right = ERROR;
        	// Checks for Encountering && or ||
            /* 1. MUST be surrounded by words
                --Right side can have new lines until word, but left side cannot
                --the exception is that left side can have ) and right side can have (
             */
        	if ( enumerated_array[i] == ANDOR)
        	{
            	//Get command to left of ;
            	int j = i;
            	while (j >= 0)
            	{
                	if (enumerated_array[j] != SPACE)
                	{
                    	left = enumerated_array[j];
                    	break;
                	}
                	j--;
            	}
            
            	j = i;
            
            	//Get command to right of ;
            	while (j < size)
            	{
                	if (enumerated_array[j] != SPACE  && enumerated_array[j] != NEWLINE  )
                	{
                    right = enumerated_array[j];
                    break;
                	}
                	j++;
            	}
            
            	//1
            	if ( left != WORD || right != WORD || left != RPAREN || right != LPAREN )
            	{
                  	fprintf(stderr, "%d : Syntax error near sequential command.", line_count);
                	exit(1);
            	}
        	}
        
        
        	left = ERROR;
        	right = ERROR;
        	// Checks for Encountering |
        /*  1. left MUST be word or ), right MUST be word or (
                --can check past newlines/spaces
         
         */
        
        
       	 	if ( enumerated_array[i] == PIPE)
        	{
            	//Get command to left of ;
            	int j = i;
            	while (j >= 0)
            	{
                	if (enumerated_array[j] != SPACE && enumerated_array[j] != NEWLINE )
                	{
                    	left = enumerated_array[j];
                    	break;
                	}
                	j--;
            	}
            
            	j = i;
            
            	//get command to right of ;
            	while (j < size)
            	{
                	if (enumerated_array[j] != SPACE  && enumerated_array[j] != NEWLINE  )
                	{
                    	right = enumerated_array[j];
                    	break;
                	}
                	j++;
            	}
            
            	//1
            	if ( left != WORD || right != WORD || left != RPAREN || right != LPAREN)
            	{
                 	 fprintf(stderr, "%d : Syntax error near pipe command.", line_count);
                	exit(1);
            	}
        	}
        
        // Checks for Encountering #
        /*
            1. can NOT have ordinary token immediately left of it ?? what is an ordinary token
            2. right can be anything...is any comment until new line character
         */
        
        /*
        left = ERROR;
        right = ERROR;
        if (enumerated_array[i] == POUND)
        {
            if (i-1 >= 0)
            {
                left = enumerated_array[i-1];
            }
            
            if (left == ANDOR || left == SEMICOLON || left == )
            
            
        }
        */
        
        
        // Checks for Encountering \n
        /*
         None. A newline character can be anywhere
         Increments the line counter
         */
            
            if ( enumerated_array[i] == NEWLINE)
            {
                line_count++;
            }
            
            left = ERROR;
            right = ERROR;
        // Checks for Encountering Input <
        /*
         1. cannot be surrounded by newlines, &&, ||, |, ;, <, > and can check for these past spaces
         2. can have ) to the left of it or ( to the right of it
        */
        // Checks for Encountering Output >
        /*
            1. cannot be surrounded by newlines, &&, ||, |, ;, <, > and can check for these past spaces
            2. can have ) to the left of it or ( to the right of it
         */
            
            if ( enumerated_array[i] == OUTPUT || enumerated_array[i] == INPUT)
            {
                //Get command to left of ;
                int j = i;
                while (j >= 0)
                {
                    if (enumerated_array[j] != SPACE)
                    {
                        left = enumerated_array[j];
                        break;
                    }
                    j--;
                }
                
                j = i;
                
                //Get command to right of ;
                while (j < size)
                {
                    if (enumerated_array[j] != SPACE )
                    {
                        right = enumerated_array[j];
                        break;
                    }
                    j++;
                }
                
                //1
                if ( left == NEWLINE || left == ANDOR || left == PIPE || left == SEMICOLON || left == INPUT || left == OUTPUT || left != RPAREN || left == ERROR ||
                    right == NEWLINE || right == ANDOR || right == PIPE || right == SEMICOLON || right == INPUT || right == OUTPUT || right != LPAREN || right == ERROR)
                {
                    fprintf(stderr, "%d : Syntax error I/O command.", line_count);
                    exit(1);
                }
            }
            
        // Checks for Encountering (
        /*
         */
            
            if (enumerated_array[i] == LPAREN)
            {
                leftcount++;
            }
        // Checks for Encountering )
        /*
         */
            if (enumerated_array[i]== RPAREN)
            {
                if (enumerated_array[i-1] == LPAREN)
                {
                    fprintf(stderr, "%d : Empty subshell", line_count);
                }
                rightcount++;
            }
        // Checks for Encountering Space
        /*
         */
        // Checks for Encountering Word
        /*
         */
        //INCREMENT COUNTER
        }
        i++;
    }

//At this point, the input has passed all tests, so it's valid, now need to process
    
    
/*Process character buffer such that
    ->&& becomes &
    -> || becomes {
    -> comments # are removed
    -> The processed buffer follows the syntax Complete Command \n Complete Command \n Complete Command \n etc...
*/
    
    process(a, size);

//create command trees
	operator_stack_t op_stack; 
	op_init(&op_stack); 

	command_stack_t com_stack; 
	com_init(&com_stack);

	command_node_t new_node; 
	node_init(&new_node); 

	command_stream_t new_stream;
	stream_init(&new_stream);  



      int x = 0;
      int y = 0;
      int n = 0;
      int m = 0;
      int q = 0; 

      char* direct_word[100];   
      char** cur_word[100][100]; 

      command_t temp_com; 

      while(a[x] != '\0')
      {
      	if(isalnum(a[x]) || (a[x] == '!') || 
      		(a[x] == '%') || (a[x] == '+')|| 
      		(a[x] == ',') || (a[x] == '-')|| 
      		(a[x] == '.') || (a[x] == '/')|| 
      		(a[x] == ':') || (a[x] == '@')|| 
      		(a[x] == '^')) 
      	{
      		y = x; 
      		
      		while((a[x] != '&') ||
      		      (a[x] != '{') ||
      		      (a[x] != '|' ) ||
      		      (a[x] != ';' ) ||
      		      (a[x] != '(' ) ||
      		      (a[x] != ')' )  ) 
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
      			}
      			cur_word[n][m] = a[x];
      			x++;
      			m++;    
      		}

      		com_push(&com_stack, create_command(SIMPLE_COMMAND, NULL, NULL, cur_word, NULL ));
      		n = 0; 
      		m = 0;  
      	}

      	if( a[x] == '&'||
      		a[x] == '{'||
      		a[x] == '|' ||
      		a[x] == ';' ||
      		a[x] == ')' ||
      		a[x] == '('  ) 
      	{
      		op_push(&op_stack, a[x]);
      		handle_stack(&op_stack, &com_stack);  
      		x++; 
      	}

      	if(a[x] == '<')
      	{
      		while(a[x] == ' ')
      		{
      			x++; 
      		}

      		while(a[x] != '&' ||
      			  a[x] != '|'  ||
      			  a[x] != '{' ||
      			  a[x] != ';'  ||
      			  a[x] != ' '  ||   
      			  a[x] != ')'  ||
      			  a[x] != '('   )
      		{
      			direct_word[q] = a[x];
      			x++;
      			q++;
      		}	

      		temp_com = com_pop(&com_stack);
      		temp_com->input = direct_word; 
      		com_push(&com_stack, temp_com); 
      	}

		if(a[x] == ">")
      	{
      		while(a[x] == " ")
      		{
      			x++; 
      		}

      		while(a[x] != '&' ||
      			  a[x] != '|' ||
      			  a[x] != '{' ||
      			  a[x] != ';' ||
      			  a[x] != ' ' ||
      			  a[x] != ')' ||
      			  a[x] != '(' )
      		{
      			direct_word[q] = a[x];
      			x++;
      			q++;
      		}	

      		temp_com = com_pop(&com_stack);
      		temp_com->output = direct_word; 
      		com_push(&com_stack, temp_com); 
      	}

      	if(a[x] == '\n')
      	{
      		finish_stack(&op_stack, &com_stack);
      		new_node->root = com_pop(&com_stack);  
      	}

      		if(new_stream->tail == NULL)
      		{
      			new_stream->tail = &new_node;	
      		}
      		else
      		{
      			new_stream->tail->next = &new_node; 
      			new_stream->tail = new_stream->tail->next; 
      		}
      		 

      		if(new_stream->head == NULL)
      		{
      			new_stream->head = new_stream->tail; 
      		}


			x++;       		 
			y = 0; 
    }

	  new_stream->cursor = new_stream->head;
      return new_stream; 

	      
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //error (1, 0, "command reading not yet implemented");
  //return 0;
}



command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}


