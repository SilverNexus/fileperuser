/**
 * @file result_list.h
 * Defines the structures for storing results in a linked list.
 */

#ifndef RESULT_LIST_H
#define RESULT_LIST_H

// By storing results as a list inside the result item, we can reduce copy operations tremendously.
typedef struct result_loc{
	int line_num;
	int col_num;
	struct result_loc *next;
} RESULT_LOC;

// Store the name of the file and a list of the results in that file.
typedef struct result_file{
	// At this point, we don't need to store search string,
	// as we have that stored in settings
	RESULT_LOC *locations;
	RESULT_LOC *locations_last;
	char *file_path;
	struct result_file *next;
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
