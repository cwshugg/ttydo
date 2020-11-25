# A simple makefile to build ttydo
#
#   Connor Shugg

CC=clang
CC_ARGS=-Wall -Werror
SOURCE_DIR=./src
SOURCE_VISUAL_DIR=$(SOURCE_DIR)/visual

default: all

all:
	$(CC) $(CC_ARGS) $(SOURCE_DIR)/*.c $(SOURCE_VISUAL_DIR)/*.c -o ttydo

all-debug:
	$(CC) $(CC_ARGS) -g $(SOURCE_DIR)/*.c $(SOURCE_VISUAL_DIR)/*.c -o ttydo

clean:
	rm -f ttydo

