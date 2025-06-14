from collections import defaultdict
from argparse import ArgumentParser
from itertools import chain, repeat
from dataclasses import dataclass
from enum import StrEnum
import subprocess
import csv


class AddressingMode(StrEnum):
    ZeroPage = "ZERO_PAGE"
    ZeroPageX = "ZERO_PAGE_X"
    ZeroPageY = "ZERO_PAGE_Y"
    Indirect = "INDIRECT"
    Immediate = "IMMEDIATE"
    Absolute = "ABSOLUTE"
    AbsoluteX = "ABSOLUTE_X"
    AbsoluteY = "ABSOLUTE_Y"
    XIndirect = "X_INDIRECT"
    IndirectY = "INDIRECT_Y"
    Accumulator = "ACCUMULATOR"
    Relative = "RELATIVE"
    Implied = "IMPLIED"


modes = {
    "zpg": AddressingMode.ZeroPage,
    "zpg,X": AddressingMode.ZeroPageX,
    "zpg,Y": AddressingMode.ZeroPageY,
    "abs": AddressingMode.Absolute,
    "abs,X": AddressingMode.AbsoluteX,
    "abs,Y": AddressingMode.AbsoluteY,
    "ind": AddressingMode.Indirect,
    "X,ind": AddressingMode.XIndirect,
    "ind,Y": AddressingMode.IndirectY,
    "#": AddressingMode.Immediate,
    "A": AddressingMode.Accumulator,
    "rel": AddressingMode.Relative,
    "impl": AddressingMode.Implied,
}

# TODO: remove me when all instructions are supported
export_only = {
    "LDX",
    "LDA",
    "JMP",
    "RTS",
    "STA",
    "STX",
    "JSR",
    "PHA",
    "PLA",
    "LDY",
    "BEQ",
    "TAY",
    "INY",
    "BNE",
    "INC",
    "DEX",
    "CPY",
    "DEY",
    "STY",
    "SEC",
    "SBC",
    "BCS",
    "DEC",
    "TXA",
    "TAX",
    "CMP",
    "BVC",
    "EOR",
    "ASL",
    "ROL",
    "CLC",
    "TYA",
    "ADC",
    "BCC",
    "BMI",
    "NOP",
    "SEI",
    "CLI",
    "SED",
    "CLV",
}


def gen(*args, **kwargs):
    global modes
    global export_only

    unique_modes = set()
    unique_instructions = set()

    instruction_modes = defaultdict(list)
    instruction_opcodes = dict()

    with open("external/instructions.csv", "r") as f:
        lines = list(csv.reader(f))

        for i in range(16):
            for j in range(16):
                n = i * 16 + j
                line = lines[i][j]
                if line == "---":
                    continue
                op, mode = line.split()

                unique_instructions.add(op)

                if mode not in modes:
                    unique_modes.add(mode)

                if op in export_only:
                    instruction_modes[op].append(modes[mode])
                    instruction_opcodes[(op, modes[mode])] = n

    assert len(unique_modes) == 0

    print("Exporting... ")
    print(", ".join(export_only))
    print("Ignoring...")
    print(", ".join(unique_instructions.difference(export_only)))

    functions = []

    decode_cases = []

    str_cases = []

    func_name = lambda op: f"chip_op_{op.lower()}"

    for op in export_only:
        functions.append((func_name(op), instruction_modes[op]))

    for op, modes in instruction_modes.items():

        if op in export_only:
            for mode in modes:
                n = instruction_opcodes[(op, mode)]
                str_case = f"""  case (0x{n:02X}): {{
    return "{op} {mode.lower()}";
  }}"""
                str_cases.append(str_case)

                decode_case = f"""  case (0x{n:02X}): {{
    {func_name(op)}(self{f", ADDR_MODE_{mode}" if len(modes) > 1 else ""});
    break;
  }}"""
                decode_cases.append(decode_case)

    functions = "\n\n".join(
        "static void "
        + f
        + f"(chip_t *{', addressing_mode_t' if len(modes) > 1 else ''});"
        for f, modes in functions
    )
    decode_cases = "\n".join(decode_cases)
    str_cases = "\n".join(str_cases)

    instruction_lookup_template = f"""
#ifndef __SPACED_H__INSTRUCTION_LOOKUP__
#define __SPACED_H__INSTRUCTION_LOOKUP__

// This code is autogenerated using x.py
// Hopefully the massive switchcase gets optimized
// into a nice manageable table

#include "../../defs.h"
#include "../addressing.h"
#include "../chip.h"

{functions}

static void chip_decode_execute(chip_t *self, byte opcode) {{
  switch (opcode) {{
{decode_cases}
  default:
    PANIC("Unknown opcode 0x%02X at PC=0x%04X", opcode, self->pc - 1);
  }}
}}

static const char *opcode_to_str(byte opcode) {{
  switch (opcode) {{
{str_cases}
  }}
  return 0;
}}

#endif
""".strip()

    with open("./src/6502/generated/lookup.h", "w+") as f:
        f.write(instruction_lookup_template)


def chunks(ls, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(ls), n):
        yield ls[i : i + n]


def test(args):
    executable_name = args.exepath
    tests = args.test

    @dataclass
    class TestCase:
        name: str
        given_raw: str
        given: str
        expected_raw: str
        expected: str

    for test in tests:
        cases = []
        content = []

        with open(f"tests/{test}.txt", "r") as f:
            content = f.read().strip().split("\n\n")

        for [name, given, expected] in chunks(content, 3):
            name = name.strip()

            def filter_out_comments(lines):
                return [line.split(";")[0].strip() for line in lines]

            given_raw = given.splitlines()
            given = filter_out_comments(given_raw)
            expected_raw = expected.splitlines()
            expected = filter_out_comments(expected_raw)

            case = TestCase(name, given_raw, given, expected_raw, expected)
            cases.append(case)

        print(f"found {len(cases)} test cases in {test}!")

        for case in cases:
            print(f"running {case.name}...")
            process = subprocess.Popen(
                executable_name,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
            )

            inputs = '\n'.join(case.given) + '\n'
            outputs = '\n'.join(case.expected)

            stdout, stderr = process.communicate(input=inputs)
            inputs = inputs.strip()
            stdout = stdout.strip()
            stderr = stderr.strip()
            if stdout != outputs:
                print("not ok!")
                print("given:\n" + inputs)
                print("expected:\n" + outputs)
                print("stdout:\n" + stdout)
                print("stderr:\n" + stderr)
            else:
                print("ok.")


if __name__ == "__main__":
    parser = ArgumentParser(prog="x.py")
    subparsers = parser.add_subparsers(
        title="subcommands", dest="subcommand", required=True
    )

    subparsers.add_parser("gen").set_defaults(func=gen)

    test_parser = subparsers.add_parser("test")
    test_parser.add_argument("-t", "--test", nargs="+", required=True)
    test_parser.add_argument("exepath")
    test_parser.set_defaults(func=test)

    args = parser.parse_args()
    args.func(args)
