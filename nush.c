#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>

#include "tokenizer.h"
#include "list.h"
#include "ast.h"

/*
 * Executes a basic child process, should only be done within a fork.
 *
 * Takes in an abstract syntax tree of commands.
 */
void execute_child(ast* aa) {
    if (!aa) {
        return;
    }
    int rv;
    // aa->cmd is the list of strings for the command and arguments
    // Command is first of the list of commands and arguments
    char* cmd = aa->cmd->data[0];
    // Exit
    if (strcmp(cmd, "exit") == 0) {
        return;
    } else if (strcmp(cmd, "cd") == 0) {
        // Changing directory
        rv = chdir(aa->cmd->data[1]);
        assert(rv != -1);
    } else {
        // Array of char* of the size of the list of command/arguments
        char* args[aa->cmd->size];
        // Loops from 0 to aa->cmd->size - 1, which is the size of the list
        // of arguments and commands in the AST leaf
        int k = 0;
        while (k < aa->cmd->size) {
            args[k] = aa->cmd->data[k];
            if (strcmp(args[k], "exit") == 0) {
                // Returns and exits out of the current command if an exit
                // command is detected
                return;
            }
            k++;
        }
        // Null terminating bytes
        cmd[strlen(cmd)] = 0;
        args[k] = 0;
        // Executes the command
        execvp(cmd, args);
    }
}

/*
 * Executes the command of the given AST leaf.
 * Takes in the AST and an integer representing to wait for the child or not.
 * 1 = wait
 * 0 = do not wait
 */
int execute_ast(ast* aa, int wait) {
    if (!aa) {
        return 1;
    }
    // Cpid is the ID of the child, rv is for error assertions
    int cpid;
    int rv;
    // Forks once
    if ((cpid = fork())) {
        int status = 0;
        char* cmd = aa->cmd->data[0];
        // Covers if an exit is discovered, continues evaluating after
        // the ;
        if (strcmp(cmd, "exit") == 0) {
            // Kills the child process if an exit command is found
            kill(cpid, SIGKILL);
            exit(1);
        }
        // If were are waiting for the child:
        if (wait) {
            rv = waitpid(cpid, &status, 0);
            assert(rv != -1);
        }
        // Returning the exit code of the child
        if (status) {
            return WIFEXITED(status) ? WEXITSTATUS(status) : 1
        }
    } else {
        // Child process:
        execute_child(aa);
    }
}

/*
 * Redirects input or output depending on the given direction.
 * 1 = redirect output
 * 0 = redirect input
 * Takes in the AST and integer direction.
 */
