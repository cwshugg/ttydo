#!/bin/bash
# A simple shell script that runs ttydo with valgrind

script_dir="$(dirname \"$0\")"

cd $script_dir/..
# compile with debug symbols
make clean all-debug;
# run with valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ttydo $@

