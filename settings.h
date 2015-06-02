/***************************************************************************/
/*                                                                         */
/*                               settings.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2015-06-01.   */
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
     * Declare the pointer to strstr or strcasestr out here,
     * so I can easily do case sensitive or case-insensitive searches
     */
    char *(*comp_func)(const char *, const char *);

    char *output_file;
    char *log_file;
    enum errorType min_log_level;
    enum errorType min_print_level;
} settings;

void init_settings();
void free_settings();
int add_exclude_dir(char *);
int add_root_dir(char *);

#endif