int redirect(ast* aa, int direction) {
    if (!aa) {
        return 1;
    }
    // CPID is the ID of the child
    // rv is for assertion and error checking
    int cpid;
    int rv;
    // Single fork
    if ((cpid = fork())) {
        int status = 0;
        // Waiting for the child
        rv = waitpid(cpid, &status, 0);
        assert(rv != -1);
        
        // Returning the exit code of the child
        if (status) {
            return WIFEXITED(status) ? WEXITSTATUS(status) : 1
        }
    } else {
        // Child process:
        // File:
        int fd;
        // arg2 is the file
        if (direction) {
            // Redirecting output
            fd = open(aa->arg2->cmd->data[0], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        } else {
            // Redirecting input
            fd = open(aa->arg2->cmd->data[0], O_CREAT | O_APPEND | O_RDONLY, 0644);
        }
        // Closes stdin or stdout
        rv = close(direction);
        assert(rv != -1);
        dup(fd);
        rv = close(fd);
        assert(rv != -1);
        
        // Executing arg1
        ast* ab = aa->arg1;
        execute_child(ab);
    }
}

/*
 * Pipes the arguments in the abstract syntax tree.
 * Takes in an abstract syntax tree.
 * Returns if the operation succeeded (1) or not (status exit code).
 */
int pipe(ast* aa) {
    int rv;
    int cpid;
    // Pipe:
    // Parent
    if ((cpid = fork())) {
        int status;
        rv = waitpid(cpid, &status, 0);
        assert(rv != -1);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1
    } else {
        // Child
        // Makes the pipe
        int pipes[2];
        rv = pipe(pipes);
        int read = pipes[0];
        int write = pipes[1];
        assert(rv != -1);
        if ((cpid = fork())) {
            // Child/parent
            rv = close(write);
            assert(rv != -1);
            rv = close(0);
            assert(rv != -1);
            rv = dup(read);
            assert(rv != -1);
            rv = close(read);
            assert(rv != -1);
            // Executing command 2
            execute_ast_tree(aa->arg2, 1);

            int status;
            rv = waitpid(cpid, &status, 0);
            assert(rv != -1);

        } else {
            // Child/child
            rv = close(read);
            assert(rv != -1);
            rv = close(1);
            assert(rv != -1);
            rv = dup(write);
            assert(rv != -1);
            rv = close(write);
            assert(rv != -1);
            // Executing command 1
            execute_ast_tree(aa->arg1, 1);
        }
    }
}

/*
 * Executes the given abstract syntax tree.
 * Takes in the AST and an integer indicating whether to wait for the child (1) or not (0).
 * Returns an integer representing if the execution succeeded or not.
 */
int execute_ast_tree(ast* aa, int wait) {
    if (!aa) {
        return 1;
    }
    // Checking for operator:
    if (aa->op) {
        char* op = aa->op;
        int result1;
        int rv;
        int cpid;
        // || operator
        if (strcmp(op, "||") == 0) {
            result1 = execute_ast_tree(aa->arg1, 1);
            if (!result1) {
                return result1;
            } else {
                return execute_ast_tree(aa->arg2, 1);
            }
            // && operator
        } else if (strcmp(op, "&&") == 0) {
            result1 = execute_ast_tree(aa->arg1, 1);
            if (result1) {
                return result1;
            } else {
                return execute_ast_tree(aa->arg2, 1);
            }
        }
        // Switch for the other operators
        switch (op[0]) {
            case ';':
                // Semicolon:
                // Basically executes both commands normally
                result1 = execute_ast_tree(aa->arg1, 1);
                if (aa->arg2) {
                    return execute_ast_tree(aa->arg2, 1);
                } else {
                    return result1;
                }
            case '|':
                return pipe(aa)
            case '&':
                // Background operator
                return result1 = execute_ast_tree(aa->arg1, 0);
            case '>':
                // Redirects output
                return redirect(aa, 1);
            case '<':
                // Redirects input
                return redirect(aa, 0);
            default:
                // For invalid operators:
                printf("Invalid operator: %c\n", op);
                return -1;
        }
        
    } else {
        // Tree has no operator at the root, so just single command
        // Means that the tree is a single leaf, so just executes the leaf
        return execute_ast(aa, wait);
    }
}

/*
 * Executes the given command line arguments.
 * Takes in a list of strings.
 */
void execute(list* ll)
{
    // Produces a list of tokens
    // AST tree of tokens and commands
    ast* tree = parse(ll);
    // Executing the AST tree:
    execute_ast_tree(tree, 1);
    // Frees the list and AST tree
    free_list(ll);
    free_ast(tree);
}

/*
 * Main to continually take in commands until EOF or exit.
 */
int main(int argc, char* argv[])
{
    char cmd[256];
    // Loops to keep executing lines of commands until EOF or exit
    while (1) {
        if (argc == 1) {
            printf("nush$ ");
            int flush = fflush(stdout);
            if (flush != 0) {
                exit(0);
            }
            char* line = fgets(cmd, 256, stdin);
            if (!line) {
                break;
            }
            
            // List to put the tokens in
            list* xs = make_list();
            tokenize(xs, line);
            execute(xs);
        }
        else {
            // Reading from file
            // Taken from main of length-sort homework
            FILE* fh = fopen(argv[1], "r");
            if (!fh) {
                perror("open failed");
                return 1;
            }
            while (1) {
                char* line = fgets(cmd, 256, fh);
                if (!line) {
                    return 0;
                }
                
                // List to put the tokens in
                list* xs = make_list();
                tokenize(xs, line);
                execute(xs);
            }
        }
    }
    
    return 0;
}
