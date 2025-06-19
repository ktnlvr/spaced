alias b := build-sim-6502
alias g := generate
alias sim := run-6502-example
alias test := run-tests

default:
    just --list

run-client: _build-warmup
    clang -Isrc src/executables/client.c -o ./build/client -std=c99 -g -lm -fsanitize=address -lglfw -lGL -lX11 -lpthread -lXrandr -ldl -lGLEW
    ./build/client

test-map: _build-warmup
    mkdir -p ./build/test
    clang -Isrc src/executables/test/map.c -o ./build/test/map -std=c99 -g -lm -fsanitize=address
    python3 x.py test ./build/test/map -t ds/map

run-tests: _build-warmup
    mkdir -p ./build/test
    clang -Isrc src/executables/test/physics.c -o ./build/test/physics -std=c99 -g -lm -fsanitize=address
    python3 x.py test ./build/test/physics -t physics/sat

run-6502-example example: build-sim-6502 build-stdlib
    ./build/spaced ./build/{{example}}/main.bin
    xxd -g2 dump.bin | grep -v '0000 0000 0000 0000 0000 0000 0000 0000'

_build-warmup:
    mkdir -p ./build/

build-sim-6502: generate _build-warmup
    clang -Isrc src/executables/sim6502.c -o ./build/spaced -std=c99 -g

generate:
    mkdir -p ./src/6502/generated
    python3 ./x.py gen

build-example-asm name: _build-warmup build-stdlib
    mkdir -p build/{{name}}
    ca65 --cpu 6502 examples/asm/{{name}}.s -o build/{{name}}/main.o
    ld65 -C target/memory.cfg -m build/{{name}}/main.map build/{{name}}/main.o -o build/{{name}}/main.bin target/stdlib.lib build/stdlib/vectors.o build/stdlib/interrupt.o
    da65 build/{{name}}/main.bin > build/{{name}}/disassembled.s
    just xxd-ignore-zeros build/{{name}}/main.bin

build-example-c name: _build-warmup build-stdlib
    mkdir -p ./build/{{name}}
    cc65 -t none --cpu 6502 examples/c/{{name}}.c -o build/{{name}}/main.s
    ca65 --cpu 6502 build/{{name}}/main.s
    ld65 -C target/memory.cfg -m build/{{name}}/main.map build/{{name}}/main.o -o build/{{name}}/main.bin target/stdlib.lib build/stdlib/vectors.o build/stdlib/interrupt.o
    da65 build/{{name}}/main.bin > build/{{name}}/disassembled.s
    just xxd-ignore-zeros build/{{name}}/main.bin
    @echo "Done building the example {{name}}!"

build-stdlib: _build-warmup
    mkdir -p build/stdlib
    ca65 --cpu 6502 target/crt0.s -o build/stdlib/crt0.o
    ca65 --cpu 6502 target/vectors.s -o build/stdlib/vectors.o
    ca65 --cpu 6502 target/interrupt.s -o build/stdlib/interrupt.o
    ar65 a build/stdlib/stdlib.lib build/stdlib/crt0.o

xxd-ignore-zeros filename:
    @echo "Printing the dump of {{filename}}"
    @echo "Careful! Zero lines not printed." 
    xxd -g2 {{filename}} | rg -v '0000 0000 0000 0000 0000 0000 0000 0000'
