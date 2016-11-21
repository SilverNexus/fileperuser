cmake_minimum_required(VERSION 2.8)
project( fileperuser )
set (VERSION 0.03)

# Allow easy toggle of debugging vs. performance builds.
option(ENABLE_DEBUG "Enable creation of debugging symbols and reduction of optimization" OFF)

# Add compiler flags to gcc-like compilers
IF(UNIX OR MINGW OR CYGWIN)
    add_compile_options(-std=gnu99 -D_XOPEN_SOURCE=600 -D_GNU_SOURCE -Wall -Wextra)
    IF (${ENABLE_DEBUG} STREQUAL ON)
	add_compile_options(-g -O0)
    ELSE()
	add_compile_options(-s -Ofast)
    ENDIF()
ELSEIF(MSVC)
    # Add compiler flags for MSVC projects.
    add_compile_options(/TC /W3)
    IF(${ENABLE_DEBUG} STREQUAL ON)
	add_compile_options(/Od /Zi)
    ELSE()
	# Currently conflicts with /RTC1 -- need to refactor debugging to use built-in CMake one.
	#add_compile_options(/Ox /Ot /Ob2 /GS- /sdl-)
    ENDIF()
ENDIF()
#Otherwise, use defaults.

include(CheckFunctionExists)
check_function_exists(nftw HAVE_NFTW)
check_function_exists(mmap HAVE_MMAP)
check_function_exists(strdup HAVE_STRDUP)

include (CheckIncludeFiles)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(io.h HAVE_IO_H)
check_include_files(errno.h HAVE_ERRNO_H)

# We need to check for d_type. Compile different code if we dont have it.
include (CheckStructHasMember)
check_struct_has_member("struct dirent" d_type dirent.h HAVE_DIRENT_D_TYPE)

include (CheckSymbolExists)
check_symbol_exists(S_ISLNK dirent.h;sys/types.h;sys/stat.h HAVE_DIRENT_S_ISLNK)

configure_file(config.h.in config.h)

# Since there doesn't appear to be a built-in way to install a manpage, do it the hard way
# but only do it in linux and bsd
if (UNIX AND NOT APPLE)
    # TODO: Should this be its own build target?
    add_custom_command(
	TARGET fileperuser
	POST_BUILD
	COMMAND sudo sh install_manpage.sh
    )
elseif(WIN32)
    #Add the directory to the system path.
    add_custom_command(
	TARGET fileperuser
	POST_BUILD
	COMMAND cmd add_to_system_path.bat "${CMAKE_INSTALL_PREFIX}/bin"
    )
endif()

add_executable(fileperuser dir_list.c ErrorLog.c fileperuser_search.c
	       jump_table.c main.c output.c parseArgs.c result_list.c
	       search.c settings.c)

install(TARGETS fileperuser DESTINATION bin)
