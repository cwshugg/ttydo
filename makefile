# A simple makefile to build ttydo
#
#   Connor Shugg

CC=clang
CC_ARGS=-Wall -Werror
SOURCE_DIR=./src

default: all

all:
	$(CC) $(CC_ARGS) $(SOURCE_DIR)/*.c -o ttydo

clean:
	rm -f ttydo

