/*
    FilePeruser, a recursive file search utility.
    Copyright (C) 2014-2016  SilverNexus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file search.c
 * The file defines the searching functions.
 */

#include "search.h"
#include <string.h>
#include "ErrorLog.h"
#include "settings.h"
#include "dir_list.h"
#include "result_list.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileperuser_search.h"
#include "jump_table.h"

#ifdef HAVE_MMAP
#include <sys/mman.h>
#include <fcntl.h>
#else
#include <stdio.h>
#endif

#if !defined (HAVE_MMAP) || !defined (HAVE_NFTW)
#include <stdlib.h>
#endif

/**
 * Parses a file for a search string, calling the appropriate matching
 * function based on user selection.
 *
 * @param fpath
 * The file path to search.
 *
 * @param file_size
 * The size of the file. Assumed to be greater than zero.
 */
inline void parse_file(const char * const fpath, const off_t file_size){
    size_t in;
#ifdef HAVE_MMAP
    // Open the file and get the file descriptor
    const int fd = open(fpath, O_RDONLY);
    if (fd == -1){
	log_event(ERROR, "Could not open file %s.", fpath);
	return;
    }
    // Map the file to memory
    // Read and write to the map, so we can substitute a \n with a \0 for searching
    char * const addr = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED){
	close(fd);
	log_event(ERROR, "Failed to map file descriptor %d (%s).", fd, fpath);
	return;
    }
    in = file_size;
    if (addr[file_size - 1] != settings.search_string[needle_len - 1])
	addr[file_size - 1] = '\0';
#else
    char * const addr = (char *)malloc(sizeof(char) * (file_size + 1));
    if (!addr){
	// Not fatal because we might legitimately not be able to parse really large files.
	log_event(ERROR, "Not enough memory to parse file %s.", fpath);
	return;
    }
    FILE *file = fopen(fpath, "rb");
    if (!file){
        log_event(ERROR, "Failed to open file %s.", fpath);
	return;
    }
    // Read the file into the malloc'ed space.
    in = fread(addr, sizeof(char), file_size, file);
    if (in != file_size)
	log_event(WARNING, "Short read occurred, may produce bogus results.");
    // Now I can even close this before the call, since we have a copy of the data.
    fclose(file);
    // Make sure the end of the file data is set to a null character.
    addr[in] = '\0';
#endif
    // Check to see if we search binary files
    // If we don't, find the first null character in the file
    // If it is before the end of the file, we assume it is a binary file.
    if (!(settings.search_flags & FLAG_BINARY_FILES)){
	// Make the last character be null
	char tmp = addr[in - 1];
	addr[in - 1] = 0;
	size_t n_len = strlen(addr);
	// If our length is too short, then it is a binary file
	if (n_len + 1 < in){
#ifdef HAVE_MMAP
	    munmap(addr, file_size);
	    close(fd);
#else
	    free(addr);
#endif
	    return;
	}
	addr[in - 1] = tmp;
    }
    /*
     * in should be the size of the file when we get here in all cases.
     * If we skip binary files, there is no reason to search those, leaving
     * text files for the full length.
     * If we search binary files, then we will always search the whole file.
     */
    settings.file_parser(addr, in, fpath);
    
    // Cleanup
#ifdef HAVE_MMAP
    munmap(addr, file_size);
    close(fd);
#else
    free(addr);
#endif
}

/**
 * Shared code between both match types.
 *
 * Macroed for maintainability.
 */
#define DETERMINE_SEARCH_FUNC_AND_SEARCH(first_var, match_type) \
    if (settings.search_flags & FLAG_NO_CASE){ \
        const char *last = addr + len - needle_len + 1; \
	if (needle_len > MIN_JUMP_TABLE_NO_CASE){ \
	    found_at = fileperuser_memcasemem_boyer(first_var, last, settings.search_string, needle_len); \
	    DO_##match_type ## _MATCHES(fileperuser_memcasemem_boyer(first_var, last, settings.search_string, needle_len)); \
	} \
	else{ \
	    found_at = fileperuser_memcasechr(first_var, last, *(settings.search_string)); \
	    DO_##match_type ## _MATCHES(fileperuser_memcasechr(first_var, last, *(settings.search_string))); \
	} \
    } \
    else{ \
	DO_CASE_SENSITIVE(first_var, match_type) \
    }

