import triangulate
import assemble

def test_PRI():
    possible_inputs = [str(i) for i in range(27)]
    test_septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    possible_outputs = ["0I" + test_septavingt_chars[i] for i in range(27)]
    for j in range(27):
        test_output = assemble.assemble_instr(['PRI', possible_inputs[j], 3])
        assert(test_output == [[possible_outputs[j]], 1])

def test_THD():
    possible_inputs = [str(i) for i in range(27)]
    test_septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    possible_outputs = ["0h" + test_septavingt_chars[i] for i in range(27)]
    for j in range(27):
        test_output = assemble.assemble_instr(['THD', possible_inputs[j], 3])
        assert(test_output == [[possible_outputs[j]], 1])

def test_MOUNT():
    possible_inputs = [str(i) for i in range(27)]
    test_septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm"
    possible_outputs = ["0m" + test_septavingt_chars[i] for i in range(27)]
    for j in range(27):
        test_output = assemble.assemble_instr(['MOUNT', possible_inputs[j], 3])
        assert(test_output == [[possible_outputs[j]], 1])