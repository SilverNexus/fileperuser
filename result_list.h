/**
 * @file result_list.h
 * Last modified on 2016-01-11 by Daniel Hawkins.
 *
 * Defines the structures for storing results in a linked list.
 */

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

void add_result(int line, int col, const char *file);

void clear_results();

#endif
