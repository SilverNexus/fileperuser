/***************************************************************************/
/*                                                                         */
/*                                search.h                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-05-30.   */
/*                                                                         */
/* The file defines several searching functions' prototypes.               */
/*                                                                         */
/***************************************************************************/

#ifndef SEARCH_H
#define SEARCH_H

#include <ftw.h>

int findIn(const char *);
int onWalk(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

#endif
