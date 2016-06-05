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
 * @file load_binary_file_cache.c
 * Contains the implementation for handling a binary file cache.
 *
 * @author Daniel Hawkins
 */

// TODO: Maybe in the future we try to do more low-level access?
#include <stdio.h>
#include "ErrorLog.h"
#include "dir_list.h"
#include <stdlib.h>
#include "binary_file_cache.h"
#include <string.h>

/*
 * Define the variables listed as extern in the header file.
 */
DIR_LIST *new_cache_list;
unsigned num_new_files;
char **binary_file_list;
unsigned list_length;

/**
 * Wrapper for strcmp for use in qsort.
 *
 * @param a
 * The first item to compare.
 *
 * @param b
 * The second item to compare.
 *
 * @return
 * < 0 if a < b (earlier asciibetically)
 * 0 if identical
 * > 0 if a > b (later asciibetically)
 */
static int compare_sort(const void *a, const void *b){
    return strcmp(*((const char **)a), *((const char **)b));
}

/**
 * Wrapper for strcmp for use in bsearch.
 *
 * @param a
 * The first item to compare.
 *
 * @param b
 * The second item to compare.
 *
 * @return
 * < 0 if a < b (earlier asciibetically)
 * 0 if identical
 * > 0 if a > b (later asciibetically)
 */
static int compare_search(const void *a, const void *b){
    return strcmp((const char *)a, (const char *)b);
}

/**
 * Initializes the global data for the binary cache.
 * They are set to zero so I don't have to deal with uninitialized space.
 */
void init_binary_cache(){
    new_cache_list = 0;
    num_new_files = 0;
    binary_file_list = 0;
    list_length = 0;
}

/**
 * Loads the binary file list from disk.
 *
 * @param path
 * Where the cache file is located.
 *
 * @retval 0 Load was successful.
 *
 * @retval 1 Load failed. Assume there is no cache.
 */
int read_cache_file(const char * const path){
    FILE *cache_file = fopen(path, "r");
    if (!cache_file){
	log_event(INFO, "Failed to find cache file at %s.", path);
	return 1;
    }
    // The first thing in the file is the number of entries in the list.
    // To reduce conversions, the file stores the length in a binary format.
    /*
     * We shouldn't have to deal with endianness because this shouldn't be
     * migrating between different processors.
     * Processors, such as ARM, that can switch likely have a default one to
     * output to, but I should investigate into that more.
     */
    size_t bytes = fread(&list_length, sizeof(int), 1, cache_file);
    if (bytes < 1){
	log_event(ERROR, "Cache file %s is truncated... skipping.", path);
	fclose(cache_file);
	return 1;
    }
    // Skip the newline after that value
    fseek(cache_file, 1, SEEK_CUR);
    // Set up an array of the binary file paths.
    binary_file_list = malloc(sizeof(char *) * list_length);
    // Check for allocation failure
    if (!binary_file_list){
	// FATAL bails out of the program.
	log_event(FATAL, "Failed to allocate space for binary file list cache.");
    }
    // Now read the file list from the file
    char linebuffer[1000]; // WAY bigger than anything should sensibly be

    // Reduce calls to strlen.
    size_t buf_len;
    // Order matters here -- the output should be sorted for a binary search.
    for (unsigned i = 0; i < list_length; ++i){
	fgets(linebuffer, 1000, cache_file);
	buf_len = strlen(linebuffer);
	// Convert the newline at the end into a null terminator
	linebuffer[buf_len - 1] = '\0';
	// Set up and copy to the binary file list.
	// We don't need to add one for the null terminator because we converted the newline to one,
	// and didn't update the buffer length.
	binary_file_list[i] = malloc(sizeof(char) * buf_len);
	if (!binary_file_list[i]){
	    log_event(FATAL, "Memory allocation failure for file path #%lu (%s).", i + 1, linebuffer);
	}
	// Since we specifically allocated for the result, we can safely use strcpy.
	strcpy(binary_file_list[i], linebuffer);
    }
    // We finished, so close the file and exit
    fclose(cache_file);
    return 0;
}

/**
 * Cleans up the dynamically allocated sections of the cache array.
 */
void cleanup_cache_list(){
    // On cleanup, order does not matter.
    for (unsigned i = list_length; i; ){
	--i; // Turn the count into an array index.
	free(binary_file_list[i]);
    }
    free(binary_file_list);
    // We also need to clean up the directory linked list from new files.
    DIR_LIST *tmp = new_cache_list;
    // TODO: Do both cleanup part on the dir_list in one pass.
    // We are handling the cleanup of the dynamically allocated path list.
    while (tmp){
	// Free the copied directory path.
	free(tmp->dir);
	tmp = tmp->next;
    }
    // Then we just do the normal directory list cleanup.
    free_dir_list(new_cache_list);
}

/**
 * Writes the new cache to file, including any new detected binary files.
 *
 * @param path
 * The path to the cache file.
 *
 * @retval 0 Cache written successfully.
 *
 * @retval 1 Cache write failed.
 */
