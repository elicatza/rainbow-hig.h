#!/usr/bin/env sh

set -xe

CFLAGS="-std=c99 -Wall -Wextra -pedantic"

# Clang compiles, but does not work!!
gcc $CFLAGS ./main.c -o target/main
