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
 * Last Modified 2016-01-28 by Daniel Hawkins
 *
 * Defines the search functions for use when needed.
 * 
 * The case-insensitive search is faster than strcasestr(), so it will always be used.
 *
 * The case-sensitive search will only be used when we cannot null-terminate the haystack
 * without trampling a potential match, since strstr() is faster when needle_len > 1.
 */

#include <ctype.h>
#include <string.h>
#include "fileperuser_search.h"
#include "settings.h"

/**
 * Finds a substring in a block of memory, ignoring case.
 *
 * @param haystack
 * The block of memory to find the substring in.
 *
 * @param haystack_len
 * The length of the block of memory to search.
 *
 * @param needle
 * The substring to search for.
 *
 * @param needle_len
 * The length of the search string.
 *
 * @param jump
 * The jump table for Boyer-Moore searching. Only used if needle_len > 3.
 *
 * @return
 * Pointer to the first character in haystack of a match to needle, or 0 if it was not found.
 */
char *fileperuser_memcasemem(char *haystack, size_t haystack_len, char *needle, size_t needle_len, const size_t * const jump){
    if (haystack_len < needle_len)
	return 0;
    if (needle_len > MIN_JUMP_TABLE_NO_CASE){
	const char * const haystack_last = haystack + haystack_len - needle_len + 1;
	const char needle_last = tolower(needle[needle_len - 1]);
	size_t at;
	while (haystack < haystack_last){
	    if (tolower(haystack[needle_len - 1]) == needle_last){
		if (needle_len == 1)
		    return haystack;
		at = needle_len - 2;
		// This becomes false at unsigned rollover
		while (at < needle_len){
		    if (tolower(needle[at]) != tolower(haystack[at]))
			break;
		    --at;
		}
		if (at > needle_len)
		    return haystack;
	    }
	    // This already is set up to handle either case, so just drop it in
	    haystack += jump[(unsigned char)haystack[needle_len - 1]];
	}
	return 0;
    }
    else if (needle_len > 1){
	haystack_len -= needle_len - 1;
	size_t at;
	while (haystack_len > 0){
	    if (tolower(*haystack) == tolower(*needle)){
		at = 1;
		while (at < needle_len){
		    if (tolower(haystack[at]) != tolower(needle[at]))
			break;
		    ++at;
		}
		if (at == needle_len)
		    return haystack;
	    }
	    ++haystack;
	    --haystack_len;
	}
	return 0;
    }
    else{
	while (haystack_len > 0){
	    if (tolower(*haystack) == tolower(*needle))
		return haystack;
	    ++haystack;
	    --haystack_len;
	}
	return 0;  
    }
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
 * @param jump
 * The Boyer-Moore jump table. Only used if needle_len > 6.
 *
 * @return
 * Pointer to the first match of needle in haystack, or 0 if not found.
 */
char *fileperuser_memmem(char *haystack, size_t haystack_len, char *needle, size_t needle_len, const size_t * const jump){
    if (haystack_len < needle_len)
	return 0;
    if (needle_len > MIN_JUMP_TABLE_CASE){
	// Boyer-Moore search
	size_t at = needle_len - 1, c_at, ch;
	while (at < haystack_len){
	    if (needle[needle_len - 1] == haystack[at]){
		// Do a backward search
		// Unsigned integer abuse
		for (c_at = at - 1, ch = needle_len - 2; ch < needle_len; --ch, --c_at){
		    if (needle[ch] != haystack[c_at])
			break;
		}
		// Unsigned integer abuse
		if (ch > needle_len){
		    /** - (needle_len - 1) == - needle_len + 1 */
		    return haystack + at - needle_len + 1;
		}
		// Move the jump so it aligns with the next letter in the needle that matches this.
		// Fall through to the same code as otherwise
	    }
	    at += jump[(unsigned char)haystack[at]];
	}
	return 0;
    }
    // This is faster than boyer-moore with really small needles (<= ~6 chars), likely because it foregoes the setup time of boyer-moore
    else if (needle_len > 1){
	// Find the first place where the first character of needle matches in haystack
	char *at = haystack;
	// Make haystack_left be the amount of the haystack needed to be checked in memchr.
	size_t n_at, haystack_left = haystack_len - needle_len + 1;
	while ((at = memchr(at, *needle, haystack_left)) != 0){
	    for (n_at = 1; n_at < needle_len; ++n_at){
		if (at[n_at] != needle[n_at])
		    break;
	    
	    }
	    if (n_at == needle_len)
		return at;
	    ++at;
	    haystack_left = haystack_len - (at - haystack) - needle_len + 1;
	    // If haystack is zero or rolled over, then we're done
	    if (!haystack_left || haystack_left > haystack_len)
		break;
	}
	return 0;
    }
    else{
	// This is far faster than what I was writing
	return memchr(haystack, *needle, haystack_len);
    }
}

/**
 * Wrapper for strstr so it can still be used when possible.
 *
 * @param haystack
 * The string we will search in. Must be null-terminated.
 * 
 * @param needle
 * The string we will search for.
 *
 * @return pointer the the beginning of the first match of needle in haystack, or 0 if not found.
 */
char *strstr_wrapper(char *haystack, size_t irrelevant, char *needle, size_t not_needed, const size_t * const unused){
    return strstr(haystack, needle);
}
