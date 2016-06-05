/**
 * @file binary_file_cache.h
 *
 * Holds the declarations for binary file cache handling.
 *
 * @author Daniel Hawkins
 */
#ifndef BINARY_FILE_CACHE_H
#define BINARY_FILE_CACHE_H

// The linked list for newly detected binary files.
extern DIR_LIST *new_cache_list;
// The number of items in the list.
extern unsigned num_new_files;
// The array for existing cache entries.
extern char **binary_file_list;
// The length of the existing array
extern unsigned list_length;

void init_binary_cache();
int read_cache_file(const char *const path);
void cleanup_cache_list();
int save_binary_cache(const char *const path);
void add_new_binary_file(const char * const bin_path);
int check_path(const char * const path);

#endif