int save_cache_file(const char *const path){
    // If no new files, skip this step entirely -- the cache should still be there
    if (num_new_files){
	/*
	 * First, count the number of new binary files we found. These are stored in a linked list
	 * from the program. To ease the process, we will stuff them in an array and sort the array.
	 */
	char **new_binary_files = malloc(sizeof(char *) * num_new_files);
	if (!new_binary_files){
	    log_event(FATAL, "Could not make an array to sort the new binary files.");
	}
	// Get the head of the linked list
	DIR_LIST *new_files = new_cache_list;
	// We will use i again later, so declare it outside the loop
	unsigned i = num_new_files;
	// Then we just make pointer assignments to all the paths -- if they made it here, they'll stick around.
	// We want to stop at the end of the linked list and at the end of our allocated space.
	// Since we're sorting them later anyway, it matters little what order they start in.
	while (new_files || i){
	    --i; // Make it an array index.
	    new_binary_files[i] = new_files->dir;
	    new_files = new_files->next;
	}
	if (i){
	    log_event(ERROR, "Binary cache was shorter than expected, skipping addition of new entries.");
	    return 1;
	}
	if (new_files){
	    log_event(ERROR, "Binary cache was longer than expected, skipping addition of new entries.");
	    return 1;
	}
	// Sort them into asciibetical order.
	qsort(new_binary_files, num_new_files, sizeof(char *), compare_sort);
	// Now we begin to rewrite the cache file.
	// We are not simply appending to the file
	/*
	 * It may be worth doing this atomically, but the cache isn't mission critical, so I'll not do that for now.
	 */
	FILE *cache_file = fopen(path, "w");
	if (!cache_file){
	    log_event(ERROR, "Could not open cache file %s for writing.", path);
	    return 1;
	}
	// Get the total length of the list. It should be the length of the old list plus the length of the additions.
	size_t total_len = list_length + num_new_files;
	// First, write the number of entries in binary to the file.
	size_t written = fwrite(&total_len, sizeof(int), 1, cache_file);
	if (written < 1){
	    log_event(ERROR, "Short write occurred on cache file write.");
	    fclose(cache_file);
	    // We broke the cache file, so get rid of it.
	    remove(path);
	    return 1;
	}
	// Write a newline afterward to ease readability.
	fputc('\n', cache_file);
	/*
	 * The i from earlier will be the index in the existing cache list, while j is the index in the new list.
	 */
	unsigned j = 0;
	i = 0;
	// Storage for the result of strcmp
	int cmp_res;
	/*
	 * We start with the section where we have results in both sections.
	 * We can't use an || here, though, because we'll segfault when one reaches the end and not the other.
	 */
	while (i < list_length && j < num_new_files){
	    cmp_res = strcmp(binary_file_list[i], new_binary_files[j]);
	    // If binary_file_list has the next path asciibetically, write it to the file and increment i.
	    if (cmp_res < 0){
		fputs(binary_file_list[i], cache_file);
		++i;
	    }
	    // If new_binary_files has the next path asciibetically, write it to the file and increment j.
	    else if (cmp_res > 0){
		fputs(new_binary_files[j], cache_file);
		++j;
	    }
	    // If they are the same, write the path once and increment both.
	    else{
		// Also, we shouldn't have reached this, so log a warning.
		log_event(WARNING, "New and old file lists had matching path (%s).", binary_file_list[i]);
		fputs(binary_file_list[i], cache_file);
		++i;
		++j;
	    }
	    // I don't believe fputs append a newline like puts.
	    fputc('\n', cache_file);
	}
	// Now we handle when one list runs out.
	if (i == list_length){
	    while(j < num_new_files){
		fputs(new_binary_files[j], cache_file);
		fputc('\n', cache_file);
		++j;
	    }
	}
	// Check the other list, too.
	// We can use an else here since j == num_new_files after we reach the end of the first if clause, anyway.
	else if (j == num_new_files){
	    while(i < list_length){
		fputs(binary_file_list[i], cache_file);
		fputc('\n', cache_file);
		++i;
	    }
	}
	// And we're done.
	fclose(cache_file);
	free(new_binary_files);
    }
    return 0;
}

/**
 * Adds a new binary file to the binary file cache list.
 *
 * @param bin_path
 * The path to the binary file.
 */
void add_new_binary_file(const char * const bin_path){
    // Init_dir_node terminates if allocation fails.
    // I need to dynamically allocate the path somewhere along the way.
    char *path = malloc(sizeof(char) * (strlen(bin_path) + 1));
    if (!path)
	log_event(FATAL, "Out of memory to cache path %s as a binary file.", bin_path);
    // We allocated carefully, so we can use simple strcpy().
    strcpy(path, bin_path);
    DIR_LIST *node = init_dir_node(path);

    link_dir_node(node, &new_cache_list);
    // Make sure we keep track of how many elements are in the list.
    ++num_new_files;
}

/**
 * Check a given path for its presence in the cache as a binary file.
 *
 * @param path
 * The file to check against the cache.
 *
 * @retval 1
 * The file is in the cache.
 *
 * @retval 0
 * The file is not in the cache.
 */
int check_path(const char * const path){
    // Do a binary search on the array of cached files.
    // Return 1 on a non-zero result.
    return bsearch(path, binary_file_list, list_length, sizeof(char *), compare_search) ? 1 : 0;
}