/**
 * Macro to allow for conditional building of each section of case-sensitive search
 *
 * @param first_var
 * the variable that keeps track of the position in the search
 *
 * @param match_type
 * the type of match we are performing.
 * Should be SINGLE or MULTI.
 */
#ifdef HAVE_MMAP
#define DO_CASE_SENSITIVE(first_var, match_type) \
    if (!addr[len - 1] && !(settings.search_flags & FLAG_BINARY_FILES)){ \
        found_at = strstr(first_var, settings.search_string); \
        DO_##match_type ## _MATCHES(strstr(first_var, settings.search_string)); \
    } \
    else{ \
	/* I can also skip the subtraction of the current position from len on this because addr - in_line = 0. */ \
	if (needle_len > MIN_JUMP_TABLE_CASE){ \
	    found_at = fileperuser_memmem_boyer(first_var, len, settings.search_string, needle_len); \
	    DO_##match_type ## _MATCHES(fileperuser_memmem_boyer(first_var, len - (first_var - addr), settings.search_string, needle_len)); \
	} \
	else if (needle_len > 1) { \
	    found_at = fileperuser_memmem_brute(first_var, len, settings.search_string, needle_len); \
	    DO_##match_type ## _MATCHES(fileperuser_memmem_brute(first_var, len - (first_var - addr), settings.search_string, needle_len)); \
	} \
	else { \
	    found_at = memchr(first_var, *(settings.search_string), len); \
	    DO_##match_type ## _MATCHES(memchr(first_var, *(settings.search_string), len - (first_var - addr))); \
	} \
    }
#else
#define DO_CASE_SENSITIVE(first_var, match_type) \
    found_at = strstr(first_var, settings.search_string); \
    DO_##match_type ## _MATCHES(strstr(first_var, settings.search_string));
#endif

/**
 * Shared section between both search macros that will be easier to maintain as
 * a single section of code.
 */
#define FIND_AND_SET_LINE_NUM() \
    /* Only count file lines if we find a match. */ \
    /* Substitute foundAt to make searching for lines easy */ \
    tmp = *found_at; \
    *found_at = '\0'; \
    /* Find the line num. */ \
    while ((end_line = strchr(start_line, '\n')) != 0){ \
	++line_num; \
	start_line = end_line + 1; \
    } \
    /* Substitute back *foundAt. */ \
    *found_at = tmp; \
    add_result(line_num, (long)found_at - (long)start_line + 1, fpath);

/**
 * Macro to allow for fewer comparisons to determine which search function to use.
 *
 * @param func
 * The fully formed search function to use.
 */
#define DO_MULTI_MATCHES(func) \
    if (found_at){ \
	const char *start_line = addr, *end_line; \
	char tmp; \
	register int line_num = 1; \
	do{ \
	    FIND_AND_SET_LINE_NUM() \
	    \
	    /* Continue search within the line */ \
	    in_line = found_at + 1; \
	    found_at = func; \
	} while (found_at); \
    }

/**
 * Searches the given memory-mapped file for the search string.
 * Matches multiple times per line.
 *
 * @param addr
 * Where the file is mapped to.
 *
 * @param len
 * The length of the mapped file. Can be ignored if not using mmap(2) and not using
 * case-insensitive searches.
 *
 * @param fpath
 * The path of the file being read from.
 */
void search_file_multi_match(char * const addr, size_t len, const char * const fpath){
    char *in_line = addr, *found_at;
    DETERMINE_SEARCH_FUNC_AND_SEARCH(in_line, MULTI)
}

/**
 * Macro to reduce the comparisons for determining what search function to use.
 *
 * This allows for one comparison in the calling function, and the appropriate function
 * tcan be called later without having to check again for which search to use.
 *
 * @param func
 * The fully-formed function call to use at the end of the loop to set found_at
 * for the next iteration of the loop.
 */
