#!/usr/bin/env python3
import sys

tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", 
    "B1": "I", "B2": "H", "C0": "G", "C1": "F", "C2": "E", "D0": "D", 
    "D1": "C", "D2": "B", "E0": "A", "E1": "0", "E2": "a", "G0": "b",
    "G1": "c", "G2": "d", "H0": "e", "H1": "f", "H2": "g", "I0": "h",
    "I1": "i", "I2": "j", "J0": "k", "J1": "l", "J2": "m"}
trint_registers = {"A": "D", "B": "C", "C": "B", "D": "A", 
    "E": "0", "G": "a", "H": "b", "I": "c", "J": "d"}
trint_register_names = "ABCDEGHIJ"
float_registers = {"F0": "D", "F1": "C", "F2": "B", "F3": "A", "F4": "0",
"F5": "a", "F6": "b", "F7": "c", "F8": "d"}
float_register_names = ["F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8"]

septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"

def print_error(line_number, err_msg):
    print(("{}, line {}: " + err_msg).format(sys.argv[1], line_number))
    print("Compilation terminated.")
    sys.exit(1)

def arg_number_check(statement, expected_args):
    if len(statement) != expected_args + 2:
        print_error(statement[-1], 
        "{} expected {} arguments, got {}".format(statement[0], expected_args, len(statement) - 2))

def arg_is_tryte_reg(arg):
    return arg in tryte_registers


def arg_is_trint_reg(arg):
    return arg in trint_registers

def arg_is_float_reg(arg):
    return arg in float_registers

def arg_is_addr(arg):
    if arg[0] == "$":
        for char in arg[1:]:
            if char not in septavingt_chars:
                return False
        
        if len(arg) != 4:
            return False
        
        return True
    else:
        return False

def arg_is_short(arg):
    try:
        int(arg)
    except ValueError:
        return False

    return 0 <= int(arg) < 27

def arg_is_unsigned_tryte_value(arg):
    try:
        int(arg)
    except ValueError:
        return False
    return 0 <= int(arg) < 19683

def arg_is_signed_tryte_value(arg):
    try:
        int(arg)
    except ValueError:
        return False
    return -9841 <= int(arg) <= 9841

def arg_is_signed_trint_value(arg):
    try:
        int(arg)
    except ValueError:
        return False
    return -(3**27 - 1) // 2 <= int(arg) <= (3**27 - 1) // 2

def arg_is_positive_large_value(arg):
    try:
        int(arg)
    except ValueError:
        return False
    return int(arg) > 0

def arg_is_float(arg):
    try:
        float(arg)
    except ValueError:
        return False
    return True

def tryte_reg_to_opcode(opcode_start, reg):
    return opcode_start + tryte_registers[reg]

def trint_reg_to_opcode(opcode_start, reg):
    return opcode_start + trint_registers[reg]

def float_reg_to_opcode(opcode_start, reg):
    return opcode_start + float_registers[reg]

def short_to_opcode(opcode_start, short):
    return opcode_start + septavingt_chars[int(short)]

def signed_value_to_tryte(arg):
    output_string = ""
    dividend = int(arg)
    remainder = 0
    for _ in range(3):
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

def unsigned_value_to_tryte(arg):
    output_string = ""
    dividend = int(arg) - 9841
    remainder = 0
    for _ in range(3):
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

def signed_trint_value_to_trint(arg):
    output_string = ""
    dividend = int(arg)
    remainder = 0
    for _ in range(9):
        remainder = dividend % 27
        dividend = dividend // 27
        if (remainder > 13):
            remainder -= 27
            dividend += 1
        elif (remainder < -13):
            remainder += 27
            dividend -= 1
        output_string += septavingt_chars[13 + remainder]
    output_string = output_string[::-1]
    return [output_string[:3], output_string[3:6], output_string[6:9]]

def float_to_tfloat(arg):
    d = float(arg)
    # calculate exponent
    exponent = 0
    if d > 0:
        while d > 1.5:
            d = d / 3
            exponent += 1
        while d < 0.5:
            d = d * 3
            exponent -= 1
    elif d < 0:
        while d < -1.5:
            d = d / 3
            exponent += 1
        while d > -0.5:
            d = d * 3
            exponent -= 1
    exponent_tryte = signed_value_to_tryte(exponent)

    # calculate mantissa from remains of d
    mantissa_trytes = signed_trint_value_to_trint(round(d * (3**17)))[1:]
    return [exponent_tryte, mantissa_trytes[0], mantissa_trytes[1]]

