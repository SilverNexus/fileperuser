/**
 * @file fileperuser_search.h
 * Declares the string search functions.
 */

#ifndef FP_SEARCH_H
#define FP_SEARCH_H
#include "config.h"

char *fileperuser_memcasemem(char *haystack, const char * const haystack_last, char *needle, size_t needle_len);

#ifdef HAVE_MMAP
/* This will only be used if we are using mmap. */
char *fileperuser_memmem(char *haystack, size_t haystack_len, char *needle, size_t needle_len);
#endif

#endif
