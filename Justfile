alias b := build

build:
    mkdir -p ./build/
    clang -Isrc src/main.c -o ./build/spaced -std=c99
