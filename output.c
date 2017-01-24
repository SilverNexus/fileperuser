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
 * @file output.c
 * The file implements the output function to be used at termination.
 */

#include "output.h"
#include "result_list.h"
#include "ErrorLog.h"
#include "settings.h"
#include <stdio.h>
#include <string.h>

void output_matches(){
	if (results.first){
		/*
		 * if we want to print to stdout, print to stdout. Otherwise print to the output file.
		 */
		FILE *results_file;
		if (settings.output_file){
			results_file = fopen(settings.output_file, "w");
			if (!results_file){
				log_event(ERROR, "Failed to open output file %s, using stdout.", settings.output_file);
				results_file = stdout;
			}
		}
		else
			results_file = stdout;
		RESULT_ITEM *res = results.first;
		RESULT_LOC *loc;
		// We already know there's at least one: that's how we got here
		do{
			// Get each location in the result.
			loc = res->locations;
			// We can assert that a result existant at the file level will have at least one result location
			do{
				fprintf(results_file, "Found '%s' in line %d, col %d of %s.\n",
					settings.search_string, loc->line_num, loc->col_num, res->file_path);
				loc = loc->next;
			} while (loc);
			res = res->next;
		} while (res);
		if (results_file != stdout){
			// This can be to stdout because it only prints when we don't print to stdout.
			/*
			 * Even though this is three calls instead of 1, it's computationally
			 * cheaper than one call to printf().
			 */
			fwrite("The matches have been stored in ", 1, 32, stdout);
			fwrite(settings.output_file, 1, strlen(settings.output_file), stdout);
			fwrite(".\n", 1, 2, stdout);
			/*
			 * We didn't explicitly open the file unless we aren't on stdout.
			 */
			fclose(results_file);
		}
	}
	else
		fputs("No matches were found.\n", stderr);
}
