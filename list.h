#ifndef LIST_H
#define LIST_H

/*
 * Defines a dynamically allocated list.
 */
typedef struct list {
    // Array of character pointers
    char** data;
    // Current size
    long size;
    // Maximum capacity
    long capacity;
} list;

// Makes the list
list* make_list();
// Frees the list
void free_list(list* ll);
// Adds an element, increasing size if necessary
void list_add(list* ll, char* kk);
// Increases size of list by doubling
void list_grow(list* ll, char* kk);
// Returns 1 if the given char* is found inside the list
int list_contains(list* ll, char* kk);

#endif

