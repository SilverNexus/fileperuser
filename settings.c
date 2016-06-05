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
 * @file settings.c
 * The file defines the functions for handling settings.
 *
 */

#include "settings.h"
#include "ErrorLog.h"
#include "dir_list.h"
#include "search.h"
#include <string.h>
#include "config.h"
#include "fileperuser_search.h"

/**
 * Initializes the settings to their default values.
 *
 * These are then overridden by command-line flags as defined in parseArgs.
 */
void init_settings(){
    settings.excluded_directories = 0;
    settings.excluded_paths = 0;
    settings.base_search_path_length = 0;
    settings.root_dirs = 0;
    settings.search_string = 0;
    settings.search_flags = FLAG_NONE;
    settings.file_parser = search_file_multi_match;
    settings.output_file = 0;
    settings.log_file = "fileperuser.log";
    // Eww... what an ugly default name.
    settings.cache_file = ".fp_binarycache";
    // Only log warning and higher by default
    settings.min_log_level = WARNING;
    settings.min_print_level = WARNING;
}

/**
 * Frees any dynamically allocated units within settings
 * (e.g. any DIR_LIST varaibles)
 */
void free_settings(){
    free_dir_list(settings.excluded_directories);
    free_dir_list(settings.excluded_paths);
    free_dir_list(settings.root_dirs);
}

/**
 * Adds a directory to the excluded directories list
 *
 * @param newDir The name of the directory to be added.
 *
 * @note cannot fail, so don't return a success value.
 */
void add_exclude_dir(char *newDir){
    DIR_LIST *newExclusion = init_dir_node(newDir);
    /*
     * init_dir_node() cannot return null, so don't check for it.
     */
    link_dir_node(newExclusion, &settings.excluded_directories);
}

/**
 * Adds a path to the excluded paths list
 *
 * @param newDir The name of the path to be added.
 *
 * @note cannot fail, so don't return a success value.
 */
void add_exclude_path(char *newDir){
    DIR_LIST *newExclusion = init_dir_node(newDir);
    /*
     * init_dir_node() cannot return null, so don't check for it.
     */
    link_dir_node(newExclusion, &settings.excluded_paths);
}

/**
 * Adds a directory to the root directory list.
 *
 * @param newDir The directory path to be added.
 *
 * @note cannot fail, so don't return a success value.
 */
void add_root_dir(char *newDir){
    DIR_LIST *newRoot = init_dir_node(newDir);
    /*
     * init_dir_node() cannot return null, so don't check for it.
     */
    link_dir_node(newRoot, &settings.root_dirs);
}
