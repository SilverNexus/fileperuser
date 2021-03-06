/**
 * @file search.h
 * The file defines several searching functions' prototypes.
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "config.h"

// Implement different functions if we have different tools available.
#if defined HAVE_DIRENT_H
#include <dirent.h>
#include <sys/types.h>

void search_folder(const char *dir_path);
#elif defined HAVE_IO_H
#include <io.h>

void search_folder(const char *dir_path);
#else
#error We do not have a directory traversal for your system at this time.
#endif

void search_file_multi_match(char * const addr, size_t len, const char * const fpath);
void search_file_single_match(char * const addr, size_t len, const char * const fpath);

#endif
