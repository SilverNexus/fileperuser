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
 * @file fileperuser_search.c
 * Defines the search functions for use when needed.
 * 
 * The case-insensitive search is faster than strcasestr(), so it will always be used.
 *
 * The case-sensitive search will only be used when we cannot null-terminate the haystack
 * without trampling a potential match, since strstr() is faster when needle_len > 1.
 * Since this only occurs from using mmap, it is not declared or implemented when
 * FilePeruser isn't compiled with mmap support.
 */

#include <ctype.h>
#include <string.h>
#include "fileperuser_search.h"
#include "jump_table.h"

/**
 * Finds a substring in a block of memory, ignoring case.
 *
 * @param haystack
 * The block of memory to find the substring in.
 *
 * @param haystack_last
 * The last character of haystack we need to worry about.
 *
 * @param needle
 * The substring to search for. Must be converted to lowercase before
 * reaching this function.
 *
 * @param needle_len
 * The length of the search string.
 *
 * @return
 * Pointer to the first character in haystack of a match to needle, or 0 if it was not found.
 */
char *fileperuser_memcasemem_boyer(char *haystack, const char * const haystack_last, char *needle, size_t needle_len){
	const char needle_last = needle[needle_len - 1];
	size_t at;
	while (haystack < haystack_last){
		if (tolower(haystack[needle_len - 1]) == needle_last){
			at = needle_len - 2;
			// This becomes false at unsigned rollover
			while (at < needle_len){
				if (needle[at] != tolower(haystack[at]))
					break;
				--at;
			}
			// Unsigned integer abuse
			if (at > needle_len)
				return haystack;
		}
		// This already is set up to handle either case, so just drop it in
		haystack += jump_tbl[(unsigned char)haystack[needle_len - 1]];
	}
	return 0;
}

/**
 * Finds a character in a block of memory, ignoring case.
 * Works only when needle has a length of 1.
 *
 * @param haystack
 * The block of memory to find the substring in.
 *
 * @param haystack_last
 * The last character of haystack we need to worry about.
 *
 * @param needle
 * The character to search for. Must be converted to lowercase before
 * reaching this function.
 *
 * @return
 * Pointer to the first character in haystack of a match to needle, or 0 if it was not found.
 */
char *fileperuser_memcasechr(char *haystack, const char * const haystack_last, const char needle){
	while (haystack < haystack_last){
		if (tolower(*haystack) == needle)
			return haystack;
		++haystack;
	}
	return 0;
}

#ifdef HAVE_MMAP
/**
 * Finds a substring in a block of memory.
 *
 * @param haystack
 * The block of memory to search.
 *
 * @param haystack_len
 * The length of the block of memory to search.
 *
 * @param needle
 * The string we wish to find in the block of memory.
 *
 * @param needle_len
 * The length of the string we wish to find.
 *
 * @return
 * Pointer to the first match of needle in haystack, or 0 if not found.
 */
char *fileperuser_memmem_boyer(char *haystack, size_t haystack_len, char *needle, size_t needle_len){
	if (haystack_len < needle_len)
		return 0;
	// Boyer-Moore search
	size_t at = needle_len - 1, c_at, ch;
	/*
	 * Since at < needle_len on the first run, and
	 * haystack_len >= needle_len, checking on the first loop
	 * is superfluous.
	 */
	do {
		if (needle[needle_len - 1] == haystack[at]){
			// Do a backward search
			// Unsigned integer abuse
			for (c_at = at - 1, ch = needle_len - 2; ch < needle_len; --ch, --c_at){
				if (needle[ch] != haystack[c_at])
					break;
			}
			// Abuse unsigned integers.
			if (ch > needle_len)
				/*
				 * We can use c_at instead to make this use less math.
				 * We add 1 since we exit the loop at the unsigned equivalent of -1 relative to
				 * the start of our seached section.
				 * This puts us back to the beginning of the searched section.
				 */
				return haystack + c_at + 1;
			// Move the jump so it aligns with the next letter in the needle that matches this.
			// Fall through to the same code as otherwise
		}
		at += jump_tbl[(unsigned char)haystack[at]];
	} while (at < haystack_len);
	return 0;
}

/**
 * Finds a substring in a block of memory.
 *
 * @param haystack
 * The block of memory to search.
 *
 * @param haystack_len
 * The length of the block of memory to search.
 *
 * @param needle
 * The string we wish to find in the block of memory.
 *
 * @param needle_len
 * The length of the string we wish to find.
 *
 * @return
 * Pointer to the first match of needle in haystack, or 0 if not found.
 *
 * @note
 * This is faster than boyer-moore with really small needles (<= ~6 chars), likely because it foregoes the setup time of boyer-moore
 */
char *fileperuser_memmem_brute(char *haystack, size_t haystack_len, char *needle, size_t needle_len){
	if (haystack_len < needle_len)
		return 0;
	// Find the first place where the first character of needle matches in haystack
	char *at = haystack;
	// Make haystack_left be the amount of the haystack needed to be checked in memchr.
	size_t n_at, haystack_left = haystack_len - needle_len + 1;
	while ((at = memchr(at, *needle, haystack_left)) != 0){
		// Comparison on zero is cheaper
		// Also, since memchr told us that the first character matches, we don't have to check again.
		for (n_at = needle_len - 1; n_at > 0; --n_at){
			if (at[n_at] != needle[n_at])
				break;
		}
		if (n_at == 0)
			return at;
		++at;
		haystack_left = haystack_len - (at - haystack) - needle_len + 1;
		// If haystack is zero or rolled over, then we're done
		if (!haystack_left || haystack_left > haystack_len)
			break;
	}
	return 0;
}
#endif
