#!/usr/bin/env python3
import sys


def print_error(line_number, err_msg):
    print(("{}, line {}: " + err_msg).format(sys.argv[1], line_number))
    print("Compilation terminated.")
    sys.exit(1)


def arg_number_success(statement, expected_args):
    if len(statement) != expected_args + 2:
        print_error(statement[-1], "READ expected {} arguments, got {}".format(expected_args, len(statement) - 2))


def tryte_check_success(tryte):
    # check tryte string is a legitimate tryte
    septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    if len(tryte) != 3:
        return False
    for char in tryte:
        if char not in septavingt_chars:
            return False
    return True


def value_to_tryte(value):
    septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    output_string = ""
    dividend = int(value)
    remainder = 0
    for i in range(3):
        remainder = dividend % 27
        dividend = dividend // 27
        if (remainder > 13):
            remainder -= 27
            dividend += 1
        elif (remainder < -13):
            remainder += 27
            dividend -= 1
        output_string += septavingt_chars[13 + remainder]
    return output_string[::-1]


def unsigned_value_to_tryte(value):
    septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    output_string = ""
    dividend = int(value) - 9841
    remainder = 0
    for i in range(3):
        remainder = dividend % 27
        dividend = dividend // 27
        if (remainder > 13):
            remainder -= 27
            dividend += 1
        elif (remainder < -13):
            remainder += 27
            dividend -= 1
        output_string += septavingt_chars[13 + remainder]
    return output_string[::-1]


def addr_to_tryte(addr, statement, arg_number):
    if addr[0] != "$":
        print_error(statement[-1], 
        "Argument {} of {} must be an address.".format(arg_number, statement[0]))
    tryte = addr[1:]
    if not tryte_check_success(tryte):
        print_error(statement[-1], 
        "Argument {} is not a valid Tryte address.".format(arg_number))
    return tryte


def reg_to_opcode(reg, statement, arg_number, tryte_opcode_start, 
    trint_opcode_start):
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", 
    "B1": "I", "B2": "H", "C0": "G", "C1": "F", "C2": "E", "D0": "D", 
    "D1": "C", "D2": "B", "E0": "A", "E1": "0", "E2": "a", "F0": "b",
    "F1": "c", "F2": "d", "G0": "e", "G1": "f", "G2": "g", "H0": "h",
    "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_registers = {"A": "D", "B": "C", "C": "B", "D": "A", 
    "E": "0", "F": "a", "G": "b", "H": "c", "I": "d"}

    reg_tryte_ID = tryte_registers.get(reg)
    reg_trint_ID = trint_registers.get(reg)
    if (reg_tryte_ID is None and reg_trint_ID is None):
        print_error(statement[-1], 
        "Argument {} of {} must be a register.".format(arg_number, statement[0]))
    elif reg_tryte_ID:
        opcode = tryte_opcode_start + reg_tryte_ID
    elif reg_trint_ID:
        opcode = trint_opcode_start + reg_trint_ID
    return opcode


def READ(statement):
    arg_number_success(statement, 2)
    opcode = reg_to_opcode(statement[2], statement, 2, "AM", "Am")
    add1 = addr_to_tryte(statement[1], statement, 1)
    return [opcode, add1]


def WRITE(statement):
    arg_number_success(statement, 2)
    opcode = reg_to_opcode(statement[1], statement, 1, "AA", "Aa")
    add1 = addr_to_tryte(statement[2], statement, 2)
    return [opcode, add1]


def LOAD(statement):
    arg_number_success(statement, 3)
    add1 = addr_to_tryte(statement[1], statement, 1)
    val1 = unsigned_value_to_tryte(statement[2])
    add2 = addr_to_tryte(statement[3], statement, 3)
    return ["00M", add1, val1, add2]


def SAVE(statement):
    arg_number_success(statement, 3)
    add1 = addr_to_tryte(statement[1], statement, 1)
    val1 = unsigned_value_to_tryte(statement[2])
    add2 = addr_to_tryte(statement[3], statement, 3)
    return ["00m", add1, val1, add2]


