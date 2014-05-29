/***************************************************************************/
/*                                                                         */
/*                               settings.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-29.   */
/*                                                                         */
/* The file defines the structures for handling important variables.       */
/*                                                                         */
/***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dir_list.h"
#include "ErrorLog.h"

struct {
    DIR_LIST *excluded_directories;
    DIR_LIST *root_dirs;
    char *search_string;
    char *output_file;
    enum errorType min_log_level;
} settings;

int init_settings();
int free_settings();
int add_exclude_dir(char *);
int add_root_dir(char *);

#endif
