// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>

//Added directives
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

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
		  _exit(c->u.command[1]->status);
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

  //If in child process, execute child -> executes command on right
  if (p == 0)
    {
      //Execute command on the right
      execute_command(c->u.command[0], false);

      //Set status of internal command
      _exit(c->u.command[0]->status);
    }

  //This block is executed by parent process, which
  //executes the command on left
  else
    {
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
    execute_command(c->u.subshell_command, false);
    break;
  default:
    break;
  }

}
