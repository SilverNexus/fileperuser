/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-28.   */
/*                                                                         */
/* The file defines the main function and several searching functions.     */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include "search.h"
#include "ErrorLog.h"
#include "parseArgs.h"
#include "settings.h"

int main(int argc, char *argv[]){
    if (argc >= 3){
        init_settings();
        // argv[0] is the executable name, which I don't need.
        int parse_results = parseArgs(argv + 1, argc - 1);
        if (parse_results == -1){
            logError(FATAL, "Invalid arguments discovered.");
            free_settings();
            return 1;
        }
        // If execution was for usage info, exit here
        else if (parse_results == 1){
            free_settings();
            return 0;
        }
        // If no arguments set the root directory, error out
        if (!settings.root_dirs){
            logError(FATAL, "No root directory specified in search.");
            free_settings();
            return 1;
        }
        // Remove any existing results file by this name
        remove(settings.output_file);
        // Begin the search
        DIR_LIST *thisDir = settings.root_dirs;
        while (thisDir){
            searchFolder(thisDir->dir);
            thisDir = thisDir->next;
        }
        printf("The matches have been stored in %s.\n", settings.output_file);
        free_settings();
    }
    else{
        logError(FATAL, "MapChecker requires arguments: ./MapChecker -s <search string> -d <path to folder to search in> [flags].");
        return 1;
    }
    return 0;
}
