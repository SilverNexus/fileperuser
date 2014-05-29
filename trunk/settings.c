/***************************************************************************/
/*                                                                         */
/*                               settings.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-29.   */
/*                                                                         */
/* The file defines the functions for handling settings.                   */
/*                                                                         */
/***************************************************************************/

#include "settings.h"
#include "ErrorLog.h"
#include "dir_list.h"

int init_settings(){
    settings.excluded_directories = 0;
    settings.root_dirs = 0;
    settings.search_string = 0;
    settings.output_file = "searchResults.txt";
    // Only log warning and higher by default
    settings.min_log_level = WARNING;
    return 0;
}

int free_settings(){
    free_dir_list(settings.excluded_directories);
    free_dir_list(settings.root_dirs);
    return 0;
}

int add_exclude_dir(char *newDir){
    DIR_LIST *newExclusion = init_dir_node(newDir);
    if (!newExclusion){
        log_event(ERROR, "Excluded directory %s not added to list.", newDir);
        return -1;
    }
    link_dir_node(newExclusion, &settings.excluded_directories);
    return 0;
}

int add_root_dir(char *newDir){
    DIR_LIST *newRoot = init_dir_node(newDir);
    if (!newRoot){
        log_event(ERROR, "Root directory %s not added to list.", newDir);
        return -1;
    }
    link_dir_node(newRoot, &settings.root_dirs);
    return 0;
}
