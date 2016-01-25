/***************************************************************************/
/*                                                                         */
/*                                search.c                                 */
/* Original code written by Daniel Hawkins. Last modified on 2016-01-20.   */
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileperuser_search.h"
#include <ctype.h>

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
    // Only check if case-sensitive
    if (!(settings.search_flags & FLAG_NO_CASE)){
	// Temporary fix to reduce frequency of segfault error further
	if (addr[file_size - 1] != settings.search_string[strlen(settings.search_string) - 1])
	    addr[file_size - 1] = '\0';
	// If we can't guarantee a null-termination, then use the custom function.
	else
	    settings.comp_func = fileperuser_memmem;
    }
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
    settings.file_parser(addr, in, fpath);
    
    // Cleanup
#ifdef HAVE_MMAP
    munmap(addr, file_size);
    close(fd);
#else
    free(addr);
#endif
}

inline size_t *get_jump_table(size_t needle_len){
    // TODO: Make the needle_len check more maintainable by making some defines to specify the algorithm swap points.
    // If case insensitive and needle_len > 3, then make a Boyer-Moore jump table
    if (settings.search_flags & FLAG_NO_CASE){
	if (needle_len > 3){
	    size_t jump_table[256];
	    unsigned short i;
	    // Initialize
	    for (i = 0; i < 256; ++i){
		jump_table[i] = needle_len;
	    }
	    // Now adjust for the characters in the needle, except the last one.
	    /**
	     * By ignoring the last character of needle, we can ensure that a failed match will jump based on
	     * the next-to-last occurrence of the last character in needle, as opposed to jumping smaller.
	     */
	    for (i = 0; i < needle_len - 1; ++i){
		jump_table[tolower(i)] = needle_len - i - 1;
		jump_table[toupper(i)] = needle_len - i - 1;
	    }
	    // Get the jump table from this scope so it can be used later.
	    return jump_table;
	}
    }
    // If case sensitive and not using strstr and needle_len > 6, then make a Boyer-Moore jump table
    else{
	if (settings.comp_func != strstr_wrapper && needle_len > 6){
	    size_t jump_table[256];
	    unsigned short i;
	    // Initialize
	    for (i = 0; i < 256; ++i){
		jump_table[i] = needle_len;
	    }
	    // Now adjust for the characters in the needle, except the last one.
	    for (i = 0; i < needle_len - 1; ++i){
		jump_table[i] = needle_len - i - 1;
	    }
	    // Get the jump table from this scope so it can be used later.
	    return jump_table;
	}
    }
    return 0;
}

/**
 * Searches the given memory-mapped file for the search string.
 * Matches multiple times per line.
 *
 * @param addr
 * Where the file is mapped to.
 *
 * @param len
 * The length of the mapped file. Can be ignored if not using mmap.
 *
 * @param fpath
 * The path of the file being read from.
 */
void search_file_multi_match(char * const addr, size_t len, const char * const fpath){
    char *in_line = addr, *found_at;
    // TODO: Abstract this check out farther to reduce redundant strlen calls.
    size_t needle_len = strlen(settings.search_string);
    // TODO: Abstract out the Boyer-Moore jump table creation. It really only needs to be done once.
    size_t *jump = get_jump_table(needle_len);

    // Only count file lines if we find a match.
    // I can also skip the subtraction of the current position from len on this because addr - in_line = 0.
    if ((found_at = settings.comp_func(in_line, len, settings.search_string, needle_len, jump)) != 0){
	char *start_line = addr, *end_line;
	char tmp;
	register int line_num = 1;
	do{
	    // Substitute foundAt to make searching for lines easy
	    tmp = *found_at;
	    *found_at = '\0';
	    // Find the line num.
	    while ((end_line = strchr(start_line, '\n')) != 0){
		++line_num;
		start_line = end_line + 1;
	    }
	    // Substitute back *foundAt.
	    *found_at = tmp;
	    add_result(line_num, (long)found_at - (long)start_line + 1, fpath);

	    // Continue search within the line
	    in_line = found_at + 1;
	} while ((found_at = settings.comp_func(in_line, len - (addr - in_line), settings.search_string, needle_len, jump)) != 0);
    }
}

/**
 * Parses a file for the search string, but only matches once per line.
 * Matches are added to the results list.
 *
 * @param addr
 * The address of the memory-mapped file to search.
 *
 * @param len
 * The length of the mapped file. Can be ignored if not using mmap.
 *
 * @param fpath
 * The path of the file being read from.
 */
void search_file_single_match(char * const addr, size_t len, const char * const fpath){
    char *start_line = addr, *found_at;
    // TODO: Abstract this check out farther to reduce redundant strlen calls.
    size_t needle_len = strlen(settings.search_string);
    // TODO: Abstract out the Boyer-Moore jump table creation. It really only needs to be done once.
    size_t *jump = get_jump_table(needle_len);
    // Only count file lines if we find a match.
    if ((found_at = settings.comp_func(start_line, len, settings.search_string, needle_len, jump)) != 0){
	char *end_line;
	char tmp;
	register int line_num = 1;
	do{
	    // Substitute foundAt to make searching for lines easy
	    tmp = *found_at;
	    *found_at = '\0';
	    // Find the line num.
	    while ((end_line = strchr(start_line, '\n')) != 0){
		++line_num;
		start_line = end_line + 1;
	    }
	    // Substitute back *foundAt.
	    *found_at = tmp;
	    add_result(line_num, (long)found_at - (long)start_line + 1, fpath);
	    // Go to the start of the next line to continue the search
	    end_line = strchr(found_at, '\n');
	    if (!end_line)
		break;
	    // Make sure we account for moving to a new line.
	    ++line_num;
	    start_line = end_line + 1;
	} while ((found_at = settings.comp_func(start_line, len - (addr - start_line), settings.search_string, needle_len, jump)) != 0);
    }
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
	    // ftwbuf->base holds the start of this section of the path
	    // This includes everything past the last slash.
	    const char * const path = fpath + ftwbuf->base;
	    for (DIR_LIST *tmp = settings.excluded_directories; tmp; tmp = tmp->next){
		if (strcmp(tmp->dir, path) == 0){
		    return FTW_SKIP_SUBTREE;
		}
	    }
        }
        return 0;
    case FTW_F:
	if (sb->st_size > 0){
	    log_event(INFO, "Searching for '%s' in %s.", settings.search_string, fpath);
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
			log_event(INFO, "Searching for '%s' in %s.", settings.search_string, currentDir);
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
