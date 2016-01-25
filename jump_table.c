/**
 * @file jump_table.c
 * Last Modified 2016-01-25 by Daniel Hawkins
 *
 * Defines functions for dealing with the Boyer-Moore serch's jump table.
 */

#include "jump_table.h"
#include <ctype.h>
#include "ErrorLog.h"
#include <stdlib.h>
#include "settings.h"

size_t *jump_tbl;
size_t needle_len;

/**
 * Makes sure jump_tbl is initialized.
 * Should be called before any access to jump_tbl
 */
void init_jump_table(){
    jump_tbl = 0;
}

/**
 * Set up the jump table from the needle
 *
 * @note
 * The needle and needle_len are defined in accessible scopes, so just use those.
 * Yes, that means it is assumed those are already set.
 */
void setup_jump_table(){
    // If case insensitive and needle_len > 3, then make a Boyer-Moore jump table
    if (settings.search_flags & FLAG_NO_CASE){
	if (needle_len > MIN_JUMP_TABLE_NO_CASE){
	    jump_tbl = malloc(sizeof(size_t) * 256);
	    if (!jump_tbl)
		log_event(FATAL, "No memory left for jump table.");
	    unsigned short i;
	    // Initialize
	    for (i = 0; i < 256; ++i){
		jump_tbl[i] = needle_len;
	    }
	    // Now adjust for the characters in the needle, except the last one.
	    /**
	     * By ignoring the last character of needle, we can ensure that a failed match will jump based on
	     * the next-to-last occurrence of the last character in needle, as opposed to jumping smaller.
	     */
	    for (i = 0; i < needle_len - 1; ++i){
		jump_tbl[tolower(i)] = needle_len - i - 1;
		jump_tbl[toupper(i)] = needle_len - i - 1;
	    }
	}
    }
    // If case sensitive and needle_len > 6, then make a Boyer-Moore jump table
    else{
	if (needle_len > MIN_JUMP_TABLE_CASE){
	    jump_tbl = malloc(sizeof(size_t) * 256);
	    if (!jump_tbl)
		log_event(FATAL, "No memory left for jump table.");
	    unsigned short i;
	    // Initialize
	    for (i = 0; i < 256; ++i){
		jump_tbl[i] = needle_len;
	    }
	    // Now adjust for the characters in the needle, except the last one.
	    for (i = 0; i < needle_len - 1; ++i){
		jump_tbl[i] = needle_len - i - 1;
	    }
	}
    }
}

/**
 * Free the jump table allocated in setup_jump_table
 *
 * Called on exit with atexit(), so not many other uses for it.
 */
void cleanup_jump_table(){
    if (jump_tbl)
	free(jump_tbl);
    // Set to zero for good measure.
    jump_tbl = 0;
}
