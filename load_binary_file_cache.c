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
    // TODO: Set up an array of the binary file paths.
    char linebuffer[1000]; // WAY bigger than anything should sensibly be
}
