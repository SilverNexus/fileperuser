/*
    FilePeruser, a recursive file search utility.
    Copyright (C) 2014-2016  SilverNexus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file parseArgs.c
 * The file defines the argument parsing functions.
 */

#include "parseArgs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ErrorLog.h"
#include "settings.h"
#include "search.h"
#include <ctype.h>
#include "config.h"
#include "fileperuser_search.h"

/**
 * Macro for checking the next argument for existence with
 * argument-value pairs.
 *
 * @param flagArgs
 * The array of c-strings that contains the command-line args.
 *
 * @param parseCount
 * The index of the string we are parsing.
 *
 * @param errMsg
 * The error message to display when a required value is not found.
 *
 * @warning errMsg must have 1 %s in it for the macro to work.
 * The argument at flagArgs[parseCount - 1] will be printed at %s.
 *
 * @warning This macro also increments the value of parseCount.
 */
#define CHECK_NEXT_ARG(flagArgs, parseCount, errMsg) \
    if (++parseCount == flagCount){ \
        log_event(ERROR, errMsg, flagArgs[parseCount - 1]); \
        return -1; \
    }

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
	    // TODO: Make all the single character checks before the strcmp calls.
            if (*cur_flag == 'h' || strcmp(cur_flag, "-help") == 0){
                help_message();
            }
            else if (*cur_flag == 'x' || strcmp(cur_flag, "-exclude") == 0){
                CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a directory to exclude.");
                add_exclude_dir(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'X' || strcmp(cur_flag, "-exclude-path") == 0){
                CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a path to exclude.");
                add_exclude_path(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'd' || strcmp(cur_flag, "-dir") == 0){
		CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a root directory.");
                add_root_dir(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'o' || strcmp(cur_flag, "-output") == 0){
		CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a file name.");
                settings.output_file = flagArgs[parseCount];
            }
            else if (*cur_flag == 'f' || strcmp(cur_flag, "-log-file") == 0){
		CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a file name.");
                settings.log_file = flagArgs[parseCount];
            }
            else if (*cur_flag == 'l' || strcmp(cur_flag, "-loglevel") == 0){
		CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a log level.");
		// If value is not numeric, atoi returns zero. Oh well. It works.
                settings.min_log_level = atoi(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'p' || strcmp(cur_flag, "-printlevel") == 0){
		CHECK_NEXT_ARG(flagArgs, parseCount, "%s flag needs a print level.");
		// If value is not numeric, atoi returns zero. Oh well. It works.
                settings.min_print_level = atoi(flagArgs[parseCount]);
            }
            else if (*cur_flag == 'n' || strcmp(cur_flag, "-no-case") == 0){
		settings.search_flags |= FLAG_NO_CASE;
            }
	    else if (*cur_flag == '1' || strcmp(cur_flag, "-single-match") == 0){
                settings.file_parser = search_file_single_match;
	    }
            else{
                log_event(WARNING, "Invalid flag '%s' detected, skipping.", flagArgs[parseCount]);
            }
        }
        else{
            if (settings.search_string){
                log_event(WARNING, "Trying to set search string to '%s' when it is already '%s'.",
                    flagArgs[parseCount], settings.search_string);
            }
            else{
                settings.search_string = flagArgs[parseCount];
            }
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
    puts("  fileperuser <flags> <search phrase>\n");
    puts("Valid Flags:");
    puts("  -h --help                 Prints this help message.");
    puts("  -d --dir [directory]      Sets root directory of the search.");
    puts("  -x --exclude [directory]  Excludes [directory] from the search.");
    puts("  -X --exclude-path [path]  Excludes [path] from the search. Can be a specific file.");
    puts("  -o --output [filename]    Sets the name (and path) of the output file. Default prints to stdout.");
    puts("  -f --log-file [filename]  Sets the name (and path) of the log file. Default is fileperuser.log.");
    puts("  -l --loglevel [level]     Sets the minimum log level to be recorded to file. Must be an integer. Default is 2 (WARNING).");
    puts("  -p --printlevel [level]   Sets the minimum log level to be displayed on-screen. Must be an integer. Default is 2 (WARNING).");
    puts("  -n --no-case              Sets the search to be case insensitive.");
    puts("  -1 --single-match         Match the search string at most once per line.");
    exit(EXIT_SUCCESS);
}
