/***************************************************************************/
/*                                                                         */
/*                               settings.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-21.   */
/*                                                                         */
/* The file defines the structures for handling important variables.       */
/*                                                                         */
/***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

struct exclude_dir{
    char *excludeDir;
    struct exclude_dir *next;
};

struct {
    struct exclude_dir *excluded_directories;
    char *root_dir;
} settings;

int init_settings();
int free_settings();
int add_exclude_dir(char *);

#endif
