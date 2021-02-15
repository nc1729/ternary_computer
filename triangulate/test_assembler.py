import triangulate
import handle_instr
import assemble

test_tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", 
    "B1": "I", "B2": "H", "C0": "G", "C1": "F", "C2": "E", "D0": "D", 
    "D1": "C", "D2": "B", "E0": "A", "E1": "0", "E2": "a", "G0": "b",
    "G1": "c", "G2": "d", "H0": "e", "H1": "f", "H2": "g", "I0": "h",
    "I1": "i", "I2": "j", "J0": "k", "J1": "l", "J2": "m"}
test_trint_registers = {"A": "D", "B": "C", "C": "B", "D": "A", 
    "E": "0", "G": "a", "H": "b", "I": "c", "J": "d"}
test_trint_register_names = "ABCDEGHIJ"
test_septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"

def test_signed_value_to_tryte():
    expected_output = "MMM"
    test_output = handle_instr.signed_value_to_tryte(-9841)
    assert(test_output == expected_output)
    expected_output = "d0A"
    test_output = handle_instr.signed_value_to_tryte(2915)
    assert(test_output == expected_output)

def test_HALT():
    expected_output = [["000"], 1]
    test_output = assemble.assemble_instr(['HALT', 11])
    assert(test_output == expected_output)

def test_NOOP():
    expected_output = [["00a"], 1]
    test_output = assemble.assemble_instr(['NOOP', 11])
    assert(test_output == expected_output)

def test_WAIT():
    expected_output = [["00A"], 1]
    test_output = assemble.assemble_instr(['WAIT', 11])
    assert(test_output == expected_output)

def test_CCMP():
    expected_output = [["0a0"], 1]
    test_output = assemble.assemble_instr(['CCMP', 11])
    assert(test_output == expected_output)

def test_CCAR():
    expected_output = [["0aA"], 1]
    test_output = assemble.assemble_instr(['CCAR', 11])
    assert(test_output == expected_output)

def test_COVF():
    expected_output = [["0aa"], 1]
    test_output = assemble.assemble_instr(['COVF', 11])
    assert(test_output == expected_output)

def test_PJP():
    expected_output = [["0jM"], 1]
    test_output = assemble.assemble_instr(['PJP', 11])
    assert(test_output == expected_output)

def test_CHK():
    expected_output = [["0c0"], 1]
    test_output = assemble.assemble_instr(['CHK', 11])
    assert(test_output == expected_output)

# 1 argument

def test_PRI():
    possible_inputs = [str(i) for i in range(27)]
    possible_outputs = ["0I" + test_septavingt_chars[i] for i in range(27)]
    for j in range(27):
        test_output = assemble.assemble_instr(['PRI', possible_inputs[j], 3])
        assert(test_output == [[possible_outputs[j]], 1])

def test_THD():
    possible_inputs = [str(i) for i in range(27)]
    possible_outputs = ["0h" + test_septavingt_chars[i] for i in range(27)]
    for j in range(27):
        test_output = assemble.assemble_instr(['THD', possible_inputs[j], 3])
        assert(test_output == [[possible_outputs[j]], 1])

def test_MOUNT():
    possible_inputs = [str(i) for i in range(27)]
    possible_outputs = ["0m" + test_septavingt_chars[i] for i in range(27)]
    for j in range(27):
        test_output = assemble.assemble_instr(['MOUNT', possible_inputs[j], 3])
        assert(test_output == [[possible_outputs[j]], 1])

