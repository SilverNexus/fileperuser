#!/bin/bash

PARAM=$1

# Gather the calls
valgrind --tool=callgrind --callgrind-out-file=fp_memcasechr.calls ./fp_memcasechr "$PARAM" > fp_memcasechr.out
valgrind --tool=callgrind --callgrind-out-file=fp_memcasemem_boyer.calls ./fp_memcasemem_boyer "$PARAM" > fp_memcasemem_boyer.out
valgrind --tool=callgrind --callgrind-out-file=strcasestr.calls ./strcasestr "$PARAM" > strcasestr.out
valgrind --tool=callgrind --callgrind-out-file=fp_memmem_boyer.calls ./fp_memmem_boyer "$PARAM" > fp_memmem_boyer.out
valgrind --tool=callgrind --callgrind-out-file=fp_memmem_brute.calls ./fp_memmem_brute "$PARAM" > fp_memmem_brute.out
valgrind --tool=callgrind --callgrind-out-file=memmem.calls ./memmem "$PARAM" > memmem.out
valgrind --tool=callgrind --callgrind-out-file=strstr.calls ./strstr "$PARAM" > strstr.out
valgrind --tool=callgrind --callgrind-out-file=strchr.calls ./strchr "$PARAM" > strchr.out
valgrind --tool=callgrind --callgrind-out-file=memchr.calls ./memchr "$PARAM" > memchr.out

# Annotate the calls
callgrind_annotate fp_memcasechr.calls > fp_memcasechr.annotated
callgrind_annotate fp_memcasemem_boyer.calls > fp_memcasemem_boyer.annotated
callgrind_annotate strcasestr.calls > strcasestr.annotated
callgrind_annotate fp_memmem_boyer.calls > fp_memmem_boyer.annotated
callgrind_annotate fp_memmem_brute.calls > fp_memmem_brute.annotated
callgrind_annotate memmem.calls > memmem.annotated
callgrind_annotate strstr.calls > strstr.annotated
callgrind_annotate strchr.calls > strchr.annotated
callgrind_annotate memchr.calls > memchr.annotated

# Use the output file to generate a report
echo -n "fp_memcasechr:       "
cat fp_memcasechr.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat fp_memcasechr.out
echo
echo -n "fp_memcasemem_boyer: "
cat fp_memcasemem_boyer.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat fp_memcasemem_boyer.out
echo
echo -n "strcasestr:          "
cat strcasestr.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat strcasestr.out
echo
echo -n "fp_memmem_boyer:     "
cat fp_memmem_boyer.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat fp_memmem_boyer.out
echo
echo -n "fp_memmem_brute:     "
cat fp_memmem_brute.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat fp_memmem_brute.out
echo
echo -n "memmem:              "
cat memmem.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat memmem.out
echo
echo -n "strstr:              "
cat strstr.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat strstr.out
echo
echo -n "strchr:              "
cat strchr.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat strchr.out
echo
echo -n "memchr:              "
cat memchr.annotated | grep "PROGRAM TOTALS" | cut -f 1 -d\ 
cat memchr.out
echo
