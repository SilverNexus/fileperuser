/***************************************************************************/
/*                                                                         */
/*                                search.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-06-03.   */
/*                                                                         */
/* The file defines the searching functions.                               */
/*                                                                         */
/***************************************************************************/

#include "search.h"
#include <string.h>
#include <stdio.h>
#include "ErrorLog.h"
#include "settings.h"
#include "dir_list.h"
#include "result_list.h"

#define BIG_BUFFER 9000

/**
 * Checks each encountered inode so that it is handled correctly. Called from nftw(3)
 * Opens files to search along the way and searches them for the desired search string.
 *
 * @param fpath The directory being searched.
 * @param sb Contains information about the current inode.
 * @param typeflag The type of inode.
 * @param ftwbuf Contains more info, but I'm not sure. Its needed for nftw(3) to work.
 *
 * @retval FTW_SKIP_SUBTREE nftw(3) should skip this folder.
 * @retval 0 nftw(3) should continue parsing as expected.
 */
int onWalk(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    switch (typeflag){
    case FTW_D:
        // No reason to make this check if no directories have been excluded
        if (settings.excluded_directories){
            // Find the last instance of '/' in fpath.
            // We want to start at the character after that.
            char *path = strrchr(fpath, '/') + 1;
            for (DIR_LIST *tmp = settings.excluded_directories; tmp; tmp = tmp->next){
                if (strcmp(tmp->dir, path) == 0){
                    return FTW_SKIP_SUBTREE;
                }
            }
        }
        return 0;
    case FTW_F:
        log_event(INFO, "Searching for '%s' in %s.", settings.search_string, fpath);
        FILE *mapFile;
        mapFile = fopen(fpath, "r");
        if (mapFile){
            register int lineNum = 0;
            char linechars[BIG_BUFFER];
            FILE *outputFile = 0;
            char *foundAt;
            int col;
            while (fgets(linechars, BIG_BUFFER, mapFile)){
                ++lineNum;
                col = 0;
                // Find multiple instances within the same line
                while ((foundAt = settings.comp_func(linechars + col, settings.search_string)) != 0){
                    col = (long)foundAt - (long)linechars + 1;
                    add_result(lineNum, col, fpath);
                }
            }
            // Only close if nonzero
            fclose(mapFile);
        }
        else
            log_event(ERROR, "Failed to open file %s.", fpath);
        return 0;
    case FTW_SL:
        return 0;
    default:
        log_event(WARNING, "Unsupported inode type found, skipping.");
        return 0;
    }
}
