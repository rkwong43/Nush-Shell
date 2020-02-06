#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <bsd/string.h>

#include "list.h"


/*
 * Splits the given string into the list of tokens.
 *
 * Takes in a list of strings, and the string to parse.
 */
void tokenize(list* ll, char* text) {
    // Temporary string to build tokens
    // Treated like a string builder
    char* temp = calloc(8, sizeof(char));
    // Starts at capacity of 8
    int capacity = 8;
    int curr = 0;
    // Total length of the line to compute
    int total_length = strlen(text);
    for (int k = 0; k < total_length - 1; k++) {
        // If it is a space, add the temp to the list
        if (isspace(text[k]) && strlen(temp) > 0) {
            list_add(ll, temp);
            // Empties the temporary string
            temp[0] = 0;
            curr = 0;
        } else if (!isspace(text[k])) {
            // Deals with the operators
            int op = 0;
            char* operator;
            switch(text[k]) {
                case '|':
                    if (text[k + 1] == '|') {
                        operator = "||";
                        // To skip the |
                        k++;
                    } else {
                        operator = "|";
                    }
                    op = 1;
                    break;
                case ';':
                    operator = ";";
                    op = 1;
                    break;
                case '&':
                    if (text[k + 1] == '&') {
                        operator = "&&";
                        // To skip the &
                        k++;
                    } else {
                        operator = "&";
                    }
                    op = 1;
                    break;
                default:
                    // Default case is when the char is not an operator
                    // Reallocates space for temp if it needs to be larger
                    if (strlen(temp) + 2 >= capacity) {
                        temp = realloc(temp, capacity * 2 * sizeof(char));
                        capacity *= 2;
                    }
                    // Adds on the single char onto temp
                    temp[curr] = text[k];
                    temp[++curr] = 0;
            }
            // If there was an operator, checks if it is next to a string
            // without a space
            if (op) {
                if (strlen(temp) > 0) {
                    // If the operator is right after a string
                    list_add(ll, temp);
                    // Empties the temporary string
                    temp[0] = 0;
                    curr = 0;
                }
                list_add(ll, operator);
            }
        }
    }
    // Adds the last string if it exists
    if (strlen(temp) > 0) {
        list_add(ll, temp);
    }
    free(temp);
}
