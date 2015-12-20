/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2015-12-19.   */
/*                                                                         */
/* The file defines the main function and several searching functions.     */
/*                                                                         */
/***************************************************************************/

#if defined HAVE_NFTW
#include <ftw.h>
#endif

#include <stdio.h>
#include "search.h"
#include "ErrorLog.h"
#include "parseArgs.h"
#include "settings.h"
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include "result_list.h"
#include "output.h"

static void handle_sigint(int sig){
    puts("\nSIGINT Received. Dumping current results to file.");
    output_matches();
    exit(sig);
}

int main(int argc, char *argv[]){
    if (argc >= 3){
        init_settings();
        init_results();
        // Add a signal handler for sigint.
        // When this happens, output what we have.
        signal(SIGINT, handle_sigint);
        // Register free_settings to clean up at exit
        atexit((void *)free_settings);
        // Register clear_results as a cleanup operation
        atexit((void *)clear_results);
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
        time_t start_time = time(0), end_time;
        // Begin the search
        DIR_LIST *thisDir = settings.root_dirs;
        while (thisDir){
#if defined HAVE_NFTW
            if (nftw(thisDir->dir, onWalk, 20, FTW_ACTIONRETVAL | FTW_PHYS) == -1)
                log_event(FATAL, "Directory walk for %s failed!", thisDir->dir);
#elif defined HAVE_DIRENT_H
	    search_folder(thisDir->dir);
#else
#error Could not find a viable directory parsing structure
#endif
            // Okay, we need to reset the base search path after a search tree is completed.
            settings.base_search_path_length = 0;
            thisDir = thisDir->next;
        }
        // end timing
        end_time = time(0);
        // Don't give the logger a chance to repress this message, so just print from here
        fprintf(stderr, "Search completed in %i seconds.\n", (int)(end_time - start_time));
        output_matches();
    }
    else{
        help_message();
    }
    return 0;
}
