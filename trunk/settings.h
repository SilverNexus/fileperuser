/***************************************************************************/
/*                                                                         */
/*                               settings.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2016-01-11.   */
/*                                                                         */
/* The file defines the structures for handling important variables.       */
/*                                                                         */
/***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dir_list.h"
#include "ErrorLog.h"
#include "config.h"
#include <stddef.h>
#include <stdio.h>

struct {
    /*
     * List of directory names to skip.
     * Example: excluding ".svn" here would skip
     * all instances of ".svn" in the searched
     * directory tree.
     */
    DIR_LIST *excluded_directories;

    /*
     * List of paths in the search tree to skip.
     * Example: excluding ".svn" here would skip
     * ".svn" in the root of the searched
     * directory tree.
     */
    DIR_LIST *excluded_paths;
    /*
     * Also, store the index of the length of the base search path.
     */
    short base_search_path_length;

    DIR_LIST *root_dirs;
    char *search_string;

    /*
     * Declare the pointer to strstr or strcasestr out here,
     * so I can easily do case sensitive or case-insensitive searches
     */
#ifdef HAVE_STRCASESTR
    char *(*comp_func)(const char *, const char *);
#endif
    
    /*
     * Pointer to the file parsing function
     * one multi-matches, the other single-matches
     */
    void (*file_parser)(char * const, const char * const);

    char *output_file;

    char *log_file;
    enum errorType min_log_level;
    enum errorType min_print_level;
} settings;

void init_settings();
void free_settings();
int add_exclude_dir(char *);
int add_root_dir(char *);
int add_exclude_path(char *);

#endif
