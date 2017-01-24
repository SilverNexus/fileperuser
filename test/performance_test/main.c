/**
 * @file main.c
 * Contains the main point where each of the searches is set up.
 *
 * This file is contained in each test case, using ifdefs to
 * compile to the right code for that test.
 *
 * @author Daniel Hawkins
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#if defined(FP_MEMMEM_BOYER) || \
	defined(FP_MEMMEM_BRUTE) || \
	defined(FP_MEMCASEMEM_BOYER) || \
	defined(FP_MEMCASECHR)
#include "../../fileperuser_search.h"
#endif

#if defined(FP_MEMMEM_BOYER) || \
	defined(FP_MEMCASEMEM_BOYER)
#define BOYER_MOORE
#endif

// Define the test file name in a macro to make it easier to maintain
#define FILENAME "test_file"
// Our test file isn't changing -- just use this value.
#define FILESIZE 4853756

// If using a jump table, define it
// All paths need this defined for now, since the other functions fail
// without it.
size_t jump_tbl[256];
// Keep needle_len global to maintain consistency across tests.
size_t needle_len;
/**
 * The main entry point for the program.
 *
 * @retval 1
 * Search failed.
 *
 * @retval 0
 * Search successful.
 */
int main(int argc, char **argv){
	// Check for a search argument
	if (argc != 2)
		return 1;
	// Reduce future dereferences
	char * const needle = argv[1];
	// Just make the full buffer like we do in the actual program.
	// Make an extra space for the null terminator.
	char * const file_data = (char *)malloc(sizeof(char) * (FILESIZE + 1));
	if (!file_data)
		return 1;
	// Just open the file normally--don't worry about mmap() in this test
	FILE *test_file = fopen(FILENAME, "r");
	if (!test_file)
		return 1;
	size_t res = fread(file_data, 1, FILESIZE, test_file);
	if (res != FILESIZE)
		return 1;
	// Ensure null termination
	file_data[FILESIZE] = '\0';
	needle_len = strlen(needle);
	#if defined(FP_MEMCASEMEM_BOYER) || \
		defined(FP_MEMCASECHR)
	// Convert needle to lower case if case insensitive.
	for (size_t i = 0; i < needle_len; ++i){
		needle[i] = tolower(needle[i]);
	}
	// These are also only used for fp_memcasemem derivatives.
	const char * const last = file_data + FILESIZE - needle_len + 1;
	#endif
	#ifdef BOYER_MOORE
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
		jump_tbl[(unsigned char)needle[i]] = needle_len - i - 1;
		#ifdef FP_MEMCASEMEM_BOYER
		jump_tbl[toupper(needle[i])] = needle_len - i - 1;
		#endif
	}
	#endif
	char *at;
	size_t result_count = 0;
	// Now we do the search.
	#ifdef FP_MEMMEM_BOYER
	at = fileperuser_memmem_boyer(file_data, FILESIZE, needle, needle_len);
	#elif defined(FP_MEMMEM_BRUTE)
	at = fileperuser_memmem_brute(file_data, FILESIZE, needle, needle_len);
	#elif defined(MEMCHR)
	at = memchr(file_data, *needle, FILESIZE);
	#elif defined(MEMMEM)
	at = memmem(file_data, FILESIZE, needle, needle_len);
	#elif defined(STRSTR)
	at = strstr(file_data, needle);
	#elif defined(FP_MEMCASEMEM_BOYER)
	at = fileperuser_memcasemem_boyer(file_data, last, needle, needle_len);
	#elif defined(FP_MEMCASECHR)
	at = fileperuser_memcasechr(file_data, last, *needle);
	#elif defined(STRCASESTR)
	at = strcasestr(file_data, needle);
	#elif defined(STRCHR)
	at = strchr(file_data, *needle);
	#else
	#error You did not choose a valid search function.
	#endif
	while (at){
		// Move to the space beyond the last found.
		++at;
		++result_count;
		#ifdef FP_MEMMEM_BOYER
		at = fileperuser_memmem_boyer(at, FILESIZE - (at - file_data), needle, needle_len);
		#elif defined(FP_MEMMEM_BRUTE)
		at = fileperuser_memmem_brute(at, FILESIZE - (at - file_data), needle, needle_len);
		#elif defined(MEMCHR)
		at = memchr(at, *needle, FILESIZE - (at - file_data));
		#elif defined(MEMMEM)
		at = memmem(at, FILESIZE - (at - file_data), needle, needle_len);
		#elif defined(STRSTR)
		at = strstr(at, needle);
		#elif defined(FP_MEMCASEMEM_BOYER)
		at = fileperuser_memcasemem_boyer(at, last, needle, needle_len);
		#elif defined(FP_MEMCASECHR)
		at = fileperuser_memcasechr(at, last, *needle);
		#elif defined(STRCASESTR)
		at = strcasestr(at, needle);
		#elif defined(STRCHR)
		at = strchr(at, *needle);
		#else
		#error You did not choose a valid search function.
		#endif
	}
	printf("Found %lu results.", result_count);

	fclose(test_file);
	return 0;
}
