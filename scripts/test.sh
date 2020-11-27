#!/bin/bash
# A simple bash script that compiles and executes testing modules
#
#   Connor Shugg

# directories
dir_scripts=$(dirname "$0")
dir_tests=$dir_scripts/../tests
dir_src=$dir_scripts/../src
# colors
c_none="\033[0m"
c_red="\033[0;31m"
c_green="\033[0;32m"
c_yellow="\033[1;33m"

# if no argument was given, print a message
if [ $# -lt 1 ]; then
    echo -e "${c_red}Error${c_none}: No test name given. Invocation: \"test.sh <test_module_name>\""
    echo ""

    # iterate and print all test names
    echo "You can specify the following tests:"
    for test_file in $dir_tests/*.c; do
        echo -e "- ${c_yellow}$(basename ${test_file%.*})${c_none}"
    done

    exit 1
fi

# run the makefile's 'make test' option
echo -e "${c_yellow}Compiling test...${c_none}"
cd $dir_tests/..
make test TEST=$dir_tests/$1.c

# if the binary wasn't created, quit
if [ ! -f ./TEST ]; then
    echo -e "${c_red}Compilation failed.${c_none}"
    exit 2
else
    echo -e "${c_green}Compilation successful.${c_none}"
fi

# move the 'TEST' binary into ./tests
mv ./TEST $dir_tests/TEST

# execute the test
echo -e "${c_yellow}Executing test...${c_none}"
cd $dir_tests
./TEST

# remove the executable once finished
rm -f ./TEST

