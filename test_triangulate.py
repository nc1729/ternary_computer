import triangulate
import handle_instr

def test_add_trytes():
    output = triangulate.triangulate("main:\nADD A0, B0\nend main")
    assert(output == "AMJ 000")
    output = triangulate.triangulate("main:\nADD E2, C1\nend main")
    assert(output == "AaF 000")

def test_set_trytes():
    output = triangulate.triangulate("main:\nSET A0, 7\nend main")
    assert(output == "K0M 00g 000")