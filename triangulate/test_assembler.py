import triangulate
import assemble

def test_add_trytes():
    output = assemble.assemble_instr(['ADD', 'A', 'B', 14])
    assert(output == [["jGL"], 1])

def test_set_trytes():
    output = assemble.assemble_instr(['SET', 'I', '8', 10])
    assert(output == [["k0d", "000", "000", "00h"], 4])