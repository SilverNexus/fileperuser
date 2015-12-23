/***************************************************************************/
/*                                                                         */
/*                                output.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-12-23.   */
/*                                                                         */
/* The file implements the output function to be used at termination.      */
/*                                                                         */
/***************************************************************************/

#include "output.h"
#include "result_list.h"
#include "ErrorLog.h"
#include "settings.h"
#include <stdio.h>

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
        // We already know there's at least one: that's how we got here
        do{
            fprintf(results_file, "Found '%s' in line %d, col %d of %s.\n",
                settings.search_string, res->line_num, res->col_num, res->file_path);
            res = res->next;
        } while (res);
        if (results_file != stdout)
            // This can be to stdout because it only prints when we don't print to stdout.
            printf("The matches have been stored in %s.\n", settings.output_file);
        fclose(results_file);
    }
    else
        fputs("No matches were found.\n", stderr);
}
