#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <bsd/string.h>
#include <string.h>

#include "list.h"
#include "ast.h"

/*
 * Creates an empty abstract syntax tree to hold operators and arguments.
 * Returns the constructed AST.
 */
ast* make_ast() {
    ast* aa = malloc(sizeof(ast));
    aa->op = 0;
    aa->arg1 = 0;
    aa->arg2 = 0;
    aa->root = 0;
    aa->cmd = make_list();
    return aa;
}

/*
 * Frees the given abstract syntax tree recursively.
 * Takes in the AST.
 */
void free_ast(ast* aa) {
    if (aa) {
        // Frees arguments and command
        if (aa->arg1) {
            free_ast(aa->arg1);
        }
        if (aa->arg2) {
            free_ast(aa->arg2);
        }
        if (aa->cmd) {
            free_list(aa->cmd);
        }
        free(aa);
    }
}

/*
 * Creates an abstract syntax tree from the given list of commands.
 * Takes in a list of strings.
 * Returns an abstract syntax tree root.
 */
ast* parse(list* ll) {
    ast* curr = make_ast(); // curr is a new AST 
    ast* result = curr; // Result is currently empty
    list* ops = make_list(); // List of operators
    list_add(ops, ";");
    list_add(ops, "||");
    list_add(ops, "|");
    list_add(ops, "&");
    list_add(ops, "&&");
    list_add(ops, ">");
    list_add(ops, "<");
    // Goes through all the tokens in ll
    char* temp;
    for (int k = 0; k < ll->size; k++) {
        // Operators: ; || | && & < >
        temp = ll->data[k];
        // Checks for an operator
        if (list_contains(ops, temp)) {
            // Makes a parent, with arg1 equal to the topmost AST leaf
            // and arg2 is a new ast leaf
            ast* parent = make_ast();
            if (curr->root) {
                // Root exists for curr, so puts parent at the top of the
                // AST tree
                parent->arg1 = curr->root;
                curr->root->root = parent;
                // Sets the current's root to the topmost root, which is the new
                // parent with the operator
                curr->root = parent;
            } else {
                // No root
                curr->root = parent;
                parent->arg1 = curr;
            }
            parent->op = temp;
            // Creates a new AST leaf for arg2, sets its root to parent
            parent->arg2 = make_ast();
            parent->arg2->root = parent;
            // Sets the current to the new AST leaf
            curr = parent->arg2;
            // Sets the result we return to the parent
            result = parent;
        } else {
            // Just adds the argument to the list of commands for the current
            // AST leaf
            list_add(curr->cmd, temp);
        }
        
    }
    free_list(ops);
    
    return result;
}
