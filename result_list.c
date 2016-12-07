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
 * @file result_list.c
 * Implements the routines for storing results in a linked list.
 */

#include "result_list.h"
#include <stdlib.h>
#include "ErrorLog.h"
#include <string.h>
#include "config.h"

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
 * Create a new result_loc struct
 *
 * @param line
 * The line the match was found on.
 *
 * @param col
 * The column number of the match.
 *
 * @return
 * The address of the new RESULT_LOC object.
 */
inline static RESULT_LOC *new_result_loc(const int line, const int col){
    RESULT_LOC *res = malloc(sizeof(RESULT_LOC));
    if (!res){
	log_event(FATAL, "No memory available to record a result location.");
    }
    res->line_num = line;
    res->col_num = col;
    // Ensure it is not linking to nowhere.
    res->next = 0;
    return res;
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
 */
void add_result(int line, int col, const char *file){
    // Set up our location -- we need it in both cases.
    RESULT_LOC *res = new_result_loc(line, col);
    // Check to see whether this is a new file we got a result from
    if (results.last && strcmp(results.last->file_path, file) == 0){
	// Add the result to the list
	results.last->locations_last->next = res;
	results.last->locations_last = res;
    }
    else{
	RESULT_ITEM *item = (RESULT_ITEM *)malloc(sizeof(RESULT_ITEM));
	if (!item)
	    log_event(FATAL, "No memory to allocate result in file %s, line %d, col %d.", file, line, col);
#ifdef HAVE_STRDUP
	item->file_path = strdup(file);
#else
	item->file_path = (char *)malloc(sizeof(char) * (strlen(file) + 1));
	if (!item->file_path)
	    log_event(FATAL, "No memory to allocate file name in result for file %s, line %d, col %d.", file, line, col);
	strcpy(item->file_path, file);
#endif
	// Link the result into the list.
	item->locations = res;
	item->locations_last = res;
	item->next = 0;
	// Append to the end of the list
	if (!results.last)
	    results.last = results.first = item;
	else{
	    results.last->next = item;
	    results.last = item;
	}
    }
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
    // Define two temporary locations for freeing the result locations
    RESULT_LOC *tmploc, *tmploc2;
    while (res){
        free(res->file_path);
	// Free the locations, too
	tmploc = res->locations;
	// There will always be at least one result
	do {
	    tmploc2 = tmploc;
	    tmploc = tmploc->next;
	    free(tmploc2);
	} while(tmploc);
        tmp = res;
        res = res->next;
        free(tmp);
    }
}
