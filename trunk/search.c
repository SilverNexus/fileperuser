/***************************************************************************/
/*                                                                         */
/*                                search.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-01-31.   */
/*                                                                         */
/* The file defines the searching functions.                               */
/*                                                                         */
/***************************************************************************/

#include "search.h"
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include "ErrorLog.h"
#include "settings.h"

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
 * Traverses through the directory tree of the designated root directory of the search.
 * Opens files to search along the way and searches them for the desired search string.
 *
 * @param dirPath The directory being searched.
 *
 * @retval 1 Could not open the directory.
 * @retval 0 Successfully parsed directory.
 *
 * @todo utilize return value or make void
 */
int searchFolder(const char *dirPath){
    DIR *mapsDirectory = opendir(dirPath);
    if (mapsDirectory){
        struct dirent *directory;
        while ((directory = readdir(mapsDirectory))){
            if (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0)
                continue;
            char currentDir[BIG_BUFFER];
            strcpy(currentDir, dirPath);
            // If there wasn't already a "/" at the end, add it here.
            if (currentDir[strlen(dirPath) - 1] != '/')
                strcat(currentDir, "/");
            strcat(currentDir, directory->d_name);
            if (directory->d_type == DT_DIR){
                struct dir_list *tmp;
                int skip = 0;
                for (tmp = settings.excluded_directories; tmp; tmp = tmp->next){
                    if (strcmp(tmp->dir, directory->d_name) == 0){
                        skip = 1;
                    }
                }
                if (!skip){
                    strcat(currentDir, "/");
                    searchFolder(currentDir);
                }
            }
            else{
                log_event(INFO, "Searching for '%s' in %s.", settings.search_string, currentDir);
                FILE *mapFile;
                mapFile = fopen(currentDir, "r");
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
                            fprintf(outputFile, "Found instance of '%s' in line %i, col %i of %s.\n", settings.search_string, lineNum, foundAt + 1, currentDir);
                        }
                    }
                    if (outputFile)
                        fclose(outputFile);
                    // Only close if nonzero
                    fclose(mapFile);
                }
                else{
                    log_event(ERROR, "Failed to open file %s.", currentDir);
                }
            }
        }
    }
    else{
        log_event(ERROR, "Could not open directory %s.", dirPath);
        return 1;
    }
    closedir(mapsDirectory);
    return 0;
}
