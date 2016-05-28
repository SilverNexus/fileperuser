/**
 * @file fileperuser_search.h
 * Declares the string search functions.
 */

#ifndef FP_SEARCH_H
#define FP_SEARCH_H
#include "config.h"

// Uses Boyer-Moore jump table to search.
char *fileperuser_memcasemem_boyer(char *haystack, const char * const haystack_last, char *needle, size_t needle_len);
// Uses a custom "brute-force" method
char *fileperuser_memcasemem_brute(char *haystack, const char * const haystack_last, char *needle, size_t needle_len);
// Uses a simplified version of the brute-force method. Only works if needle_len = 1.
char *fileperuser_memcasemem_single(char *haystack, const char * const haystack_last, const char needle);

#ifdef HAVE_MMAP
/* This will only be used if we are using mmap. */
char *fileperuser_memmem(char *haystack, size_t haystack_len, char *needle, size_t needle_len);
#endif

#endif
