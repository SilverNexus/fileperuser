/***************************************************************************/
/*                                                                         */
/*                             result_list.c                               */
/* Original code written by Daniel Hawkins. Last modified on 2015-06-03.   */
/*                                                                         */
/* Implements the routines for storing results in a linked list.           */
/*                                                                         */
/***************************************************************************/

#include "result_list.h"
#include <stdlib.h>
#include "ErrorLog.h"
#include <string.h>

/**
 * Initializes the results list to an empty list.
 *
 * Used to ensure results begins with sane values.
 */
void init_results(){
    results.first = 0;
    results.last = 0;
}

/**
 * Adds a result to the end of the result list.
 * Updates the list pointers to point to the front and
 * end of the list.
 *
 * @param line The line number of the result.
 *
 * @param col The column number of the result.
 *
 * @param file The path the the file the result was found in.
 *
 * @retval 0 Successfully added to list.
 *
 * @todo Make void? Any errors in here terminate the entire program.
 */
int add_result(int line, int col, const char *file){
    RESULT_ITEM *item = (RESULT_ITEM *)malloc(sizeof(RESULT_ITEM));
    if (!item)
        log_event(FATAL, "No memory to allocate result in file %s, line %d, col %d.", file, line, col);
    item->file_path = (char *)malloc(sizeof(char) * (strlen(file) + 1));
    if (!item->file_path)
        log_event(FATAL, "No memory to allocate file name in result for file %s, line %d, col %d.", file, line, col);
    strcpy(item->file_path, file);
    item->line_num = line;
    item->col_num = col;
    item->next = 0;
    // Append to the end of the list
    if (!results.last)
        results.last = results.first = item;
    else{
        results.last->next = item;
        results.last = item;
    }
    return 0;
}

/**
 * Clears all entries in the result list.
 *
 * Use this to clean up the dynamically-allocated
 * result list items and their file paths.
 */
void clear_results(){
    RESULT_ITEM *res = results.first;
    RESULT_ITEM *tmp;
    while (res){
        free(res->file_path);
        tmp = res;
        res = res->next;
        free(tmp);
    }
}
