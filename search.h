/***************************************************************************/
/*                                                                         */
/*                                search.h                                 */
/* Original code written by Daniel Hawkins. Last modified on 2016-01-10.   */
/*                                                                         */
/* The file defines several searching functions' prototypes.               */
/*                                                                         */
/***************************************************************************/

#ifndef SEARCH_H
#define SEARCH_H

#include "config.h"
#ifndef HAVE_MMAP
#include <stdio.h>
#endif

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

#ifdef HAVE_MMAP
void search_file_multi_match(char * const addr, const char * const fpath);
void search_file_single_match(char * const addr, const char * const fpath);
#else
void search_file_multi_match(FILE *file, const char * const fpath);
void search_file_single_match(FILE *file, const char * const fpath);
#endif

#endif
