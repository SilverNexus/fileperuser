/***************************************************************************/
/*                                                                         */
/*                               settings.h                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-22.   */
/*                                                                         */
/* The file defines the structures for handling important variables.       */
/*                                                                         */
/***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

struct dir_list{
    char *dir;
    struct dir_list *next;
};

struct {
    struct dir_list *excluded_directories;
    struct dir_list *root_dirs;
} settings;

int init_settings();
int free_settings();
int add_exclude_dir(char *);
int add_root_dir(char *);

#endif
