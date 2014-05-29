/***************************************************************************/
/*                                                                         */
/*                               dir_list.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-28.   */
/*                                                                         */
/* The file defines the structures for handling sets of directories.       */
/*                                                                         */
/***************************************************************************/

#ifndef DIR_LIST_H
#define DIR_LIST_H

struct dir_list{
    char *dir;
    struct dir_list *next;
};

// It seems to be normal to make structure types in all caps in C.
typedef struct dir_list DIR_LIST;

DIR_LIST *init_dir_node(char *);
int link_dir_node(DIR_LIST *, DIR_LIST **);
DIR_LIST *free_dir_node(DIR_LIST *, DIR_LIST *);
void free_dir_list(DIR_LIST *);

#endif
