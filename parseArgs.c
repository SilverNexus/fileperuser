/***************************************************************************/
/*                                                                         */
/*                              parseArgs.c                                */
/* Original code written by Daniel Hawkins. Last modified on 2016-01-10.   */
/*                                                                         */
/* The file defines the argument parsing functions.                        */
/*                                                                         */
/***************************************************************************/

#include "parseArgs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ErrorLog.h"
#include "settings.h"
#include "search.h"
#include <ctype.h>
#include "config.h"

/**
 * Parses flags that alter program behavior
 *
 * @param flagArgs The flag arguments to parse
 *
 * @param flagCount The number of flags to parse.
 * Includes variables for specific flags in the count.
 *
 * @retval 0 Flags are correct and intend execution.
 *
 * @retval -1 Invalid flags detected.
 */
int parseArgs(char **flagArgs, int flagCount){
    char *cur_flag;
    // Parse each argument that shows up
    for (register int parseCount = 0; parseCount < flagCount; ++parseCount){
        // Don't check everything if the first letter doesn't match -- we always start w/ a dash
        if (*flagArgs[parseCount] == '-'){
            // Optimize out the initial dash -- this reduces the calls to strcmp.
            cur_flag = flagArgs[parseCount] + 1;
            if (*cur_flag == 'h' || strcmp(cur_flag, "-help") == 0){
                help_message();
            }
            else if (*cur_flag == 'x' || strcmp(cur_flag, "-exclude") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a directory to exclude.", flagArgs[parseCount - 1]);
                    return -1;
                }
                add_exclude_dir(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'X' || strcmp(cur_flag, "-exclude-path") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a path to exclude.", flagArgs[parseCount - 1]);
                    return -1;
                }
                add_exclude_path(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'd' || strcmp(cur_flag, "-dir") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a root directory.", flagArgs[parseCount - 1]);
                    return -1;
                }
                add_root_dir(flagArgs[parseCount]);
            }
            else if (*cur_flag == 's' || strcmp(cur_flag, "-search") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a search string.", flagArgs[parseCount - 1]);
                    return -1;
                }
                if (settings.search_string){
                    log_event(WARNING, "Trying to set search string to '%s' when it is already '%s'.",
                        flagArgs[parseCount], settings.search_string);
                }
                else{
                    settings.search_string = flagArgs[parseCount];
                }
            }
            else if (*cur_flag == 'o' || strcmp(cur_flag, "-output") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a file name.", flagArgs[parseCount - 1]);
                    return -1;
                }
                settings.output_file = flagArgs[parseCount];
            }
            else if (*cur_flag == 'f' || strcmp(cur_flag, "-log-file") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a file name.", flagArgs[parseCount - 1]);
                    return -1;
                }
                settings.log_file = flagArgs[parseCount];
            }
            else if (*cur_flag == 'l' || strcmp(cur_flag, "-loglevel") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a log level.", flagArgs[parseCount - 1]);
                    return -1;
                }
                // Make sure the value is numeric
                int checkNum = strlen(flagArgs[parseCount]);
                for (int check = 0; check < checkNum; ++check){
                    if (!isdigit(flagArgs[parseCount][check])){
                        log_event(ERROR, "%s flag expected numeric log level, got %s.", flagArgs[parseCount - 1], flagArgs[parseCount]);
                        return -1;
                    }
                }
                settings.min_log_level = atoi(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'p' || strcmp(cur_flag, "-printlevel") == 0){
                if (++parseCount == flagCount){
                    log_event(ERROR, "%s flag needs a print level.", flagArgs[parseCount - 1]);
                    return -1;
                }
                // Make sure the value is numeric
                int checkNum = strlen(flagArgs[parseCount]);
                for (int check = 0; check < checkNum; ++check){
                    if (!isdigit(flagArgs[parseCount][check])){
                        log_event(ERROR, "%s flag expected numeric print level, got %s.", flagArgs[parseCount - 1], flagArgs[parseCount]);
                        return -1;
                    }
                }
                settings.min_print_level = atoi(flagArgs[parseCount]);
            }
#ifdef HAVE_STRCASESTR
            else if (*cur_flag == 'n' || strcmp(cur_flag, "-no-case") == 0){
                settings.comp_func = strcasestr;
            }
#endif
	    else if (*cur_flag == '1' || strcmp(cur_flag, "-single-match") == 0){
                settings.file_parser = search_file_single_match;
	    }
            else{
                log_event(WARNING, "Invalid flag '%s' detected, skipping.", flagArgs[parseCount]);
            }
        }
        else{
            log_event(WARNING, "Invalid flag '%s' detected, skipping.", flagArgs[parseCount]);
        }
    }
    return settings.search_string ? 0 : -1;
}

/**
 * Prints the help message for the program and exits.
 *
 * @warning This function exits the program at the end,
 * so don't call it if you intend to still continue the program.
 */
void help_message(){
    puts("Usage:");
    puts("  fileperuser -s [search string] -d [directory] <flags>\n");
    puts("Required Arguments:");
    puts("  -d --dir [directory]      Sets root directory of the search.");
    puts("  -s --search [phrase]      Sets the string to be searched.\n");
    puts("Valid Flags:");
    puts("  -h --help                 Prints this help message.");
    puts("  -x --exclude [directory]  Excludes [directory] from the search.");
    puts("  -X --exclude-path [path]  Excludes [path] from the search. Can be a specific file.");
    puts("  -o --output [filename]    Sets the name (and path) of the output file. Default prints to stdout.");
    puts("  -f --log-file [filename]  Sets the name (and path) of the log file. Default is fileperuser.log.");
    puts("  -l --loglevel [level]     Sets the minimum log level to be recorded to file. Must be an integer. Default is 2 (WARNING).");
    puts("  -p --printlevel [level]   Sets the minimum log level to be displayed on-screen. Must be an integer. Default is 2 (WARNING).");
#ifdef HAVE_STRCASESTR
    puts("  -n --no-case              Sets the search to be case insensitive.");
#endif
    puts("  -1 --single-match         Match the search string at most once per line.");
    exit(EXIT_SUCCESS);
}
