/**
 * @file fileperuser_search.h
 * Last Modified 2016-01-25 by Daniel Hawkins
 *
 * Declares the string search functions.
 */

#ifndef FP_SEARCH_H
#define FP_SEARCH_H

char *fileperuser_memcasemem(char *haystack, size_t haystack_len, char *needle, size_t needle_len, const size_t * const jump);

char *fileperuser_memmem(char *haystack, size_t haystack_len, char *needle, size_t needle_len, const size_t * const jump);

char *strstr_wrapper(char *haystack, size_t, char *needle, size_t, const size_t * const);

#endif
