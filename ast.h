#ifndef AST_H
#define AST_H

#include "list.h"

/*
 * Defines an abstract syntax binary tree that holds an operator, two arguments, the command to
 * apply to the arguments, and a root.
 */
typedef struct ast {
    // Operator
    char* op;
    // The two arguments for the operator
    struct ast* arg1;
    struct ast* arg2;
    // The command and its following arguments
    // Ex: echo one two = list{"echo", "one", "two"}
    struct list* cmd;
    // The root of the AST tree
    struct ast* root;
} ast;

// Creates an AST tree off of a list of strings
ast* parse(list* ll);
// Frees the AST tree
void free_ast(ast* aa);
// Makes an AST
ast* make_ast();

#endif
