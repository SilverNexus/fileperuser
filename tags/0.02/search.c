/***************************************************************************/
/*                                                                         */
/*                                search.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-05-30.   */
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

#define BIG_BUFFER 9000

/**
 * Searches a given string (usually a line of text from a file) for the search string.
 *
 * @param searchIn the string to search
 *
 * @return The postion in the string where the search string was found, or -1 if it wasn't.
 */
int findIn(const char *searchIn){
    register int searchInLen = strlen(searchIn);
    for (register int startAt = 0; startAt <= searchInLen - settings.search_string_len; startAt++){
        // Add condition to speed up searches -- only even try to find if first character matches
        if (*(searchIn + startAt) == *settings.search_string){
            if (settings.comp_func(searchIn + startAt, settings.search_string, settings.search_string_len) == 0){
                return startAt;
            }
        }
    }
    return -1;
}

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
            while (fgets(linechars, BIG_BUFFER, mapFile)){
                lineNum++;
                int foundAt;
                if ((foundAt = findIn(linechars)) != -1){
                    if (!outputFile){
                        // Open the file only if we need it.
                        outputFile = fopen(settings.output_file, "a");
                    }
                    // Error out if load failed
                    if (!outputFile){
                        log_event(ERROR, "Could not open '%s' for writing.", settings.output_file);
                        break;
                    }
                    // There is no reason to print it to the screen - you not going to see anything in the swirling mass of text flying by
                    fprintf(outputFile, "Found instance of '%s' in line %i, col %i of %s.\n", settings.search_string, lineNum, foundAt + 1, fpath);
                }
            }
            if (outputFile)
                fclose(outputFile);
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
