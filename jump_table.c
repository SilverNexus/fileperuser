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
 * @file jump_table.c
 * Defines functions for dealing with the Boyer-Moore serch's jump table.
 */

#include "jump_table.h"
#include <ctype.h>
#include <stdlib.h>
#include "settings.h"

size_t jump_tbl[256];
size_t needle_len;

/**
 * Set up the jump table from the needle
 *
 * @note
 * The needle and needle_len are defined in accessible scopes, so just use those.
 * Yes, that means it is assumed those are already set.
 * In the case of case insensitive searches, it is also assumed that the needle
 * has been converted to entirely lowercase.
 */
void setup_jump_table(){
	// If case insensitive and needle_len > 3, then make a Boyer-Moore jump table
	if (settings.search_flags & FLAG_NO_CASE){
		if (needle_len > MIN_JUMP_TABLE_NO_CASE){
			size_t i = 256;
			// Initialize
			// Comparing to zero is faster than comparing to 256
			do {
				/*
				 * Decrement first, then index.
				 * In this way, we can check on 256 -> 1,
				 * but affect indexes 255 -> 0.
				 */
				jump_tbl[--i] = needle_len;
			} while (i);
			// Now adjust for the characters in the needle, except the last one.
			/**
			 * By ignoring the last character of needle, we can ensure that a failed match will jump based on
			 * the next-to-last occurrence of the last character in needle, as opposed to jumping smaller.
			 */
			for (i = 0; i < needle_len - 1; ++i){
				jump_tbl[(unsigned char)settings.search_string[i]] = needle_len - i - 1;
				jump_tbl[toupper(settings.search_string[i])] = needle_len - i - 1;
			}
		}
	}
	// If case sensitive and needle_len > 6, then make a Boyer-Moore jump table
	else{
		if (needle_len > MIN_JUMP_TABLE_CASE){
			size_t i = 256;
			// Initialize
			// Comparing to zero is faster than comparing to 256,
			// and we know it is not starting at zero.
			do {
				/*
				 * Decrement first, then index.
				 * In this way, we can check on 256 -> 1,
				 * but affect indexes 255 -> 0.
				 */
				jump_tbl[--i] = needle_len;
			} while (i);
			// Now adjust for the characters in the needle, except the last one.
			for (i = 0; i < needle_len - 1; ++i){
				jump_tbl[(unsigned char)settings.search_string[i]] = needle_len - i - 1;
			}
		}
	}
}
