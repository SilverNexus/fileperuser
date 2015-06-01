EXTRA_DIST = AUTHORS ChangeLog COPYING autogen.sh

ACLOCAL_AMFLAGS = -I macros --install

bin_PROGRAMS = fileperuser

fileperuser_SOURCES =\
	main.c\
	ErrorLog.c\
	dir_list.c\
	search.c\
	settings.c\
	parseArgs.c

AM_CFLAGS = -std=gnu99 -D_XOPEN_SOURCE=600 -D_GNU_SOURCE
