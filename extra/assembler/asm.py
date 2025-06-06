import argparse
from collections import OrderedDict
from pprint import pprint

from compile import compile
from parser import parse


def main(args):
    file = open(args.filename, 'r')
    content = file.read()
    file.close()

    parsed = parse(content)

    compiled = compile(parsed)
    pprint(compiled)

    with open('o.bin', 'wb') as f:
        f.write(compiled)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="asm", description="Compile the assembly into bytecode"
    )

    parser.add_argument('filename')

    args = parser.parse_args()

    main(args)
