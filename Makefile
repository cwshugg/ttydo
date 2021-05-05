# A simple makefile to build ttydo
#
#   Connor Shugg

CC=clang
CFLAGS=-Wall -Werror
# source directories
SOURCE_DIR=./src
SOURCE_VISUAL_DIR=$(SOURCE_DIR)/visual
SOURCE_CLI_DIR=$(SOURCE_DIR)/cli
SOURCE_CLI_HANDLERS_DIR=$(SOURCE_CLI_DIR)/handlers
SOURCE_TEST_DIR=./tests
# source file compiler args
SOURCE_ARGS=$(SOURCE_DIR)/*.c $(SOURCE_VISUAL_DIR)/*.c $(SOURCE_CLI_DIR)/*.c $(SOURCE_CLI_HANDLERS_DIR)/*.c
SOURCE_ARGS_NO_CLI=$(SOURCE_DIR)/*.c $(SOURCE_VISUAL_DIR)/*.c
# testing
TEST=you_need_to_specify_a_C_source_file_for_TEST_SOURCE
# installation
INSTALL_LOCATION=/usr/local/bin

default: all

all:
	$(CC) $(SOURCE_ARGS) -o ttydo

all-debug:
	$(CC) -g $(SOURCE_ARGS) -o ttydo

test:
	$(CC) -g $(SOURCE_ARGS_NO_CLI) $(TEST) -o ttydo-test

clean:
	rm -f ttydo

install:
	make all
	mv ttydo $(INSTALL_LOCATION)/ttydo

