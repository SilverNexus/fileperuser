/**
 * @file dir_list.h
 * The file defines the structures for handling sets of directories.
 */

#ifndef DIR_LIST_H
#define DIR_LIST_H

typedef struct dir_list{
	const char *dir;
	struct dir_list *next;
} DIR_LIST;

DIR_LIST *init_dir_node(const char *);
/* The use of static 1 tells the compiler that this should be non-null
 * (an array of size 1 or more)
 */
int link_dir_node(DIR_LIST [static 1], DIR_LIST **);
void free_dir_list(DIR_LIST *);

#endif
