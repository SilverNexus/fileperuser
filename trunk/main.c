/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-21.   */
/*                                                                         */
/* The file defines the main function and several searching functions.     */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "main.h"
#include <dirent.h>
#include <sys/types.h>
#include "ErrorLog.h"
#include "parseArgs.h"
#include "settings.h"

#define BIG_BUFFER 9000

int main(int argc, char *argv[]){
    if (argc >= 3){
        init_settings();
        // argv[0] is the executable name, which I don't need.
        int parse_results = parseArgs(argv + 2, argc - 2);
        if (parse_results == -1){
            logError(ERROR, "Invalid arguments discovered.");
            free_settings();
            return 1;
        }
        // If execution was for usage info, exit here
        else if (parse_results == 1){
            free_settings();
            return 0;
        }
        // If no arguments set the root directory, error out
        if (!settings.root_dir){
            logError(ERROR, "No root directory specified in search.");
            free_settings();
            return 1;
        }
        // Remove the old results file
        remove("searchResults.txt");
        // Begin the search
        searchFolder(argv[1], settings.root_dir);
        puts("The matches have been stored in searchResults.txt.\n");
        free_settings();
    }
    else{
        logError(ERROR, "MapChecker requires arguments: ./MapChecker <search string> -d <path to folder to search in> [flags].");
        return 1;
    }
    return 0;
}

int findIn(const char *searchIn, const char *searchFor){
    register int searchInLen = strlen(searchIn);
    register int searchForLen = strlen(searchFor);
    for (register int startAt = 0; startAt <= searchInLen - searchForLen; startAt++){
        // Add condition to speed up searches -- only even try to find if first character matches
        if (*(searchIn + startAt) == *searchFor){
            if (strncmp(searchIn + startAt, searchFor, searchForLen) == 0){
                return startAt;
            }
        }
    }
    return -1;
}

int searchFolder(const char *searchFor, const char *dirPath){
    DIR *mapsDirectory = opendir(dirPath);
    if (mapsDirectory){
        struct dirent *directory;
        while ((directory = readdir(mapsDirectory))){
            if (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0)
                continue;
            char currentDir[BIG_BUFFER];
            strcpy(currentDir, dirPath);
            strcat(currentDir, directory->d_name);
            if (directory->d_type == DT_DIR){
                struct exclude_dir *tmp;
                int skip = 0;
                for (tmp = settings.excluded_directories; tmp; tmp = tmp->next){
                    if (strcmp(tmp->excludeDir, directory->d_name) == 0){
                        skip = 1;
                    }
                }
                if (!skip){
                    strcat(currentDir, "/");
                    searchFolder(searchFor, currentDir);
                }
            }
            else{
                printf("Searching for '%s' in %s.\n", searchFor, currentDir);
                FILE *mapFile;
                mapFile = fopen(currentDir, "r");
                if (mapFile){
                    register int lineNum = 0;
                    char linechars[BIG_BUFFER];
                    FILE *outputFile = fopen("searchResults.txt", "a");
                    while (fgets(linechars, BIG_BUFFER, mapFile)){
                        lineNum++;
                        int foundAt;
                        if ((foundAt = findIn(linechars, searchFor)) != -1){
                            // There is no reason to print it to the screen - you not going to see anything in the swirling mass of text flying by
                            fprintf(outputFile, "Found instance of '%s' in line %i, col %i of %s.\n", searchFor, lineNum, foundAt + 1, currentDir);
                        }
                    }
                    fclose(outputFile);
                    // Only close if nonzero
                    fclose(mapFile);
                }
                else{
                    logError(ERROR, "Failed to open file %s.", currentDir);
                }
            }
        }
    }
    else{
        logError(ERROR, "Could not open directory %s.", dirPath);
        return 1;
    }
    closedir(mapsDirectory);
    return 0;
}
