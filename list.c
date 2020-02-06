#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <bsd/string.h>
#include <string.h>

#include "list.h"

/*
 * Creates a list of strings with base maximum capacity of 4.
 * Returns the constructed list.
 */
list* make_list() {
    list* ll = malloc(sizeof(list));
    ll->capacity = 4;
    ll->size = 0;
    ll->data = calloc(4, sizeof(char*));
    return ll;
}

/*
 * Frees the given list by freeing its associated data.
 */
void free_list(list* ll) {
    for (int k = 0; k < ll->size; k++) {
        free(ll->data[k]);
    }
    free(ll->data);
    free(ll);
}

/*
 * Pushes the given string to the end of the list.
 */
void put(list* ll, char* kk) {
    int length = strlen(kk);
    ll->data[ll->size] = malloc(1 + length * sizeof(char*));
    strncpy(ll->data[ll->size], kk, 1 + length * sizeof(char*));
    ll->size += 1;
}

/*
 * Grows the given list by doubling its size then placing the given string at the end.
 * Takes in the list to grow and the string to append.
 */
void list_grow(list* ll, char* kk) {
    long new_size = ll->capacity * 2;
    ll->data = realloc(ll->data, 1 + new_size * sizeof(char*));
    ll->capacity = new_size;
    put(ll, kk);
}

/*
 * Adds the string to the end of the given list, increasing capacity if necessary.
 * Takes in the list and a string.
 */
void list_add(list* ll, char* kk) {
    if (ll->size + 1 >= ll->capacity) {
        list_grow(ll, kk);
    } else {
        put(ll, kk);
    }
}

/*
 * Iterates through the list to search for a given string.
 * Takes in the list and the target string to search for.
 * Returns 1 upon finding the string, 0 otherwise.
 */
int list_contains(list* ll, char* kk) {
    for (int k = 0; k < ll->size; k++) {
        if (strcmp(ll->data[k], kk) == 0) {
            return 1;
        }
    }
    return 0;
}

