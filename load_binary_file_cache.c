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
    unsigned int list_length;
    // To reduce conversions, the file stores the length in a binary format.
    /*
     * We shouldn't have to deal with endianness because this shouldn't be
     * migrating between different processors.
     * Processors, such as ARM, that can switch likely have a default one to
     * output to, but I should investigate into that more.
     */
    size_t bytes = fread(&list_length, sizeof(int), 1, cache_file);
    if (bytes < sizeof(int)){
	log_event(ERROR, "Cache file %s is truncated... skipping.", path);
    }
    // Skip the newline after that value
    fseek(cache_file, 1, SEEK_CUR);
    // Set up an array of the binary file paths.
    // TODO: Make this accessible outside the function
    const char **binary_file_list;
    binary_file_list = malloc(sizeof(char *) * list_length);
    // Check for allocation failure
    if (!binary_file_list){
	// FATAL bails out of the program.
	log_event(FATAL, "Failed to allocate space for binary file list cache.");
    }
    // Now read the file list from the file
    char linebuffer[1000]; // WAY bigger than anything should sensibly be

    // Order matters here -- the output should be sorted for a binary search.
    for (unsigned i = 0; i < list_length; ++i){
	fgets(linebuffer, 1000, cache_file);
	// Set up and copy to the binary file list.
	binary_file_list[i] = malloc(sizeof(char) * (strlen(linebuffer) + 1));
	if (!binary_file_list[i]){
	    log_event(FATAL, "Memory allocation failure for file path #%lu (%s).", i + 1, linebuffer);
	}
	// Since we specifically allocated for the result, we can safely use strcpy.
	strcpy(binary_file_list[i], linebuffer);
    }
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
}
