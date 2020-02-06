# Nush Shell

_Project for CS3650, Computer Systems_

Represents a basic Bash shell that continually takes in commands
and inputs, tokenizes them, and executes the commands by forking.

The shell only takes in one line of commands at a time.

Each line is parsed into a set of tokens to construct an abstract
syntax binary tree, which is then executed in the main.c file.
In execution, the program forks and execs the command in the
child process, depending on the operator given.

**Functionality**:
* Continually receives lines of commands from the terminal.
* Tokenizes the command to construct an abstract syntax tree to
execute the commands.
* Forks to create a child process, executing the command in the child.
Parent waits on child unless specified otherwise.
* Changes directory if given a "cd" command.
* Exits the shell if "exit" is given.

**Accepted Operators:**
* Redirecting input: <
* Redirecting output: >
* Piping: |
* Background operations: &
* And operator: &&
* Or operator: ||
* Semicolons: ;

Any additional commands handled by system exec().