#!/bin/sh

# Test the common cases here -- each search function under each type.
../../fileperuser -o script_results1 -d test_file "object "
../../fileperuser -o script_results2 -d test_file cake
../../fileperuser -o script_results3 -d test_file alt
../../fileperuser -o script_results4 -d test_file q
../../fileperuser -o script_nocase_results1 -n -d test_file quest
../../fileperuser -o script_nocase_results2 -n -d test_file ary
../../fileperuser -o script_nocase_results3 -n -d test_file d
../../fileperuser -o script_single_results1 -1 -d test_file speed_left
../../fileperuser -o script_single_results2 -1 -d test_file Str
../../fileperuser -o script_single_results3 -1 -d test_file j
../../fileperuser -o script_single_nocase_results1 -1 -n -d test_file attacks
../../fileperuser -o script_single_nocase_results2 -1 -n -d test_file Pi
../../fileperuser -o script_single_nocase_results3 -1 -n -d test_file r
# Make a set of cases designed to catch the failure to search the end of the file.

#
# IMPORTANT: If the test file changes, these need to be adjusted to match the
# new end of the test file. That is what they are testing for, anyway.
#

../../fileperuser -o script_testend -d test_file "end
"
../../fileperuser -o script_single_testend -1 -d test_file "end
"
../../fileperuser -o script_nocase_testend -n -d test_file "end
"
../../fileperuser -o script_single_nocase_testend -1 -n -d test_file "end
"
../../fileperuser -o script_shortend -d test_file "
"
../../fileperuser -o script_nocase_shortend -n -d test_file "
"
../../fileperuser -o script_single_shortend -1 -d test_file "
"
../../fileperuser -o script_single_nocase_shortend -1 -n -d test_file "
"
# These last couple are for case-sensitive needles to use boyer-moore.
# We don't need to check case-insensitive since we already got the boyer-moore check.
../../fileperuser -o script_longend -d test_file "ons
end
"
../../fileperuser -o script_single_longend -1 -d test_file "ons
end
"
# Add binary searches to the list. Current implementation of binary file detection
# should not affect the single vs. multi match.
../../fileperuser -o script_binary_nocase_short -b -n -d test_file p
../../fileperuser -o script_binary_nocase_medium -b -n -d test_file po
../../fileperuser -o script_binary_nocase_long -b -n -d test_file "haystack_last"
../../fileperuser -o script_binary_short -b -d test_file ";"
../../fileperuser -o script_binary_medium -b -d test_file try
../../fileperuser -o script_binary_long -b -d test_file creation

# Do more test cases on the end.
../../fileperuser -o script_binary_nocase_shortend -b -n -d test_file "
"
../../fileperuser -o script_binary_nocase_testend -b -n -d test_file "end
"
../../fileperuser -o script_binary_nocase_longend -b -n -d test_file "ons
end
"
../../fileperuser -o script_binary_shortend -b -d test_file "
"
../../fileperuser -o script_binary_testend -b -d test_file "end
"
../../fileperuser -o script_binary_longend -b -d test_file "ons
end
"


# Compile the differences between the actual results and the expected results
diff script_results1 test_results1 > diff_file
diff script_results2 test_results2 >> diff_file
diff script_results3 test_results3 >> diff_file
diff script_results4 test_results4 >> diff_file
diff script_nocase_results1 test_nocase_results1 >> diff_file
diff script_nocase_results2 test_nocase_results2 >> diff_file
diff script_nocase_results3 test_nocase_results3 >> diff_file
diff script_single_results1 test_single_results1 >> diff_file
diff script_single_results2 test_single_results2 >> diff_file
diff script_single_results3 test_single_results3 >> diff_file
diff script_single_nocase_results1 test_single_nocase_results1 >> diff_file
diff script_single_nocase_results2 test_single_nocase_results2 >> diff_file
diff script_single_nocase_results3 test_single_nocase_results3 >> diff_file
diff script_binary_nocase_short test_binary_nocase_short >> diff_file
diff script_binary_nocase_medium test_binary_nocase_medium >> diff_file
diff script_binary_nocase_long test_binary_nocase_long >> diff_file
diff script_binary_short test_binary_short >> diff_file
diff script_binary_medium test_binary_medium >> diff_file
diff script_binary_long test_binary_long >> diff_file
# And these for the end case
diff script_testend test_testend >> diff_file
diff script_single_testend test_single_testend >> diff_file
diff script_nocase_testend test_nocase_testend >> diff_file
diff script_single_nocase_testend test_single_nocase_testend >> diff_file
diff script_shortend test_shortend >> diff_file
diff script_single_shortend test_single_shortend >> diff_file
diff script_nocase_shortend test_nocase_shortend >> diff_file
diff script_single_nocase_shortend test_single_nocase_shortend >> diff_file
diff script_longend test_longend >> diff_file
diff script_single_longend test_single_longend >> diff_file
diff script_binary_nocase_shortend test_binary_nocase_shortend >> diff_file
diff script_binary_nocase_testend test_binary_nocase_testend >> diff_file
diff script_binary_nocase_longend test_binary_nocase_longend >> diff_file
diff script_binary_shortend test_binary_shortend >> diff_file
diff script_binary_testend test_binary_testend >> diff_file
diff script_binary_longend test_binary_longend >> diff_file

# Show the differences
RES=`cat diff_file`
# Make sure that there was nothing in diff_file
if (test "x" != "x$RES")
then
    echo "Failed to assert search was correct in your current build."
    # Give a failure code to indicate failure for autimation
    exit 1
fi
# We did it -- remove the generated files
rm script_results1 script_results2 script_results3 script_results4 script_nocase_results1 \
script_nocase_results2 script_nocase_results3 script_single_results1 script_single_results2 \
script_single_results3 script_single_nocase_results1 script_single_nocase_results2 \
script_single_nocase_results3 script_testend script_single_testend script_nocase_testend \
script_single_nocase_testend script_shortend script_single_shortend script_nocase_shortend \
script_single_nocase_shortend script_longend script_single_longend script_binary_short \
script_binary_nocase_short script_binary_shortend script_binary_nocase_shortend \
script_binary_medium script_binary_nocase_medium script_binary_testend \
script_binary_nocase_testend script_binary_long script_binary_nocase_long \
script_binary_longend script_binary_nocase_longend diff_file
# exit successfully
exit 0
