cmake_minimum_required(VERSION 2.8)
project( fileperuser )
set (VERSION 0.03)
add_executable(fileperuser dir_list.c ErrorLog.c fileperuser_search.c
	       jump_table.c main.c output.c parseArgs.c result_list.c
	       search.c settings.c)

# Allow easy toggle of debugging vs. performance builds.
option(ENABLE_DEBUG "Enable creation of debugging symbols and reduction of optimization" OFF)

# Add compiler flags to gcc-like compilers
IF(UNIX)
    add_compile_options(-std=gnu99 -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -Wall -Wextra)
    IF (${ENABLE_DEBUG} == ON)
	add_compile_options(-g -O0)
    ELSE()
	add_compile_options(-s -Ofast)
    ENDIF()
ENDIF()
#Otherwise, use defaults.

include(CheckFunctionExists)
check_function_exists(nftw HAVE_NFTW)
check_function_exists(mmap HAVE_MMAP)
check_function_exists(strdup HAVE_STRDUP)

include (CheckIncludeFiles)
check_include_files(dirent.h HAVE_DIRENT_H)

# We need to check for d_type. Compile different code if we dont have it.
include (CheckStructHasMember)
check_struct_has_member("struct dirent" d_type dirent.h HAVE_DIRENT_D_TYPE)

configure_file(config.h.in config.h)

