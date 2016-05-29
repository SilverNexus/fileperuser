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
 script_single_nocase_results3 diff_file
# exit successfully
exit 0