def test_PEEK():
    for tryte in test_tryte_registers:
        expected_output = [["bM" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["PEEK", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["bm" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["PEEK", trint, 7])
        assert(test_output == expected_output)

def test_PUSH():
    for tryte in test_tryte_registers:
        expected_output = [["bA" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["PUSH", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["ba" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["PUSH", trint, 7])
        assert(test_output == expected_output)

def test_POP():
    for tryte in test_tryte_registers:
        expected_output = [["bB" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["POP", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["bb" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["POP", trint, 7])
        assert(test_output == expected_output)

def test_WHERE():
    for tryte in test_tryte_registers:
        expected_output = [["b0" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["WHERE", tryte, 7])
        assert(test_output == expected_output)

def test_SHOW():
    for tryte in test_tryte_registers:
        expected_output = [["aA" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["SHOW", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["aa" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["SHOW", trint, 7])
        assert(test_output == expected_output)

def test_TELL():
    for tryte in test_tryte_registers:
        expected_output = [["aB" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["TELL", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["ab" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["TELL", trint, 7])
        assert(test_output == expected_output)
    
def test_INC():
    for tryte in test_tryte_registers:
        expected_output = [["Ki" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["INC", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["ki" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["INC", trint, 7])
        assert(test_output == expected_output)

def test_DEC():
    for tryte in test_tryte_registers:
        expected_output = [["KI" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["DEC", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["kI" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["DEC", trint, 7])
        assert(test_output == expected_output)

def test_FLIP():
    for tryte in test_tryte_registers:
        expected_output = [["Kf" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["FLIP", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["kf" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["FLIP", trint, 7])
        assert(test_output == expected_output)

def test_ABS():
    for tryte in test_tryte_registers:
        expected_output = [["KA" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["ABS", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["kA" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["ABS", trint, 7])
        assert(test_output == expected_output)

def test_NOT():
    for tryte in test_tryte_registers:
        expected_output = [["KB" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["NOT", tryte, 7])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["kB" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["NOT", trint, 7])
        assert(test_output == expected_output)

# 2 arguments

def test_SETINT():
    possible_inputs = [str(i) for i in range(27)]
    for i in range(27):
        expected_output = [["0i" + test_septavingt_chars[i], "DDD"], 2]
        test_output = assemble.assemble_instr(["SETINT", "$DDD", possible_inputs[i], 5])
        assert(test_output == expected_output)

def test_PRINT():
    expected_output = [["a00", "DDD", "MME"], 3]
    test_output = assemble.assemble_instr(["PRINT", "$DDD", "8", 9])
    assert(test_output == expected_output)

def test_SHL():
    for tryte in test_tryte_registers:
        expected_output = [["KM" + test_tryte_registers[tryte], "MMC"], 2]
        test_output = assemble.assemble_instr(["SHL", tryte, "10", 5])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["kM" + test_trint_registers[trint], "MMG"], 2]
        test_output = assemble.assemble_instr(["SHL", trint, "6", 5])
        assert(test_output == expected_output)

def test_SHR():
    for tryte in test_tryte_registers:
        expected_output = [["Km" + test_tryte_registers[tryte], "MMC"], 2]
        test_output = assemble.assemble_instr(["SHR", tryte, "10", 5])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["km" + test_trint_registers[trint], "MMG"], 2]
        test_output = assemble.assemble_instr(["SHR", trint, "6", 5])
        assert(test_output == expected_output)

def test_READ():
    for tryte in test_tryte_registers:
        expected_output = [["aC" + test_tryte_registers[tryte], "DDD"], 2]
        test_output = assemble.assemble_instr(["READ", "$DDD", tryte, 5])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["ac" + test_trint_registers[trint], "DDD"], 2]
        test_output = assemble.assemble_instr(["READ", "$DDD", trint, 5])
        assert(test_output == expected_output)

def test_WRITE():
    for tryte in test_tryte_registers:
        expected_output = [["aD" + test_tryte_registers[tryte], "DDD"], 2]
        test_output = assemble.assemble_instr(["WRITE", tryte, "$DDD", 5])
        assert(test_output == expected_output)
    for trint in test_trint_registers:
        expected_output = [["ad" + test_trint_registers[trint], "DDD"], 2]
        test_output = assemble.assemble_instr(["WRITE", trint, "$DDD", 5])
        assert(test_output == expected_output)

def test_SET_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["B" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["SET", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_SET_tryte_to_num():
    for tryte in test_tryte_registers:
        expected_output = [["Kb" + test_tryte_registers[tryte], "00g"], 2]
        test_output = assemble.assemble_instr(["SET", tryte, 7, 213])
        assert(test_output == expected_output)

def test_SET_tryte_to_addr():
    for tryte in test_tryte_registers:
        expected_output = [["aC" + test_tryte_registers[tryte], "DDD"], 2]
        test_output = assemble.assemble_instr(["SET", tryte, "$DDD", 37])
        assert(test_output == expected_output)

def test_SET_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num - 4*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["SET", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_SET_trint_to_num():
    for trint in test_trint_registers:
        expected_output = [["kb" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["SET", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_SET_trint_to_addr():
    for trint in test_trint_registers:
        expected_output = [["ac" + test_trint_registers[trint], "DDD"], 2]
        test_output = assemble.assemble_instr(["SET", trint, "$DDD", 37])
        assert(test_output == expected_output)

def test_CMP_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["C" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["CMP", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_CMP_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Kc" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["CMP", tryte, 9, 29])
        assert(test_output == expected_output)

def test_CMP_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num - 3*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["CMP", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_CMP_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["kc" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["CMP", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_ADD_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["A" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["ADD", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_ADD_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Ka" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["ADD", tryte, 9, 29])
        assert(test_output == expected_output)

def test_ADD_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num - 2*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["ADD", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_ADD_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["ka" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["ADD", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_MUL_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["E" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["MUL", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_MUL_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Ke" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["MUL", tryte, 9, 29])
        assert(test_output == expected_output)

def test_MUL_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num - 1*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["MUL", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_MUL_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["ke" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["MUL", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_DIV_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["D" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["DIV", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_DIV_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Kd" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["DIV", tryte, 9, 29])
        assert(test_output == expected_output)

def test_DIV_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num - 0*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["DIV", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_DIV_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["kd" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["DIV", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_AND_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["F" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["AND", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_AND_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Kf" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["AND", tryte, 9, 29])
        assert(test_output == expected_output)

def test_AND_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num + 1*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["AND", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_AND_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["kf" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["AND", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_OR_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["G" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["OR", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_OR_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Kg" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["OR", tryte, 9, 29])
        assert(test_output == expected_output)

def test_OR_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num + 2*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["OR", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_OR_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["kg" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["OR", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_XOR_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["H" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["XOR", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_XOR_tryte_with_num():
    for tryte in test_tryte_registers:
        expected_output = [["Kh" + test_tryte_registers[tryte], "00i"], 2]
        test_output = assemble.assemble_instr(["XOR", tryte, 9, 29])
        assert(test_output == expected_output)

def test_XOR_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num + 3*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["XOR", trint1, trint2, 98])
            assert(test_output == expected_output)

def test_XOR_trint_with_num():
    for trint in test_trint_registers:
        expected_output = [["kh" + test_trint_registers[trint], "000", "0bC", "EGa"], 4]
        test_output = assemble.assemble_instr(["XOR", trint, 1000000, 23])
        assert(test_output == expected_output)

def test_SWAP_trytes():
    for tryte1 in test_tryte_registers:
        for tryte2 in test_tryte_registers:
            expected_output = [["I" + test_tryte_registers[tryte1] + test_tryte_registers[tryte2]], 1]
            test_output = assemble.assemble_instr(["SWAP", tryte1, tryte2, 11])
            assert(test_output == expected_output)

def test_SWAP_trints():
    for trint1 in test_trint_registers:
        for trint2 in test_trint_registers:
            reg1_pos = test_trint_register_names.find(trint1)
            reg2_pos = test_trint_register_names.find(trint2)
            num = 9 * reg1_pos + reg2_pos
            opcode = handle_instr.signed_value_to_tryte(num + 4*81 - 40)
            opcode = "j" + opcode[1:]
            expected_output = [[opcode], 1]
            test_output = assemble.assemble_instr(["SWAP", trint1, trint2, 98])
            assert(test_output == expected_output)

# 3 arguments
def test_LOAD():
    expected_output = [["aM0", "DDD", "00a", "eee"], 4]
    test_output = assemble.assemble_instr(["LOAD", "$DDD", 1, "$eee", 26])
    assert(test_output == expected_output)
    expected_output = [["aM0", "DDD", "AAA", "eee"], 4]
    test_output = assemble.assemble_instr(["LOAD", "$DDD", -757, "$eee", 26])
    assert(test_output == expected_output)

def test_SAVE():
    expected_output = [["am0", "DDD", "00a", "eee"], 4]
    test_output = assemble.assemble_instr(["SAVE", "$DDD", 1, "$eee", 26])
    assert(test_output == expected_output)
    expected_output = [["am0", "DDD", "AAA", "eee"], 4]
    test_output = assemble.assemble_instr(["SAVE", "$DDD", -757, "$eee", 26])
    assert(test_output == expected_output)

def test_FILL():
    expected_output = [["af0", "DDD", "0b0", "0CC"], 4]
    test_output = assemble.assemble_instr(["FILL", "$DDD", 54, -84, 26])
    assert(test_output == expected_output)