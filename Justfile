alias b := build
alias g := generate

generate:
    mkdir -p ./src/generated
    python3 ./x.py

build: generate
    mkdir -p ./build/
    clang -Isrc src/main.c -o ./build/spaced -std=c99 -g

hello-world:
    cc65 ./examples/c/hello-world/main.c
    ca65 ./examples/c/hello-world/main.s
    ld65 -t none ./examples/c/hello-world/main.o -o ./examples/c/hello-world/main.bin none.lib
    xxd -p ./examples/c/hello-world/main.bin
