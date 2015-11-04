/***************************************************************************/
/*                                                                         */
/*                                output.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-11-04.   */
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
        FILE *results_file = fopen(settings.output_file, "w");
        if (!results_file)
            log_event(FATAL, "Failed to open output file %s.", settings.output_file);
        RESULT_ITEM *res = results.first;
        // We already know there's at least one: that's how we got here
        do{
            fprintf(results_file, "Found instance of '%s' in line %d, col %d of %s.\n",
                settings.search_string, res->line_num, res->col_num, res->file_path);
            res = res->next;
        } while (res);
        printf("The matches have been stored in %s.\n", settings.output_file);
        fclose(results_file);
    }
    else
        puts("No matches were found.");
}
