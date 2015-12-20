/***************************************************************************/
/*                                                                         */
/*                               dir_list.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2015-12-19.   */
/*                                                                         */
/* The file defines the functions for handling the dir_list structure.     */
/*                                                                         */
/***************************************************************************/

#include "dir_list.h"
#include "ErrorLog.h"
#include <stdlib.h>

/**
 * Initializes an instance of the dir_list structure
 *
 * @param directory The directory this node stores.
 *
 * @return Pointer to the new node.
 */
DIR_LIST *init_dir_node(char *directory){
    DIR_LIST *new_node = (DIR_LIST *)malloc(sizeof(DIR_LIST));
    if (!new_node){
        log_event(FATAL, "Could not allocate space for directory %s", directory);
    }
    new_node->dir = directory;
    new_node->next = 0;
    return new_node;
}

/**
 * Links a node to another node, making a list.
 *
 * @param toLink The node being added to the list.
 *
 * @param current_list Address of the list variable being added to.
 *        The address of list is the first node in the list,
 *        and toLink is assigned to the first node, adjusting
 *        the address of this to match toLink.
 *
 * @retval 0 The operation succeeded.
 * @retval -1 The operation failed.
 */
int link_dir_node(DIR_LIST *toLink, DIR_LIST **current_list){
    if (!toLink){
        log_event(ERROR, "Trying to link nonexistent directory node to current list.");
        return -1;
    }
    toLink->next = *current_list;
    *current_list = toLink;
    return 0;
}

/**
 * Frees a whole list of nodes from memory.
 *
 * @param freeMe Pointer to the head of the list being freed.
 */
void free_dir_list(DIR_LIST *freeMe){
    DIR_LIST *tmp;
    while (freeMe){
        tmp = freeMe;
        freeMe = freeMe->next;
        free(tmp);
    }
    // By the end, freeMe already points to 0, so don't do it again.
}
