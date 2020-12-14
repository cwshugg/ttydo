# A simple makefile to build ttydo
#
#   Connor Shugg

CC=clang
CC_ARGS=-Wall -Werror
# source files
SOURCE_DIR=./src
SOURCE_VISUAL_DIR=$(SOURCE_DIR)/visual
SOURCE_CLI_DIR=$(SOURCE_DIR)/cli
SOURCE_TEST_DIR=./tests
SOURCE_ARGS=$(SOURCE_DIR)/*.c $(SOURCE_VISUAL_DIR)/*.c $(SOURCE_CLI_DIR)/*.c
# testing
TEST=you_need_to_specify_a_C_source_file_for_TEST_SOURCE

default: all

all:
	$(CC) $(CC_ARGS) $(SOURCE_ARGS) -o ttydo

all-debug:
	$(CC) $(CC_ARGS) -g $(SOURCE_ARGS) -o ttydo

test:
	$(CC) $(CC_ARGS) -g $(SOURCE_ARGS) $(TEST) -o TEST

clean:
	rm -f ttydo

