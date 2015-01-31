/***************************************************************************/
/*                                                                         */
/*                               settings.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2015-01-31.   */
/*                                                                         */
/* The file defines the structures for handling important variables.       */
/*                                                                         */
/***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dir_list.h"
#include "ErrorLog.h"
#include <stddef.h>

struct {
    DIR_LIST *excluded_directories;
    DIR_LIST *root_dirs;
    char *search_string;
    /*
     * Declare the length out here to reduce calls to strlen
     * in the search. Before it recalculated with every line
     */
    int search_string_len;

    /*
     * Declare the pointer to strncmp or strncasecmp out here,
      *so I can easily do case sensitive or case-insensitive searches
     */
    int (*comp_func)(const char *, const char *, size_t);

    char *output_file;
    enum errorType min_log_level;
    enum errorType min_print_level;
} settings;

void init_settings();
void free_settings();
int add_exclude_dir(char *);
int add_root_dir(char *);

#endif
