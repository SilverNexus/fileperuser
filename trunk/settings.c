/***************************************************************************/
/*                                                                         */
/*                               settings.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-21.   */
/*                                                                         */
/* The file defines the functions for handling settings.                   */
/*                                                                         */
/***************************************************************************/

#include "settings.h"
#include <stdlib.h>
#include "ErrorLog.h"

int init_settings(){
    settings.excluded_directories = 0;
    return 0;
}

int free_settings(){
    while (settings.excluded_directories){
        struct exclude_dir *tmp = settings.excluded_directories;
        settings.excluded_directories = settings.excluded_directories->next;
        free(tmp);
    }
    settings.excluded_directories = 0;
    return 0;
}
int add_exclude_dir(char *dir){
    struct exclude_dir *newExclusion = (struct exclude_dir *)malloc(sizeof(struct exclude_dir));
    if (!newExclusion){
        logError(ERROR, "Could not allocate space for excluded directory.");
        return -1;
    }

    /*
     * Since dir is an argument passed into the program, in should stay allocated for
     * the duration of the program. Thus, just assign the reference in exclude_dir.
     */
    newExclusion->excludeDir = dir;
    newExclusion->next = settings.excluded_directories;
    settings.excluded_directories = newExclusion;
    return 0;
}
