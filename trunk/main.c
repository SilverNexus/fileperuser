/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2014-06-03.   */
/*                                                                         */
/* The file defines the main function and several searching functions.     */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include "search.h"
#include "ErrorLog.h"
#include "parseArgs.h"
#include "settings.h"
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (argc >= 3){
        init_settings();
        // Register free_settings to clean up at exit
        atexit((void *)free_settings);
        // argv[0] is the executable name, which I don't need.
        int parse_results = parseArgs(argv + 1, argc - 1);
        if (parse_results == -1){
            help_message();
        }
        // If execution was for usage info, exit here
        else if (parse_results == 1){
            free_settings();
            return 0;
        }
        // If no arguments set the root directory, error out
        if (!settings.root_dirs)
            log_event(FATAL, "No root directory specified in search.");
        // Remove any existing results file by this name
        remove(settings.output_file);
        // Begin the search
        DIR_LIST *thisDir = settings.root_dirs;
        while (thisDir){
            searchFolder(thisDir->dir);
            thisDir = thisDir->next;
        }
        log_event(INFO, "The matches have been stored in %s.", settings.output_file);
        exit(EXIT_SUCCESS);
    }
    else{
        help_message();
    }
    return 0;
}
