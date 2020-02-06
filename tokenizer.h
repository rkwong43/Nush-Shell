#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "list.h"

/*
 * Tokenizes a list of strings and splits based on whitespace.
 */

// Splits the given string into tokens and places it into the given list
void tokenize(list* ll, char* text);

#endif
