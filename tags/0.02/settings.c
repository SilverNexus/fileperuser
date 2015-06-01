/***************************************************************************/
/*                                                                         */
/*                               settings.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2015-03-10.   */
/*                                                                         */
/* The file defines the functions for handling settings.                   */
/*                                                                         */
/***************************************************************************/

#include "settings.h"
#include "ErrorLog.h"
#include "dir_list.h"
#include <string.h>

/**
 * Initializes the settings to their default values.
 */
void init_settings(){
    settings.excluded_directories = 0;
    settings.root_dirs = 0;
    settings.search_string = 0;
    settings.search_string_len = 0;
    settings.comp_func = strncmp;
    settings.output_file = "searchResults.txt";
    settings.log_file = "fileperuser.log";
    // Only log warning and higher by default
    settings.min_log_level = WARNING;
    settings.min_print_level = WARNING;
    return;
}

/**
 * Frees any dynamically allocated units within settings
 * (e.g. any DIR_LIST varaibles)
 */
void free_settings(){
    free_dir_list(settings.excluded_directories);
    free_dir_list(settings.root_dirs);
    return;
}

/**
 * Adds a directory to the excluded directories list
 *
 * @param newDir The name of the directory to be added.
 *
 * @retval 0 Successfully added directory to the list.
 *
 * @retval -1 The node could not be allocated.
 */
int add_exclude_dir(char *newDir){
    DIR_LIST *newExclusion = init_dir_node(newDir);
    if (!newExclusion){
        log_event(ERROR, "Excluded directory %s not added to list.", newDir);
        return -1;
    }
    link_dir_node(newExclusion, &settings.excluded_directories);
    return 0;
}

/**
 * Adds a directory to the root directory list.
 *
 * @param newDir The directory path to be added.
 *
 * @retval 0 The directory was successfully added.
 *
 * @retval -1 Allocation of directory node failed.
 */
int add_root_dir(char *newDir){
    DIR_LIST *newRoot = init_dir_node(newDir);
    if (!newRoot){
        log_event(ERROR, "Root directory %s not added to list.", newDir);
        return -1;
    }
    link_dir_node(newRoot, &settings.root_dirs);
    return 0;
}
