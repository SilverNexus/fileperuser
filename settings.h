/**
 * @file settings.h
 * The file defines the structures for handling important variables.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dir_list.h"
#include "ErrorLog.h"
#include "config.h"
#include <stddef.h>
#include <stdio.h>

#define FLAG_NONE         0x0000
#define FLAG_NO_CASE      0x0001
#define FLAG_SINGLE_MATCH 0x0002

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

    // Keep track of case sensitivity here for easy access.
    short search_flags;

    char *output_file;

    char *log_file;
    enum errorType min_log_level;
    enum errorType min_print_level;
} settings;

void init_settings();
void free_settings();
void add_exclude_dir(char *);
void add_root_dir(char *);
void add_exclude_path(char *);

#endif
