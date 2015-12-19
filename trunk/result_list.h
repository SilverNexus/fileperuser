/***************************************************************************/
/*                                                                         */
/*                             result_list.h                               */
/* Original code written by Daniel Hawkins. Last modified on 2015-06-03.   */
/*                                                                         */
/* Defines the structures for storing results in a linked list.            */
/*                                                                         */
/***************************************************************************/

#ifndef RESULT_LIST_H
#define RESULT_LIST_H

typedef struct result_item{
    // At this point, we don't need to store search string,
    // as we have that stored in settings
    int line_num;
    int col_num;
    char *file_path;
    struct result_item *next;
} RESULT_ITEM;

struct result_list {
    RESULT_ITEM *first;
    RESULT_ITEM *last;
} results;

/** Functions to operate on the result list */

void init_results();

int add_result(int line, int col, const char *file);

void clear_results();

#endif