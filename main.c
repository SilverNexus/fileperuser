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
 * @file main.c
 * The file defines the main function and several searching functions.
 */

#include <stdio.h>
#include "search.h"
#include "ErrorLog.h"
#include "parseArgs.h"
#include "settings.h"
#include <stdlib.h>
#include <signal.h>
#include "result_list.h"
#include "output.h"
#include "jump_table.h"
#include <string.h>
#include <ctype.h>

static void handle_sigint(int sig){
	puts("\nSIGINT Received. Dumping current results to file.");
	output_matches();
	exit(sig);
}

int main(int argc, char *argv[]){
	if (argc >= 2){
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
		/* Get this earlier to reduce processing if no search string */
		// Get the length of the needle.
		needle_len = strlen(settings.search_string);
		// If no needle, we can bail now.
		if (!needle_len){
			log_event(ERROR, "Search string has no length.");
			return 1;
		}

		// Root directory defaults to current directory, so set if not set in args
		if (!settings.root_dirs){
			add_root_dir(".");
			log_event(INFO, "No root directory specified in search, setting to './'.");
		}
		// Remove any existing results file by this name
		// but only do that if we are writing to it.
		if (settings.output_file)
			remove(settings.output_file);
		/*
		 * Convert the needle to lowercase if we are doing a case insensitive search.
		 * We do this before the jump table so we can make some assumptions about the
		 * needle in the jump table to save time.
		 */
		if (settings.search_flags & FLAG_NO_CASE){
			/*
			 * Go from end to start, since comparison on 0 is cheaper.
			 *
			 * We also can skip a comparison on i, since we can assert that i > 0.
			 */
			size_t i = needle_len;
			do {
				// There, now we will fix all characters.
				--i;
				settings.search_string[i] = tolower(settings.search_string[i]);
			} while (i);
		}
		// Build the jump table.
		setup_jump_table();
		// Begin the search
		DIR_LIST *thisDir = settings.root_dirs;
		// There will be at least one root directory.
		// We make sure to set to current directory if not specified.
		do{
			// Set the base search path length once, and then don't check for it every time.
			if (settings.excluded_paths){
				size_t len = strlen(thisDir->dir);
				// Only add one if we don't end in a slash.
#ifdef HAVE_IO_H
				settings.base_search_path_length = len + (thisDir->dir[len - 1] == '\\' ? 0 : 1);
#else
				settings.base_search_path_length = len + (thisDir->dir[len - 1] == '/' ? 0 : 1);
#endif
			}
			// Do the file tree walk.
#if defined (HAVE_DIRENT_H) || defined (HAVE_IO_H)
			/** Both dirent- and io- based searches have a wrapper function of the same name. */
			search_folder(thisDir->dir);
#else
#error Could not find a viable directory parsing structure
#endif
			// Okay, we need to reset the base search path after a search tree is completed.
			settings.base_search_path_length = 0;
			thisDir = thisDir->next;
		} while (thisDir);
		output_matches();
	}
	else{
		help_message();
	}
	return 0;
}
