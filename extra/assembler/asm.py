import argparse
from collections import OrderedDict

from ops import str_to_op, str_to_arg
from bin import binarize

def main(args):
    file = open(args.filename, 'r')
    lines = file.readlines()
    file.close()

    labels = {}
    code = OrderedDict()

    for i, line in enumerate(lines):
        line = line.strip()

        if not line:
            continue

        if line.startswith('--'):
            # Ignore comments
            continue

        if line.startswith('.'):
            label = line[1:]
            labels[label] = i
            continue

        op, *args = line.split()
        op = str_to_op(op)
        args = list(map(str_to_arg, args))
        code[i] = (op, *args)

    print(labels)
    print(code)

    b = binarize(labels, code)
    with open('o.bin', 'wb+') as o:
        o.write(b)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="asm", description="Compile the assembly into bytecode"
    )

    parser.add_argument('filename')

    args = parser.parse_args()

    main(args)
