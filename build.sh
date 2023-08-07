#!/usr/bin/env sh

set -xe

CFLAGS="-std=c99 -Wall -Wextra -pedantic"

# Clang compiles, but does not work!!
gcc $CFLAGS ./main.c -o target/main
gcc $CFLAGS ./examples/cargo.c -o target/example_cargo
gcc $CFLAGS ./examples/date.c -o target/example_date
