import triangulate
import assemble

test_tryte_registers = {"A0": "M", "A1": "L", "A2": "K", "B0": "J", 
    "B1": "I", "B2": "H", "C0": "G", "C1": "F", "C2": "E", "D0": "D", 
    "D1": "C", "D2": "B", "E0": "A", "E1": "0", "E2": "a", "F0": "b",
    "F1": "c", "F2": "d", "G0": "e", "G1": "f", "G2": "g", "H0": "h",
    "H1": "i", "H2": "j", "I0": "k", "I1": "l", "I2": "m"}
test_trint_registers = {"A": "D", "B": "C", "C": "B", "D": "A", 
    "E": "0", "F": "a", "G": "b", "H": "c", "I": "d"}
test_trint_register_names = "ABCDEFGHI"
test_septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"

def test_HALT():
    expected_output = [["000"], 1]
    test_output = assemble.assemble_instr(['HALT', 11])
    assert(expected_output == test_output)

def test_NOOP():
    expected_output = [["00a"], 1]
    test_output = assemble.assemble_instr(['NOOP', 11])
    assert(expected_output == test_output)

def test_WAIT():
    expected_output = [["00A"], 1]
    test_output = assemble.assemble_instr(['WAIT', 11])
    assert(expected_output == test_output)

def test_CCMP():
    expected_output = [["0a0"], 1]
    test_output = assemble.assemble_instr(['CCMP', 11])
    assert(expected_output == test_output)

def test_CCAR():
    expected_output = [["0aA"], 1]
    test_output = assemble.assemble_instr(['CCAR', 11])
    assert(expected_output == test_output)

def test_COVF():
    expected_output = [["0aa"], 1]
    test_output = assemble.assemble_instr(['COVF', 11])
    assert(expected_output == test_output)

def test_PJP():
    expected_output = [["0jM"], 1]
    test_output = assemble.assemble_instr(['PJP', 11])
    assert(expected_output == test_output)

def test_CHK():
    expected_output = [["0c0"], 1]
    test_output = assemble.assemble_instr(['CHK', 11])
    assert(expected_output == test_output)

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
        output = [["bM" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["PEEK", tryte, 7])
        assert(test_output == output)
    for trint in test_trint_registers:
        output = [["bm" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["PEEK", trint, 7])
        assert(test_output == output)

def test_PUSH():
    for tryte in test_tryte_registers:
        output = [["bA" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["PUSH", tryte, 7])
        assert(test_output == output)
    for trint in test_trint_registers:
        output = [["ba" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["PUSH", trint, 7])
        assert(test_output == output)

def test_POP():
    for tryte in test_tryte_registers:
        output = [["bB" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["POP", tryte, 7])
        assert(test_output == output)
    for trint in test_trint_registers:
        output = [["bb" + test_trint_registers[trint]], 1]
        test_output = assemble.assemble_instr(["POP", trint, 7])
        assert(test_output == output)

def test_WHERE():
    for tryte in test_tryte_registers:
        output = [["b0" + test_tryte_registers[tryte]], 1]
        test_output = assemble.assemble_instr(["WHERE", tryte, 7])
        assert(test_output == output)