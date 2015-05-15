 // UCLA CS 111 Lab 1 command internals

enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
      
      
    //Part 1A Design
    ONE_COMMAND,        // A >> B     - redirect standard output to replace current contents
    TWO_COMMAND,        // A >& B     - redirect standard output and standard error to replace current contents
    THREE_COMMAND,      // A <& B       -??
    FOUR_COMMAND,       // A <> B     - ??
    FIVE_COMMAND,       // A >| B     - ??
  };

// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  int status;

  // I/O redirections, or null if none.
  char *input;
  char *output;

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;
};
