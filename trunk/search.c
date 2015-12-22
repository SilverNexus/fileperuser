/***************************************************************************/
/*                                                                         */
/*                                search.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2015-12-20.   */
/*                                                                         */
/* The file defines the searching functions.                               */
/*                                                                         */
/***************************************************************************/

#include "search.h"
#include <string.h>
#include "ErrorLog.h"
#include "settings.h"
#include "dir_list.h"
#include "result_list.h"

#ifdef HAVE_MMAP
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <stdio.h>
#define BIG_BUFFER 9000
#endif

/**
 * Parses a file for a search string, matching any number of occurrences per line.
 *
 * @param fpath
 * The file path to search.
 */
void parse_file(const char *fpath){
#ifdef HAVE_MMAP
    // Open the file and get the file descriptor
    int fd = open(fpath, O_RDONLY);
    if (!fd){
	log_event(ERROR, "Could not open file %s.", fpath);
	return;
    }
    struct stat sb;
    // Get the file size (and some other stuff, too)
    if (fstat(fd, &sb) == -1){
	close(fd);
	log_event(ERROR, "Could not fstat file descriptor %d (%s).", fd, fpath);
	return;
    }
    // Also, if an empty file, return here silently.
    if (sb.st_size == 0){
	close(fd);
	return;
    }
    char *addr;
    char *start_line, *end_line, *in_line;
    // Map the file to memory
    // Read and write to the map, so we can substitute a \n with a \0 for searching
    in_line = start_line = addr = mmap(0, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED){
	close(fd);
	log_event(ERROR, "Failed to map file descriptor %d (%s).", fd, fpath);
	return;
    }
    char *foundAt;
    // Only count file lines if we find a match.
    if ((foundAt = strstr(in_line, "alt")) != 0){
	char tmp;
	register int line_num = 1;
	do{
	    // Substitute foundAt to make searching for lines easy
	    tmp = *foundAt;
	    *foundAt = '\0';
	    // Find the line num.
	    while ((end_line = strchr(start_line, '\n')) != 0){
		++line_num;
		start_line = end_line + 1;
	    }
	    // Substitute back *foundAt.
	    *foundAt = tmp;
	    add_result(line_num, (long)foundAt - (long)start_line + 1, fpath);

	    // Continue search within the line
	    in_line = foundAt + 1;
	} while ((foundAt = strstr(in_line, "alt")) != 0);
    }
    
    munmap(addr, sb.st_size);
    close(fd);
#else
    static char start_line[BIG_BUFFER];
    FILE *file = fopen(fpath, "r");
    if (!file){
        log_event(ERROR, "Failed to open file %s.", fpath);
	return;
    }
    // Loop optimization
    if (fgets(start_line, BIG_BUFFER, file)){
        char *foundAt;
        int col;
        register int line_num = 0;
        do{
	    ++line_num;
	    col = 0;
	    while ((foundAt = settings.comp_func(start_line + col, settings.search_string)) != 0){
		col = (long)foundAt - (long)start_line + 1;
		add_result(line_num, col, fpath);
	    }
        } while (fgets(start_line, BIG_BUFFER, file));
    }
    fclose(file);
#endif
}

/**
 * Parses a file for the search string, but only matches once per line.
 * Matches are added to the results list.
 *
 * @param fpath
 * The file path to search.
 */
void parse_file_single_match(const char *fpath){
#ifdef HAVE_MMAP
    // Open the file and get the file descriptor
    int fd = open(fpath, O_RDONLY);
    if (!fd){
	log_event(ERROR, "Could not open file %s.", fpath);
	return;
    }
    struct stat sb;
    // Get the file size (and some other stuff, too)
    if (fstat(fd, &sb) == -1){
	close(fd);
	log_event(ERROR, "Could not fstat file descriptor %d (%s).", fd, fpath);
	return;
    }
    // Also, if an empty file, return here silently.
    if (sb.st_size == 0){
	close(fd);
	return;
    }
    char *addr;
    char *start_line, *end_line;
    // Map the file to memory
    // Read and write to the map, so we can substitute a \n with a \0 for searching
    start_line = addr = mmap(0, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED){
	close(fd);
	log_event(ERROR, "Failed to map file descriptor %d (%s).", fd, fpath);
	return;
    }
    char *foundAt;
    // Only count file lines if we find a match.
    if ((foundAt = strstr(start_line, "alt")) != 0){
	char tmp;
	register int line_num = 1;
	do{
	    // Substitute foundAt to make searching for lines easy
	    tmp = *foundAt;
	    *foundAt = '\0';
	    // Find the line num.
	    while ((end_line = strchr(start_line, '\n')) != 0){
		++line_num;
		start_line = end_line + 1;
	    }
	    // Substitute back *foundAt.
	    *foundAt = tmp;
	    add_result(line_num, (long)foundAt - (long)start_line + 1, fpath);
	    // Go to the start of the next line to continue the search
	    end_line = strchr(foundAt, '\n');
	    if (!end_line)
		break;
	    // Make sure we account for moving to a new line.
	    ++line_num;
	    start_line = end_line + 1;
	} while ((foundAt = strstr(start_line, "alt")) != 0);
    }
    
    munmap(addr, sb.st_size);
    close(fd);
#else
    static char start_line[BIG_BUFFER];
    FILE *file = fopen(fpath, "r");
    if (!file){
        log_event(ERROR, "Failed to open file %s.", fpath);
	return;
    }
    // Loop optimization
    if (fgets(start_line, BIG_BUFFER, file)){
        char *foundAt;
        register int line_num = 0;
        do{
	    ++line_num;
	    if ((foundAt = settings.comp_func(start_line, settings.search_string)) != 0){
		add_result(line_num, (long)foundAt - (long)start_line + 1, fpath);
	    }
        } while (fgets(start_line, BIG_BUFFER, file));
    }
    fclose(file);
#endif
}
    

