/**
 * @file dir_list.h
 * The file defines the structures for handling sets of directories.
 */

#ifndef DIR_LIST_H
#define DIR_LIST_H

typedef struct dir_list{
    char *dir;
    struct dir_list *next;
} DIR_LIST;

DIR_LIST *init_dir_node(char *);
int link_dir_node(DIR_LIST *, DIR_LIST **);
void free_dir_list(DIR_LIST *);

#endif
