/**
 * @file search.h
 * Last modified on 2016-01-25 by Daniel Hawkins.
 *
 * The file defines several searching functions' prototypes.
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "config.h"

// Implement different functions if we have different tools available.
#if defined HAVE_NFTW
#include <ftw.h>

int onWalk(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
#elif defined HAVE_DIRENT_H
#include <dirent.h>
#include <sys/types.h>

void search_folder(const char *dir_path);
#else
#error We do not have a directory traversal for your system at this time.
#endif

void search_file_multi_match(char * const addr, size_t len, const char * const fpath);
void search_file_single_match(char * const addr, size_t len, const char * const fpath);

#endif
