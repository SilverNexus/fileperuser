/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2014-06-11.   */
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
#include <time.h>

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
        // If no arguments set the root directory, error out
        if (!settings.root_dirs)
            log_event(FATAL, "No root directory specified in search.");
        // Remove any existing results file by this name
        remove(settings.output_file);
        // Time the search -- start timing
        time_t start_time = time(0);
        // Begin the search
        DIR_LIST *thisDir = settings.root_dirs;
        while (thisDir){
            searchFolder(thisDir->dir);
            thisDir = thisDir->next;
        }
        // end timing
        time_t end_time = time(0);
        // Don't give the logger a chance to repress this message, so just print from here
        printf("Search completed in %i seconds.\n", (int)(end_time - start_time));
        printf("The matches have been stored in %s.\n", settings.output_file);
        exit(EXIT_SUCCESS);
    }
    else{
        help_message();
    }
    return 0;
}
