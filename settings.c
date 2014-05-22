/***************************************************************************/
/*                                                                         */
/*                               settings.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-22.   */
/*                                                                         */
/* The file defines the functions for handling settings.                   */
/*                                                                         */
/***************************************************************************/

#include "settings.h"
#include <stdlib.h>
#include "ErrorLog.h"

int init_settings(){
    settings.excluded_directories = 0;
    settings.root_dirs = 0;
    return 0;
}

int free_settings(){
    while (settings.excluded_directories){
        struct dir_list *tmp = settings.excluded_directories;
        settings.excluded_directories = settings.excluded_directories->next;
        free(tmp);
    }
    settings.excluded_directories = 0;
    while (settings.root_dirs){
        struct dir_list *tmp = settings.root_dirs;
        settings.root_dirs = settings.root_dirs->next;
        free(tmp);
    }
    settings.root_dirs = 0;
    return 0;
}
int add_exclude_dir(char *newDir){
    struct dir_list *newExclusion = (struct dir_list *)malloc(sizeof(struct dir_list));
    if (!newExclusion){
        logError(ERROR, "Could not allocate space for excluded directory %s.", newDir);
        return -1;
    }

    /*
     * Since newDir is an argument passed into the program, in should stay allocated for
     * the duration of the program. Thus, just assign the reference in dir_list.
     */
    newExclusion->dir = newDir;
    newExclusion->next = settings.excluded_directories;
    settings.excluded_directories = newExclusion;
    return 0;
}
int add_root_dir(char *newDir){
    struct dir_list *newRoot = (struct dir_list *)malloc(sizeof(struct dir_list));
    if (!newRoot){
        logError(ERROR, "Could not allocate space for root directory %s.", newDir);
        return -1;
    }

    /*
     * Since newDir is an argument passed into the program, in should stay allocated for
     * the duration of the program. Thus, just assign the reference in dir_list.
     */
    newRoot->dir = newDir;
    newRoot->next = settings.root_dirs;
    settings.root_dirs = newRoot;
    return 0;
}