#if defined HAVE_NFTW
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
    if (settings.excluded_paths){
        if (!settings.base_search_path_length)
            // If this is the first time into this search tree, get the absolute path of the tree root.
            // Conveniently, this is fpath on the first run, so use that.
            // NOTE: We add one to cover the trailing slash.
            settings.base_search_path_length = strlen(fpath) + 1;

        // Check to see if we are specifically excluding this path from the search
        // This is listed out here so we can exclude specific files, as well.
        for (DIR_LIST *pth = settings.excluded_paths; pth; pth = pth->next){
            // For each excluded path, see if we are encountering the path we wish to exclude.
            if (strcmp(pth->dir, fpath + settings.base_search_path_length) == 0)
                return FTW_SKIP_SUBTREE;
        }
    }
    switch (typeflag){
    case FTW_D:
        // No reason to make this check if no directories have been excluded
        if (settings.excluded_directories){
            // Find the last instance of '/' in fpath.
            // We want to start at the character after that.
            char *path = strrchr(fpath, '/');

            if (path){
                // If we found it, then we take the character after the slash
                ++path;

                for (DIR_LIST *tmp = settings.excluded_directories; tmp; tmp = tmp->next){
                    if (strcmp(tmp->dir, path) == 0){
                        return FTW_SKIP_SUBTREE;
                    }
                }
            }
        }
        return 0;
    case FTW_F:
        log_event(INFO, "Searching for '%s' in %s.", settings.search_string, fpath);
        settings.file_parser(fpath);
        return 0;
    case FTW_SL:
        return 0;
    default:
        log_event(WARNING, "Unsupported inode type found, skipping.");
        return 0;
    }
}
#elif defined HAVE_DIRENT_H

/**
 * Traverses through the directory tree of the designated root directory of the search.
 * Opens files to search along the way and searches them for the desired search string.
 *
 * @param fpath The directory being searched.
 */
void search_folder(const char *fpath){
    DIR *mapsDirectory = opendir(fpath);
    if (mapsDirectory){
        struct dirent *directory;
        while ((directory = readdir(mapsDirectory))){
            if (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0)
                continue;
	    if (settings.excluded_paths){
		if (!settings.base_search_path_length)
		    // If this is the first time into this search tree, get the absolute path of the tree root.
		    // Conveniently, this is fpath on the first run, so use that.
		    // NOTE: We add one to cover the trailing slash.
		    settings.base_search_path_length = strlen(fpath) + 1;

		// Check to see if we are specifically excluding this path from the search
		// This is listed out here so we can exclude specific files, as well.
		for (DIR_LIST *pth = settings.excluded_paths; pth; pth = pth->next){
		    // For each excluded path, see if we are encountering the path we wish to exclude.
		    if (strcmp(pth->dir, fpath + settings.base_search_path_length) == 0)
			return;
		}
	    }
            char currentDir[BIG_BUFFER];
            strcpy(currentDir, fpath);
            // If there wasn't already a "/" at the end, add it here.
            if (currentDir[strlen(fpath) - 1] != '/')
                strcat(currentDir, "/");
            strcat(currentDir, directory->d_name);
	    switch (directory->d_type){
		case DT_DIR:
		    ; // Silences errors about declaring variables within the switch statement.
		    int skip = 0;
		    for (DIR_LIST *tmp = settings.excluded_directories; tmp; tmp = tmp->next){
			if (strcmp(tmp->dir, directory->d_name) == 0){
			    skip = 1;
			    break;
			}
		    }
		    if (!skip){
			strcat(currentDir, "/");
			search_folder(currentDir);
		    }
		    break;
		case DT_REG:
		    log_event(INFO, "Searching for '%s' in %s.", settings.search_string, currentDir);
		    settings.file_parser(currentDir);
		    break;
		case DT_LNK:
		    break;
		default:
		    log_event(WARNING, "Unsupported inode type found, skipping.");
            }
        }
    }
    else
        log_event(ERROR, "Could not open directory %s.", fpath);
    closedir(mapsDirectory);
}
#else
#error Implementation for a folder walk on your system is currently lacking.
#endif
