import triangulate
import handle_instr

def test_add_trytes():
    output = triangulate.triangulate("main:\nADD A0, B0\nend main")
    assert(output == "AMJ 000")
    output = triangulate.triangulate("main:\nADD E2, C1\nend main")
    assert(output == "AaF 000")