def PRINT(statement):
    arg_number_success(statement, 2)
    add1 = addr_to_tryte(statement[1], statement, 1)
    try:
        int(statement[2])
        val1 = unsigned_value_to_tryte(int(statement[2]) - 9841)
        print(["D00", add1, val1])
        return ["D00", add1, val1]
    except ValueError:
        raise ValueError("PRINT: Argument 2 must be an integer.")


def SHOW(statement):
    arg_number_success(statement, 1)
    opcode = reg_to_opcode(statement[1], statement, 1, "DB", "Db")
    return [opcode]


def TELL(statement):
    arg_number_success(statement, 1)
    opcode = reg_to_opcode(statement[1], statement, 1, "DA", "Da")
    return [opcode]


def PEEK(statement):
    arg_number_success(statement, 1)
    opcode = reg_to_opcode(statement[1], statement, 1, "BA", "Ba")
    return [opcode]


def FILL(statement):
    arg_number_success(statement, 3)
    add1 = addr_to_tryte(statement[1], statement, 1)
    val1 = unsigned_value_to_tryte(statement[2])
    val2 = value_to_tryte(statement[3])
    return ["Bm0", add1, val1, val2]


def MOUNT(statement):
    arg_number_success(statement, 1)
    opcode_start = "B0"
    septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    val = int(statement[1])
    if not (0 <= val <= 26):
        print_error(statement[-1], "MOUNT value must be between 0 and 26.")
    opcode = opcode_start + septavingt_chars[val]
    return [opcode]


def PUSH(statement):
    arg_number_success(statement, 1)
    opcode = reg_to_opcode(statement[1], statement, 1, "BB", "Bb")
    return [opcode]


def POP(statement):
    arg_number_success(statement, 1)
    opcode = reg_to_opcode(statement[1], statement, 1, "BC", "Bc")
    return [opcode]


def WHERE(statement):
    arg_number_success(statement, 1)
    reg1 = statement[1]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    if (reg1 in tryte_registers):
        opcode = "C0" + tryte_registers[reg1]
    else:
        print_error(statement[-1], "Unrecognised tryte register in WHERE statement.")
    return [opcode]


def SET(statement):
    arg_number_success(statement, 2)
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"

    reg1 = statement[1]
    if reg1 in tryte_registers:
        if statement[2] in tryte_registers:
            reg1_pos = trint_register_names.find(reg1)
            reg2_pos = trint_register_names.find(reg2)
            num = 9 * reg1_pos + reg2_pos
            opcode = value_to_tryte(num - 4*81)
            opcode = "j" + opcode[1:]
            return opcode
        elif statement[2][0] == "$":
            add1 = addr_to_tryte(statement[2][:-1])
            opcode = "ma" + tryte_registers[reg1]
            return [opcode, add1]
        else:
            try:
                int(statement[2])
            except ValueError:
                print_error(statement[-1], "Argument 2 of SET statement must be a register, address or integer.")
            opcode = "mb" + tryte_registers[reg1]
            val1 = value_to_tryte(statement[2])
            return [opcode, val1]
    elif reg1 in trint_register_names:
        if statement[2] in trint_register_names:
            reg1_pos = trint_register_names.find(reg1)
            reg2_pos = trint_register_names.find(reg2)
            num = 9 * reg1_pos + reg2_pos
            opcode = value_to_tryte(num - 4*81)
            opcode = "j" + opcode[1:]
            return opcode
        elif statement[2][0] == "$":
            add1 = addr_to_tryte(statement[2][:-1])
            reg1_pos = trint_register_names.find(reg1)
            opcode_chars = "DBCA0abcd"
            opcode = "mA" + opcode_chars[reg1_pos]
            return [opcode, add1]
        else:
            try:
                int(statement[2])
            except ValueError:
                print_error(statement[-1], "Argument 2 of SET statement must be a register, address or integer.")
            reg1_pos = trint_register_names.find(reg1)
            opcode_chars = "DBCA0abcd"
            opcode = "mB" + tryte_registers[reg1]
            val1 = value_to_tryte(statement[2])
            return [opcode, val1]
    else:
        print_error(statement[-1], "Argument 1 of SET statement must be a register.")


