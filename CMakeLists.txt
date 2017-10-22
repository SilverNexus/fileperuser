cmake_minimum_required(VERSION 3.0)
project(fileperuser VERSION 0.03 LANGUAGES C)

# Make sure we have a list of build types.
IF(CMAKE_CONFIGURATION_TYPES)
    # Reduce the number of build options we have.
    set(CMAKE_CONFIGURATION_TYPES Debug Release)
    # Send it to the cache
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reduce build options" FORCE)
ENDIF()

# Add compiler flags to gcc-like compilers
IF(UNIX OR MINGW OR CYGWIN)
    add_compile_options(-std=gnu99 -Wall -Wextra)
    # Set debug nd release flags
    set(CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "Have debugging information for debug" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "-s -Ofast" CACHE STRING "Optimize for release" FORCE)
ELSEIF(MSVC)
    # Add compiler flags for MSVC projects.
    add_compile_options(/TC /W3)
    # Set debug nd release flags
    set(CMAKE_C_FLAGS_DEBUG "/Od /Zi" CACHE STRING "Have debugging information for debug" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "/Ox /Ot /Ob2 /GS- /sdl-" CACHE STRING "Optimize for release" FORCE)
    # Add define for conditional compilation of MSVC-specific fixes.
    add_definitions(/DMSVC)
ENDIF()
#Otherwise, use defaults.

include(CheckFunctionExists)
check_function_exists(mmap HAVE_MMAP)

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

add_executable(fileperuser dir_list.c ErrorLog.c fileperuser_search.c
	       jump_table.c main.c output.c parseArgs.c result_list.c
	       search.c settings.c)

# Since there doesn't appear to be a built-in way to install a manpage, do it the hard way
# but only do it in linux and bsd
if (UNIX AND NOT APPLE)
    # Allow for choice of man directory.
    set(MAN_INSTALL_DIR /usr/local/man/en_US CACHE STRING "Manual Directory")
    mark_as_advanced(MAN_INSTALL_DIR)

    # Make a list of manpages and set them up to install on installation.
    # Gleaned from https://cmake.org/pipermail/cmake/2010-September/039781.html
    set(MAN_NAMES fileperuser.1)
    set(MAN_FILES)
    foreach(m IN LISTS MAN_NAMES)
        set(mf ${CMAKE_BINARY_DIR}/doc/${m})
        set(ms ${CMAKE_SOURCE_DIR}/doc/${m}.txt)
        # Since this whole section is contingent on Unix-like OSes, cp should exist.
        add_custom_command(OUTPUT ${mf}
            COMMAND cp ${ms} ${mf}
            DEPENDS ${ms}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building manpage ${mf}"
            VERBATIM)
        list(APPEND MAN_FILES ${mf})
    endforeach()
    # Then we make a build target
    add_custom_target(man ALL DEPENDS ${MAN_FILES})
    # Set up the man install directory.
    install(FILES ${mf} DESTINATION ${MAN_INSTALL_DIR}/man1/)
elseif(WIN32)
    #Add the directory to the system path.
    add_custom_command(
	TARGET fileperuser
	POST_BUILD
	COMMAND cmd add_to_system_path.bat "${CMAKE_INSTALL_PREFIX}\\bin"
    )
endif()

install(TARGETS fileperuser DESTINATION bin)
