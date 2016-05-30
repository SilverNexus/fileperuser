# This will build the test files for each search function.

CC = gcc
CFLAGS = -std=c99 -Og -g -Wall -Wextra -march=native -mtune=native
FP_SEARCH_O = fp_search.o

.PHONY: all

# Make all the different test cases part of the 'all' target
all: fp_memmem_boyer fp_memmem_brute memchr strstr strchr memmem \
	fp_memcasemem_boyer fp_memcasemem_brute fp_memcasechr strcasestr

# Set up each build target

# First, do the binary files
fp_memmem_boyer: fp_memmem_boyer.o $(FP_SEARCH_O)
	$(CC) -o fp_memmem_boyer fp_memmem_boyer.o $(FP_SEARCH_O) $(CFLAGS)

fp_memmem_brute: fp_memmem_brute.o $(FP_SEARCH_O)
	$(CC) -o fp_memmem_brute fp_memmem_brute.o $(FP_SEARCH_O) $(CFLAGS)

memchr: memchr.o
	$(CC) -o memchr memchr.o $(CFLAGS)

strstr: strstr.o
	$(CC) -o strstr strstr.o $(CFLAGS)

strchr: strchr.o
	$(CC) -o strchr strchr.o $(CFLAGS)

memmem: memmem.o
	$(CC) -o memmem memmem.o -D_GNU_SOURCE $(CFLAGS)

fp_memcasemem_boyer: fp_memcasemem_boyer.o $(FP_SEARCH_O)
	$(CC) -o fp_memcasemem_boyer fp_memcasemem_boyer.o $(FP_SEARCH_O) $(CFLAGS)

fp_memcasemem_brute: fp_memcasemem_brute.o $(FP_SEARCH_O)
	$(CC) -o fp_memcasemem_brute fp_memcasemem_brute.o $(FP_SEARCH_O) $(CFLAGS)

fp_memcasechr: fp_memcasechr.o $(FP_SEARCH_O)
	$(CC) -o fp_memcasechr fp_memcasechr.o $(FP_SEARCH_O) $(CFLAGS)

strcasestr: strcasestr.o
	$(CC) -o strcasestr strcasestr.o $(CFLAGS)

# And do the object files

fp_memmem_boyer.o: main.c ../../fileperuser_search.h
	$(CC) -c main.c -o fp_memmem_boyer.o -DFP_MEMMEM_BOYER $(CFLAGS)

fp_memmem_brute.o: main.c ../../fileperuser_search.h
	$(CC) -c main.c -o fp_memmem_brute.o -DFP_MEMMEM_BRUTE $(CFLAGS)

memchr.o: main.c
	$(CC) -c main.c -o memchr.o -DMEMCHR $(CFLAGS)

strstr.o: main.c
	$(CC) -c main.c -o strstr.o -DSTRSTR $(CFLAGS)

strchr.o: main.c
	$(CC) -c main.c -o strchr.o -DSTRCHR $(CFLAGS)

memmem.o: main.c
	$(CC) -c main.c -o memmem.o -DMEMMEM -D_GNU_SOURCE $(CFLAGS)

fp_memcasemem_boyer.o: main.c ../../fileperuser_search.h
	$(CC) -c main.c -o fp_memcasemem_boyer.o -DFP_MEMCASEMEM_BOYER $(CFLAGS)

fp_memcasemem_brute.o: main.c ../../fileperuser_search.h
	$(CC) -c main.c -o fp_memcasemem_brute.o -DFP_MEMCASEMEM_BRUTE $(CFLAGS)

fp_memcasechr.o: main.c ../../fileperuser_search.h
	$(CC) -c main.c -o fp_memcasechr.o -DFP_MEMCASECHR $(CFLAGS)

strcasestr.o: main.c
	$(CC) -c main.c -o strcasestr.o -DSTRCASESTR -D_GNU_SOURCE $(CFLAGS)

$(FP_SEARCH_O): ../../fileperuser_search.c ../../fileperuser_search.h
	$(CC) -c ../../fileperuser_search.c -o $(FP_SEARCH_O) $(CFLAGS)

clean:
	rm fp_memmem_boyer fp_memmem_boyer.o fp_memmem_brute fp_memmem_brute.o \
	memchr memchr.o strstr strstr.o strchr strchr.o memmem memmem.o \
	fp_memcasemem_boyer fp_memcasemem_boyer.o fp_memcasemem_brute fp_memcasemem_brute.o \
	fp_memcasechr fp_memcasechr.o strcasestr strcasestr.o $(FP_SEARCH_O)
