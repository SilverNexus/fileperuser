/***************************************************************************/
/*                                                                         */
/*                                 main.c                                  */
/* Original code written by Daniel Hawkins. Last modified on 2014-05-23.   */
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
        int parse_results = parseArgs(argv + 1, argc - 1);
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
        if (!settings.root_dirs){
            logError(ERROR, "No root directory specified in search.");
            free_settings();
            return 1;
        }
        // Remove any existing results file by this name
        remove(settings.output_file);
        // Begin the search
        DIR_LIST *thisDir = settings.root_dirs;
        while (thisDir){
            searchFolder(thisDir->dir);
            thisDir = thisDir->next;
        }
        printf("The matches have been stored in %s.\n", settings.output_file);
        free_settings();
    }
    else{
        logError(ERROR, "MapChecker requires arguments: ./MapChecker -s <search string> -d <path to folder to search in> [flags].");
        return 1;
    }
    return 0;
}

int findIn(const char *searchIn){
    register int searchInLen = strlen(searchIn);
    register int searchForLen = strlen(settings.search_string);
    for (register int startAt = 0; startAt <= searchInLen - searchForLen; startAt++){
        // Add condition to speed up searches -- only even try to find if first character matches
        if (*(searchIn + startAt) == *settings.search_string){
            if (strncmp(searchIn + startAt, settings.search_string, searchForLen) == 0){
                return startAt;
            }
        }
    }
    return -1;
}

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
                printf("Searching for '%s' in %s.\n", settings.search_string, currentDir);
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
                                logError(ERROR, "Could not open '%s' for writing.", settings.output_file);
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
