/**
 * @file fileperuser_search.h
 * Last Modified 2016-05-03 by Daniel Hawkins
 *
 * Declares the string search functions.
 */

#ifndef FP_SEARCH_H
#define FP_SEARCH_H
#include "config.h"

char *fileperuser_memcasemem(char *haystack, size_t haystack_len, char *needle, size_t needle_len);

#ifdef HAVE_MMAP
/* This will only be used if we are using mmap. */
char *fileperuser_memmem(char *haystack, size_t haystack_len, char *needle, size_t needle_len);
#endif

char *strstr_wrapper(char *haystack, size_t, char *needle, size_t);

#endif