def SWAP(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "i" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num + 4*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in SWAP statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in SWAP statement.")
    return [opcode]


def CCMP(statement):
    arg_number_success(statement, 0)
    return ["00b"]


def CCAR(statement):
    arg_number_success(statement, 0)
    return ["00c"]


def COVF(statement):
    arg_number_success(statement, 0)
    return ["00d"]


def PRI(statement):
    arg_number_success(statement, 1)
    opcode = "0M"
    septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    val = int(statement[1])
    if not (-13 <= val <= 13):
        print_error(statement[-1], "Mount value must be between -13 and 13.")
    opcode += septavingt_chars[13 + val]
    return [opcode]


def CHK(statement):
    arg_number_success(statement, 0)
    return ["0m0"]


def FLIP(statement):
    arg_number_success(statement, 1)
    reg = statement[1]
    opcode = reg_to_opcode(reg, statement, 1, "kc", "kC")
    return [opcode]


def INC(statement):
    arg_number_success(statement, 1)
    reg = statement[1]
    opcode = reg_to_opcode(reg, statement, 1, "kd", "kD")
    return [opcode]


def DEC(statement):
    arg_number_success(statement, 1)
    reg = statement[1]
    opcode = reg_to_opcode(reg, statement, 1, "ke", "kE")
    return [opcode]


def ADD(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "c" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num - 2*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in ADD statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in ADD statement.")
    return [opcode]


def MUL(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "d" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num - 1*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in MUL statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in MUL statement.")
    return [opcode]


def DIV(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "e" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in DIV statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in DIV statement.")
    return [opcode]


def SHL(statement):
    arg_number_success(statement, 2)
    opcode = reg_to_opcode(statement[1], statement, 1, "ka", "kA")
    val1 = unsigned_value_to_tryte(statement[2])
    return [opcode, val1]


def SHR(statement):
    arg_number_success(statement, 2)
    opcode = reg_to_opcode(statement[1], statement, 1, "kb", "kB")
    val1 = unsigned_value_to_tryte(statement[2])
    return [opcode, val1]


def CMP(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "b" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num - 3*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in CMP statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in CMP statement.")
    return [opcode]


def AND(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "f" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num + 1*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in AND statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in AND statement.")
    return [opcode]


def OR(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "g" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num + 2*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in OR statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in OR statement.")
    return [opcode]


def XOR(statement):
    arg_number_success(statement, 2)
    reg1 = statement[1]
    reg2 = statement[2]
    tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", "B1": "I", "B2": "H",
    "C0": "G", "C1": "F", "C2": "E", "D0": "D", "D1": "C", "D2": "B", "E0": "A",
    "E1": "0", "E2": "a", "F0": "b", "F1": "c", "F2": "d", "G0": "e", "G1": "f",
    "G2": "g", "H0": "h", "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
    trint_register_names = "ABCDEFGHI"
    if reg1 in tryte_registers and reg2 in tryte_registers:
        opcode = "h" + tryte_registers[reg1] + tryte_registers[reg2]
    elif reg1 in trint_register_names and reg2 in trint_register_names:
        reg1_pos = trint_register_names.find(reg1)
        reg2_pos = trint_register_names.find(reg2)
        num = 9 * reg1_pos + reg2_pos
        opcode = value_to_tryte(num + 3*81)
        opcode = "j" + opcode[1:]
    elif len(reg1) != len(reg2):
        print_error(statement[-1], "Register types must match in XOR statement.")
    else:
        print_error(statement[-1], "Unrecognised registers in XOR statement.")
    return [opcode]


def NOT(statement):
    arg_number_success(statement, 1)
    opcode = reg_to_opcode(statement[1], statement, 1, "Mm", "MM")
    return [opcode]


def NOOP(statement):
    arg_number_success(statement, 0)
    return ["00a"]


def JPZ(statement):
    arg_number_success(statement, 1)
    add1 = addr_to_tryte(statement[1], statement, 1)
    return ["0a0", add1]


def JPN(statement):
    arg_number_success(statement, 1)
    add1 = addr_to_tryte(statement[1], statement, 1)
    return ["0b0", add1]


def JPP(statement):
    arg_number_success(statement, 1)
    add1 = addr_to_tryte(statement[1], statement, 1)
    return ["0c0", add1]


def JP(statement):
    arg_number_success(statement, 1)
    add1 = addr_to_tryte(statement[1], statement, 1)
    return ["0d0", add1]


def JPS(statement):
    arg_number_success(statement, 1)
    add1 = addr_to_tryte(statement[1], statement, 1)
    return ["0e0", add1]


def PJP(statement):
    arg_number_success(statement, 0)
    return ["0E0"]


def THD(statement):
    arg_number_success(statement, 1)
    opcode = "0M"
    septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    val = int(statement[1])
    if not (-13 <= val <= 13):
        print_error(statement[-1], "THD value must be between -13 and 13.")
    opcode += septavingt_chars[13 + val]
    return [opcode]


def INT(statement):
    arg_number_success(statement, 2)
    opcode = "0g"
    val = int(statement[1])
    if not (-13 <= val <= 13):
        print_error(statement[-1], "THD value must be between -13 and 13.")
    opcode += septavingt_chars[13 + val]
    addr = addr_to_tryte(statement[2], statement, 2)
    return [opcode, addr]


def HALT(statement):
    arg_number_success(statement, 0)
    return ["000"]


def WAIT(statement):
    arg_number_success(statement, 0)
    return ["00a"]


def CALL(statement):
    arg_number_success(statement, 1)
    fn_name = statement[1]
    # convert to actual address at linking stage
    # convert CALL $func1 to
    # JPS $func1 = 0e0 $func1
    # this jumps to $func1, and pushes the instruction pointer onto the stack
    return ["0e0", "$" + fn_name]


def STRWRT(statement):
    arg_number_success(statement, 2)
    input_str = statement[1]
    add1 = addr_to_tryte(statement[2], statement, 2)
    septavingt_chars = "MLKJHIGFEDCBA0abcdefghijklm"
    add1_value = (729 * (septavingt_chars.find(add1[0]) - 13) 
    + 27 * (septavingt_chars.find(add1[1]) - 13) 
    + (septavingt_chars.find(add1[2]) - 13))
    output_tryte_string = []
    output_instr_list = []
    if (len(input_str) % 2 == 1):
        # if input_str has odd length, pad it with zero
        new_input_str = input_str + '\0'
        input_str = new_input_str
    else:
        # if input_str has even length, pad it with two zeroes
        new_input_str = input_str + '\0\0'
        input_str = new_input_str
    
    # convert string into list of trytes
    for i in range(len(input_str) // 2):
        char1 = input_str[2*i]
        char2 = input_str[2*i + 1]
        output_tryte_string.append(value_to_tryte(128*ord(char1) + ord(char2) - 9841))
    
    # generate list of instructions for print this string
    offset = 0
    for tryte in output_tryte_string:
        # SET I2, n
        output_instr_list += ["mbm", tryte]
        # WRITE I2, $add1+offset
        output_instr_list += ["AAm", value_to_tryte(add1_value + offset)]
        offset += 1

    return output_instr_list


def STRPNT(statement):
    arg_number_success(statement, 1)
    input_str = statement[1]

    # pad input_str with zeroes
    output_tryte_string = []
    if (len(input_str) % 2 == 1):
        # if input_str has odd length, pad it with zero
        new_input_str = input_str + '\0'
        input_str = new_input_str
    else:
        # if input_str has even length, pad it with two zeroes
        new_input_str = input_str + '\0\0'
        input_str = new_input_str
    
    for i in range(len(input_str) // 2):
        char1 = input_str[2*i]
        char2 = input_str[2*i + 1]
        output_tryte_string.append(value_to_tryte(128*ord(char1) + ord(char2) - 9841))
    
    # now one by one send to register I2 and SHOW them to the console
    output_instr_list = []
    for tryte in output_tryte_string:
        # SET I2, n
        output_instr_list += ["mbm", tryte]
        # SHOW I2
        output_instr_list += ["DBm"]
    
    return output_instr_list
