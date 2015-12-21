/***************************************************************************/
/*                                                                         */
/*                                search.h                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-12-20.   */
/*                                                                         */
/* The file defines several searching functions' prototypes.               */
/*                                                                         */
/***************************************************************************/

#ifndef SEARCH_H
#define SEARCH_H

#include "config.h"
#include <stdio.h>

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

void parse_file(const char *fpath);
void parse_file_single_match(const char *fpath);

#endif