#define DO_SINGLE_MATCHES(func) \
    if (found_at){ \
	char *end_line; \
	char tmp; \
	register int line_num = 1; \
	do{ \
	    FIND_AND_SET_LINE_NUM() \
	    /* Go to the start of the next line to continue the search */ \
	    end_line = strchr(found_at, '\n'); \
	    if (!end_line) \
		break; \
	    /* Make sure we account for moving to a new line. */ \
	    ++line_num; \
	    start_line = end_line + 1; \
	    found_at = func; \
	} while (found_at); \
    }

/**
 * Parses a file for the search string, but only matches once per line.
 * Matches are added to the results list.
 *
 * @param addr
 * The address of the memory-mapped file to search.
 *
 * @param len
 * The length of the mapped file. Can be ignored if not using mmap(2) and not using
 * case-insensitive searches.
 *
 * @param fpath
 * The path of the file being read from.
 */
void search_file_single_match(char * const addr, size_t len, const char * const fpath){
    char *start_line = addr, *found_at;
    DETERMINE_SEARCH_FUNC_AND_SEARCH(start_line, SINGLE)
}

/**
 * Check the excluded paths for our current path
 *
 * @param fpath
 * The current path we are searching in.
 *
 * @todo
 * Make this check smarter so it does some understanding of the excluded paths
 */
#define CHECK_EXCLUDED_PATHS(fpath) \
    if (settings.excluded_paths){ \
        if (!settings.base_search_path_length) \
            settings.base_search_path_length = strlen(fpath) + 1; \
        for (DIR_LIST *pth = settings.excluded_paths; pth; pth = pth->next){ \
            if (strcmp(pth->dir, fpath + settings.base_search_path_length) == 0) \
                return FTW_SKIP_SUBTREE; \
        } \
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
    CHECK_EXCLUDED_PATHS(fpath);
    switch (typeflag){
    case FTW_D:
        // No reason to make this check if no directories have been excluded
        if (settings.excluded_directories){
	    // ftwbuf->base holds the start of this section of the path
	    // This includes everything past the last slash.
	    const char * const path = fpath + ftwbuf->base;
	    // TODO: Make the list sorted and make this a binary search.
	    for (DIR_LIST *tmp = settings.excluded_directories; tmp; tmp = tmp->next){
		if (strcmp(tmp->dir, path) == 0){
		    return FTW_SKIP_SUBTREE;
		}
	    }
        }
        return 0;
    case FTW_F:
	if (sb->st_size > 0){
#if 0
	    // Preprocess this out unless we really need it.
	    log_event(INFO, "Searching for '%s' in %s.", settings.search_string, fpath);
#endif
	    parse_file(fpath, sb->st_size);
	}
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
	    CHECK_EXCLUDED_PATHS(fpath);
	    /*
	     * We add three to the length of the two pieces so we have enough
	     * for both a null terminator and both slashes if necessary.
	     *
	     * The second slash is concatenated if we have a dir.
	     */
            char *currentDir = malloc(sizeof(char) * (strlen(fpath) + strlen(directory->d_name) + 3));
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
		    ; // Again, silencing errors
		    struct stat sb;
		    if (stat(currentDir, &sb) == -1){
			log_event(ERROR, "Could not stat %s.", currentDir);
			break;
		    }
		    if (sb.st_size > 0){
#if 0
			// Preprocess this out unless we really need it.
			log_event(INFO, "Searching for '%s' in %s.", settings.search_string, currentDir);
#endif
			parse_file(currentDir, sb.st_size);
		    }
		    break;
		case DT_LNK:
		    break;
		default:
		    log_event(WARNING, "Unsupported inode type found, skipping.");
            }
	    // We're done with it, so free it.
	    free(currentDir);
        }
    }
    else
        log_event(ERROR, "Could not open directory %s.", fpath);
    closedir(mapsDirectory);
}
#else
#error Implementation for a folder walk on your system is currently lacking.
#endif
