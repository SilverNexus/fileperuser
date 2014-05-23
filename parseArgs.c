/***************************************************************************/
/*                                                                         */
/*                              parseArgs.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-23.   */
/*                                                                         */
/* The file defines the argument parsing functions.                        */
/*                                                                         */
/***************************************************************************/

#include "parseArgs.h"
#include <string.h>
#include <stdio.h>
#include "ErrorLog.h"
#include "settings.h"

/*
 * Parses flags that alter program behavior
 *
 * @param flagArgs The flag arguments to parse
 *
 * @param flagCount The number of flags to parse.
 * Includes variables for specific flags in the count.
 *
 * @retval 0 Flags are correct and intend execution.
 *
 * @retval 1 Flags are correct, but only show help.
 *
 * @retval -1 Invalid flags detected.
 *
 * @todo Integrate parsing of search directory and search string into this
 */
int parseArgs(char **flagArgs, int flagCount){
    // Parse each argument that shows up
    for (int parseCount = 0; parseCount < flagCount; parseCount++){
        if (strcmp(flagArgs[parseCount], "-h") == 0 ||
            strcmp(flagArgs[parseCount], "--help") == 0){
                puts("Usage:\n");
                puts("\t./MapChecker -s [search string] -d [directory] <flags>\n\n");
                puts("Valid Flags:\n");
                puts("\t-h --help\t\t\t\tPrints this help message.\n");
                puts("\t-x --exclude [directory name]\tExcludes [directory name] from the search.\n");
                puts("\t-d --dir [directory name]\t\tSets root directory of the search.\n");
                puts("\t-s --search [phrase]\t\t\tSets the string to be search.\n");
                puts("\t-o --output [filename]\t\t\tSets the name of the output file. Default is searchResults.txt.\n");
                // If help, don't actually run the program
                return 1;
        }
        else if (strcmp(flagArgs[parseCount], "-x") == 0 ||
            strcmp(flagArgs[parseCount], "--exclude") == 0){
                if (++parseCount == flagCount){
                    logError(ERROR, "%s flag needs a directory to exclude.", flagArgs[parseCount - 1]);
                    return -1;
                }
                add_exclude_dir(flagArgs[parseCount]);
        }
        else if (strcmp(flagArgs[parseCount], "-d") == 0 ||
            strcmp(flagArgs[parseCount], "--dir") == 0){
                if (++parseCount == flagCount){
                    logError(ERROR, "%s flag needs a root directory.", flagArgs[parseCount - 1]);
                    return -1;
                }
                add_root_dir(flagArgs[parseCount]);
        }
        else if (strcmp(flagArgs[parseCount], "-s") == 0 ||
            strcmp(flagArgs[parseCount], "--search") == 0){
                if (++parseCount == flagCount){
                    logError(ERROR, "%s flag needs a search string.", flagArgs[parseCount - 1]);
                    return -1;
                }
                if (settings.search_string){
                    logError(ERROR, "Trying to set search string to '%s' when it is already '%s'.",
                        flagArgs[parseCount], settings.search_string);
                }
                else{
                    settings.search_string = flagArgs[parseCount];
                }
        }
        else if (strcmp(flagArgs[parseCount], "-o") == 0 ||
            strcmp(flagArgs[parseCount], "--output") == 0){
                if (++parseCount == flagCount){
                    logError(ERROR, "%s flag needs a file name.", flagArgs[parseCount - 1]);
                    return -1;
                }
                settings.output_file = flagArgs[parseCount];
        }
        else{
            logError(ERROR, "Invalid flag '%s' detected, skipping.", flagArgs[parseCount]);
        }
    }
    return 0;
}