## Compilation functions
# 0 arguments

def HALT(statement):
    arg_number_check(statement, 0)
    return ["000"]

def NOOP(statement):
    arg_number_check(statement, 0)
    return ["00a"]

def WAIT(statement):
    arg_number_check(statement, 0)
    return ["00A"]

def CCMP(statement):
    arg_number_check(statement, 0)
    return ["0a0"]

def CCAR(statement):
    arg_number_check(statement, 0)
    return ["0aA"]

def COVF(statement):
    arg_number_check(statement, 0)
    return ["0aa"]

def PJP(statement):
    arg_number_check(statement, 0)
    return ["0jM"]

def CHK(statement):
    return ["0c0"]

# 1 argument

def PRI(statement):
    arg_number_check(statement, 1)
    if arg_is_short(statement[1]):
        opcode = short_to_opcode("0I", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must satisfy 0 <= n < 27.".format(1, statement[0]))

def THD(statement):
    arg_number_check(statement, 1)
    if arg_is_short(statement[1]):
        opcode = short_to_opcode("0h", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must satisfy 0 <= n < 27.".format(1, statement[0]))

def MOUNT(statement):
    arg_number_check(statement, 1)
    if arg_is_short(statement[1]):
        opcode = short_to_opcode("0m", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must satisfy 0 <= n < 27.".format(1, statement[0]))

# Handle all jump statements at link time (except PJP)
def JPZ(statement):
    arg_number_check(statement, 1)
    if isinstance(statement[1], str):
        return ["JPZ", statement[1]]
    else:
        print_error(statement[-1], "Argument {} of {} statement must be a string.".format(1, statement[0]))

def JPN(statement):
    arg_number_check(statement, 1)
    if isinstance(statement[1], str):
        return ["JPN", statement[1]]
    else:
        print_error(statement[-1], "Argument {} of {} statement must be a string.".format(1, statement[0]))

def JPP(statement):
    arg_number_check(statement, 1)
    if isinstance(statement[1], str):
        return ["JPP", statement[1]]
    else:
        print_error(statement[-1], "Argument {} of {} statement must be a string.".format(1, statement[0]))

def JP(statement):
    arg_number_check(statement, 1)
    if isinstance(statement[1], str):
        return ["JP", statement[1]]
    else:
        print_error(statement[-1], "Argument {} of {} statement must be a string.".format(1, statement[0]))

def JPS(statement):
    arg_number_check(statement, 1)
    if isinstance(statement[1], str):
        return ["JPS", statement[1]]
    else:
        print_error(statement[-1], "Argument {} of {} statement must be a string.".format(1, statement[0]))

def PEEK(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("bM", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("bm", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gl", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def PUSH(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("bA", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("ba", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gb", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def POP(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("bB", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("bb", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gB", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def WHERE(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("b0", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid Tryte register.".format(1, statement[0]))

def SHOW(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("cC", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("cc", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("ga", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def TELL(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("cD", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("cd", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gA", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def DSET(statement):
    arg_number_check(statement, 1)
    if arg_is_short(statement[1]):
        opcode = short_to_opcode("cm", statement[1])
        return [opcode]
    elif arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("cA", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("ca", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], 
        "Argument {} in {} statement must be a valid register or an integer satisfying 0 <= n < 27.".format(1, statement[0]))

def DGET(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("cB", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("cb", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def INC(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("Ki", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("ki", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def DEC(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("KI", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("kI", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def FLIP(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("Kf", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("kf", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gd", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def ABS(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("KA", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("kA", statement[1])
        return [opcode]
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gD", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def NOT(statement):
    arg_number_check(statement, 1)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("KB", statement[1])
        return [opcode]
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("kB", statement[1])
        return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

# 2 arguments
def SETINT(statement):
    arg_number_check(statement, 2)
    if arg_is_addr(statement[1]):
        addr = statement[1][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(1, statement[0]))
    
    if arg_is_short(statement[2]):
        opcode = short_to_opcode("0i", statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= n < 27.".format(2, statement[0]))
    
    return [opcode, addr]

def PRINT(statement):
    arg_number_check(statement, 2)
    if arg_is_addr(statement[1]):
        addr = statement[1][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(1, statement[0]))
    
    if arg_is_unsigned_tryte_value(statement[2]):
        val = unsigned_value_to_tryte(statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= x < 19683.".format(2, statement[0]))
    
    return ["c00", addr, val]

def SHL(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("KM", statement[1])
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("kM", statement[1])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))
    
    if arg_is_unsigned_tryte_value(statement[2]):
        arg = unsigned_value_to_tryte(statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= x < 19683.".format(2, statement[0]))
    
    return [opcode, arg]

def SHR(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("Km", statement[1])
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("km", statement[1])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))
    
    if arg_is_unsigned_tryte_value(statement[2]):
        arg = unsigned_value_to_tryte(statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= x < 19683.".format(2, statement[0]))
    
    return [opcode, arg]

def READ(statement):
    arg_number_check(statement, 2)
    if arg_is_addr(statement[1]):
        addr = statement[1][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(1, statement[0]))
    if arg_is_tryte_reg(statement[2]):
        opcode = tryte_reg_to_opcode("aA", statement[2])
    elif arg_is_trint_reg(statement[2]):
        opcode = trint_reg_to_opcode("aa", statement[2])
    elif arg_is_float_reg(statement[2]):
        opcode = float_reg_to_opcode("gm", statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} must be a valid register.".format(2, statement[0]))
    
    return [opcode, addr]

def WRITE(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        opcode = tryte_reg_to_opcode("aB", statement[1])
    elif arg_is_trint_reg(statement[1]):
        opcode = trint_reg_to_opcode("ab", statement[1])
    elif arg_is_float_reg(statement[1]):
        opcode = float_reg_to_opcode("gM", statement[1])
    else:
        print_error(statement[-1], "Argument {} in {} must be a valid register.".format(1, statement[0]))
    if arg_is_addr(statement[2]):
        addr = statement[2][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(2, statement[0]))
    
    return [opcode, addr]

def SET(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # SET X, Y
            opcode = "B" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # SET X, n
            opcode = tryte_reg_to_opcode("Kb", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        elif arg_is_addr(statement[2]):
            # SET X, $Y
            opcode = tryte_reg_to_opcode("aC", statement[1])
            addr = statement[2][1:]
            return [opcode, addr]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # SET X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 4*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # SET X, n
            opcode = trint_reg_to_opcode("kb", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        elif arg_is_addr(statement[2]):
            # SET X, $Y
            opcode = trint_reg_to_opcode("ac", statement[1])
            addr = statement[2][1:]
            return [opcode, addr]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_float_reg(statement[1]):
        if arg_is_float_reg(statement[2]):
            # SET Fx, Fy
            reg1_pos = float_register_names.index(statement[1])
            reg2_pos = float_register_names.index(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 4*81 - 40)
            opcode = "f" + opcode[1:]
            return [opcode]
        elif arg_is_float(statement[2]):
            # SET Fx, n
            opcode = float_reg_to_opcode("gC", statement[1])
            tfloat_list = float_to_tfloat(statement[2])
            return [opcode] + tfloat_list
        elif arg_is_addr(statement[2]):
            # SET Fx, $Y
            opcode = float_reg_to_opcode("gc", statement[1])
            addr = statement[2][1:]
            return [opcode, addr]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def CMP(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # CMP X, Y
            opcode = "C" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # CMP X, n
            opcode = tryte_reg_to_opcode("Kc", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # CMP X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 3*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # CMP X, N
            opcode = trint_reg_to_opcode("kc", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_float_reg(statement[1]):
        if arg_is_float_reg(statement[2]):
            # CMP Fx, Fy
            reg1_pos = float_register_names.index(statement[1])
            reg2_pos = float_register_names.index(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 3*81 - 40)
            opcode = "f" + opcode[1:]
            return [opcode]
        elif arg_is_float(statement[2]):
            # CMP Fx, n
            opcode = float_reg_to_opcode("g0", statement[1])
            tfloat_list = float_to_tfloat(statement[2])
            return [opcode] + tfloat_list
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def ADD(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # ADD X, Y
            opcode = "A" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # ADD X, n
            opcode = tryte_reg_to_opcode("Ka", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # ADD X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 2*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # ADD X, N
            opcode = trint_reg_to_opcode("ka", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_float_reg(statement[1]):
        if arg_is_float_reg(statement[2]):
            # ADD Fx, Fy
            reg1_pos = float_register_names.index(statement[1])
            reg2_pos = float_register_names.index(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 2*81 - 40)
            opcode = "f" + opcode[1:]
            return [opcode]
        elif arg_is_float(statement[2]):
            # ADD Fx, n
            opcode = float_reg_to_opcode("ge", statement[1])
            tfloat_list = float_to_tfloat(statement[2])
            return [opcode] + tfloat_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def DIV(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # DIV X, Y
            opcode = "D" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # DIV X, n
            opcode = tryte_reg_to_opcode("Kd", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # DIV X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 0*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # DIV X, N
            opcode = trint_reg_to_opcode("kd", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_float_reg(statement[1]):
        if arg_is_float_reg(statement[2]):
            # DIV Fx, Fy
            reg1_pos = float_register_names.index(statement[1])
            reg2_pos = float_register_names.index(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 0*81 - 40)
            opcode = "f" + opcode[1:]
            return [opcode]
        elif arg_is_float(statement[2]):
            # DIV Fx, n
            opcode = float_reg_to_opcode("gg", statement[1])
            tfloat_list = float_to_tfloat(statement[2])
            return [opcode] + tfloat_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def MUL(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # MUL X, Y
            opcode = "E" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # MUL X, n
            opcode = tryte_reg_to_opcode("Ke", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # MUL X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 1*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # MUL X, N
            opcode = trint_reg_to_opcode("ke", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_float_reg(statement[1]):
        if arg_is_float_reg(statement[2]):
            # MUL Fx, Fy
            reg1_pos = float_register_names.index(statement[1])
            reg2_pos = float_register_names.index(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num - 1*81 - 40)
            opcode = "f" + opcode[1:]
            return [opcode]
        elif arg_is_float(statement[2]):
            # MUL Fx, n
            opcode = float_reg_to_opcode("gf", statement[1])
            tfloat_list = float_to_tfloat(statement[2])
            return [opcode] + tfloat_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def AND(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # AND X, Y
            opcode = "F" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # AND X, n
            opcode = tryte_reg_to_opcode("Kf", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # AND X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num + 1*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # AND X, N
            opcode = trint_reg_to_opcode("kf", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def OR(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # OR X, Y
            opcode = "G" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # OR X, n
            opcode = tryte_reg_to_opcode("Kg", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # OR X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num + 2*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # OR X, N
            opcode = trint_reg_to_opcode("kg", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def XOR(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            # XOR X, Y
            opcode = "H" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        elif arg_is_signed_tryte_value(statement[2]):
            # XOR X, n
            opcode = tryte_reg_to_opcode("Kh", statement[1])
            val = signed_value_to_tryte(statement[2])
            return [opcode, val]
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            # XOR X, Y
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num + 3*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        elif arg_is_signed_trint_value(statement[2]):
            # XOR X, N
            opcode = trint_reg_to_opcode("kh", statement[1])
            trint_list = signed_trint_value_to_trint(statement[2])
            return [opcode] + trint_list
        else:
            print_error(statement[-1], "Argument {} in {} statement must be a register, integer or address.".format(2, statement[0]))
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

def SWAP(statement):
    arg_number_check(statement, 2)
    if arg_is_tryte_reg(statement[1]):
        if arg_is_tryte_reg(statement[2]):
            opcode = "I" + tryte_registers[statement[1]] + tryte_registers[statement[2]]
            return [opcode]
        else:
            print_error(statement[-1], "Sizes of registers in {} statement must match.".format(statement[0]))
    elif arg_is_trint_reg(statement[1]):
        if arg_is_trint_reg(statement[2]):
            reg1_pos = trint_register_names.find(statement[1])
            reg2_pos = trint_register_names.find(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num + 4*81 - 40)
            opcode = "j" + opcode[1:]
            return [opcode]
        else:
            print_error(statement[-1], "Sizes of registers in {} statement must match.".format(statement[0]))
    elif arg_is_float_reg(statement[1]):
        if arg_is_float_reg(statement[2]):
            reg1_pos = float_register_names.index(statement[1])
            reg2_pos = float_register_names.index(statement[2])
            num = 9 * reg1_pos + reg2_pos
            opcode = signed_value_to_tryte(num + 4*81 - 40)
            opcode = "f" + opcode[1:]
            return [opcode]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid register.".format(1, statement[0]))

# 3 arguments
def LOAD(statement):
    arg_number_check(statement, 3)
    if arg_is_addr(statement[1]):
        addr1 = statement[1][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(1, statement[0]))
    if arg_is_signed_tryte_value(statement[2]):
        val = signed_value_to_tryte(statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= x < 19683.".format(2, statement[0]))
    if arg_is_addr(statement[3]):
        addr2 = statement[3][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(3, statement[0]))
    return ["aM0", addr1, val, addr2]

def SAVE(statement):
    arg_number_check(statement, 3)
    if arg_is_addr(statement[1]):
        addr1 = statement[1][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(1, statement[0]))
    if arg_is_signed_tryte_value(statement[2]):
        val = signed_value_to_tryte(statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= x < 19683.".format(2, statement[0]))
    if arg_is_addr(statement[3]):
        addr2 = statement[3][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(3, statement[0]))
    return ["am0", addr1, val, addr2]

def FILL(statement):
    arg_number_check(statement, 3)
    if arg_is_addr(statement[1]):
        addr1 = statement[1][1:]
    else:
        print_error(statement[-1], "Argument {} in {} statement must be a valid address.".format(1, statement[0]))
    if arg_is_unsigned_tryte_value(statement[2]):
        val1 = signed_value_to_tryte(statement[2])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying 0 <= x < 19683.".format(2, statement[0]))
    if arg_is_signed_tryte_value(statement[3]):
        val2 = signed_value_to_tryte(statement[3])
    else:
        print_error(statement[-1], "Argument {} in {} statement must be an integer satisfying -9841 <= x <= 9841.".format(3, statement[0]))
    return ["af0", addr1, val1, val2]

# Assembler macros - these don't correspond to machine instructions

def CALL(statement):
    arg_number_check(statement, 1)
    fn_name = statement[1]
    # convert to actual instructions at linking stage
    # this jumps to $func1, and pushes the instruction pointer onto the stack
    return ["CALL", "$" + fn_name]

def STRWRT(statement):
    arg_number_check(statement, 2)
    input_str = statement[1]
    if arg_is_addr(statement[2]):
        add1 = statement[1][1:]
    add1_value = (729 * (septavingt_chars.find(add1[0]) - 13) 
    + 27 * (septavingt_chars.find(add1[1]) - 13) 
    + (septavingt_chars.find(add1[2]) - 13))
    output_trytes = []
    output_instr_list = []
    if (len(input_str) % 2 == 1):
        # if input_str has odd length, pad it with zero
        new_input_str = input_str + '\0'
        input_str = new_input_str
    else:
        # if input_str has even length, pad it with two zeroes
        new_input_str = input_str + '\0\0'
        input_str = new_input_str
    
    # convert string into list of tryte values
    for i in range(len(input_str) // 2):
        char1 = input_str[2*i]
        char2 = input_str[2*i + 1]
        output_trytes.append(128*ord(char1) + ord(char2) - 9841)
    
    print("Output trytes!")
    print(output_trytes)
    
    # generate list of instructions for printing this string
    offset = 0
    # PUSH D - store whatever is in D before this macro was called
    output_instr_list += PUSH(["PUSH", "D", 8])
    for tryte in output_trytes:
        # SET D2, n
        output_instr_list += SET(["SET", "D2", tryte, 8])
        # WRITE D2, $add1+offset
        output_instr_list += WRITE(["WRITE", "D2", "$" + str(add1_value + offset), 8])
        offset += 1
    # POP D - recover original state of D
    output_instr_list += POP(["POP", "D", 8])

    return output_instr_list


def STRPNT(statement):
    arg_number_check(statement, 1)
    input_str = statement[1]

    # pad input_str with zeroes
    output_trytes = []
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
        output_trytes.append(128*ord(char1) + ord(char2) - 9841)
    
    # now one by one send to register D2 and SHOW them to the console
    output_instr_list = []
    # PUSH D - store whatever is in D before this macro was called
    output_instr_list += PUSH(["PUSH", "D", 8])
    # DGET D1 - store the current display mode in D1
    output_instr_list += DGET(["DGET", "D1", 27])
    # DSET 3 - set output mode to wide_text
    output_instr_list += DSET(["DSET", 3, 0])
    for tryte in output_trytes:
        # SET D2, n
        output_instr_list += SET(["SET", "D2", tryte, 8])
        # SHOW D2
        output_instr_list += SHOW(["SHOW", "D2", 9])
    # DSET D1 - restore original output mode
    output_instr_list += DSET(["DSET", "D1", 29])
    # POP D - recover original state of D
    output_instr_list += POP(["POP", "D", 37])
    
    return output_instr_list