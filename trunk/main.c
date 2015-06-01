/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2015-05-30.   */
/*                                                                         */
/* The file defines the main function and several searching functions.     */
/*                                                                         */
/***************************************************************************/

#include <ftw.h>

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
        // Root directory defaults to current directory, so set if not set in args
        if (!settings.root_dirs){
            add_root_dir(".");
            log_event(INFO, "No root directory specified in search, setting to './'.");
        }
        // Remove any existing results file by this name
        remove(settings.output_file);
        // Time the search -- start timing
        time_t start_time = time(0);
        // Begin the search
        DIR_LIST *thisDir = settings.root_dirs;
        while (thisDir){
            if (nftw(thisDir->dir, onWalk, 20, FTW_ACTIONRETVAL | FTW_PHYS) == -1)
                log_event(FATAL, "Directory walk for %s failed!", thisDir->dir);
            thisDir = thisDir->next;
        }
        // end timing
        time_t end_time = time(0);
        // Don't give the logger a chance to repress this message, so just print from here
        printf("Search completed in %i seconds.\n", (int)(end_time - start_time));
        if (fopen(settings.output_file, "r"))
            printf("The matches have been stored in %s.\n", settings.output_file);
        else
            puts("No matches were found.");
        exit(EXIT_SUCCESS);
    }
    else{
        help_message();
    }
    return 0;
}