from error import print_error
import handle_instr

def function_dict_maker(code):
    # Turn parsed code into a dict of functions (and error if the functions don't make sense)
    function_dict = dict()
    new_function_name = ""
    # find function names
    for index, statement in enumerate(code):
        if statement[0][-1] == ":":
            new_function_name = statement[0].strip(':')
            # check if new function name matches one of the previous ones
            if new_function_name in function_dict:
                print_error(statement[-1], "Function {} already defined.".format(new_function_name))
            function_start_index = index
        if statement[0] == "end":
            if len(statement) == 2:
                print_error(statement[-1], "Unresolved end statement.")
            else:
                if statement[1] == new_function_name:
                    # found a function body, collect it
                    function_end_index = index
                    new_function_body = []
                    for statement_index in range(function_start_index + 1, function_end_index):
                        new_function_body.append(code[statement_index])
                    function_dict[new_function_name] = new_function_body
                        
                else:
                    print_error(index, "Unrecognised function name '{}'.".format(statement[1]))
    
    # if main not in function names, abort
    if "main" not in function_dict:
        print_error(1, "No main function found.")
    
    return function_dict


def assemble_function(function_body, function_name):
    instruction_dict = {
        "READ": handle_instr.READ,
        "WRITE": handle_instr.WRITE,
        "LOAD": handle_instr.LOAD,
        "SAVE": handle_instr.SAVE,
        "PRINT": handle_instr.PRINT,
        "SHOW": handle_instr.SHOW,
        "TELL": handle_instr.TELL,
        "PEEK": handle_instr.PEEK,
        "FILL": handle_instr.FILL,
        "MOUNT": handle_instr.MOUNT,
        "PUSH": handle_instr.PUSH,
        "POP": handle_instr.POP,
        "WHERE": handle_instr.WHERE,
        "SET": handle_instr.SET,
        "SWAP": handle_instr.SWAP,
        "CCMP": handle_instr.CCMP,
        "CCAR": handle_instr.CCAR,
        "COVF": handle_instr.COVF,
        "PRI": handle_instr.PRI,
        "CHK": handle_instr.CHK,
        "FLIP": handle_instr.FLIP,
        "INC": handle_instr.INC,
        "DEC": handle_instr.DEC,
        "ADD": handle_instr.ADD,
        "MUL": handle_instr.MUL,
        "DIV": handle_instr.DIV,
        "SHL": handle_instr.SHL,
        "SHR": handle_instr.SHR,
        "CMP": handle_instr.CMP,
        "AND": handle_instr.AND,
        "OR": handle_instr.OR,
        "XOR": handle_instr.XOR,
        "NOT": handle_instr.NOT,
        "NOOP": handle_instr.NOOP,
        "JPZ": handle_instr.JPZ,
        "JPN": handle_instr.JPN,
        "JPP": handle_instr.JPP,
        "JP": handle_instr.JP,
        "JPS": handle_instr.JPS,
        "PJP": handle_instr.PJP,
        "THD": handle_instr.THD,
        "SETINT": handle_instr.SETINT,
        "HALT": handle_instr.HALT,
        "WAIT": handle_instr.WAIT,
        "CALL": handle_instr.CALL,
        "STRWRT": handle_instr.STRWRT,
        "STRPNT": handle_instr.STRPNT}

    assembled_fn = []
    # assemble function
    for statement in function_body:
        if statement[0][0] == "!":
            output_trytes = [statement[0]]
            assembled_fn += output_trytes
        else:
            instruction = instruction_dict.get(statement[0])
            if not instruction:
                print_error(statement[-1],
                            "Unrecognised instruction '{}'.".format(statement[0]))
            else:
                output_trytes = instruction(statement)
                assembled_fn += output_trytes
        
    # append "0jM" to each internal function - it's a pop and jump
    if function_name != "main":
        assembled_fn += ["0jM"]
    else:
        # if we hit the end of main, we have to halt
        assembled_fn += ["000"]

    return assembled_fn

def assemble_code(code, debug_mode):
    function_dict = function_dict_maker(code)
    if debug_mode:
        print("Function dict:")
        print(function_dict)
    assembled_function_dict = dict()
    for fn in function_dict:
        assembled_function_dict[fn] = assemble_function(function_dict[fn], fn)
    if debug_mode:
        print("Assembled code:")
        print(assembled_function_dict)
    return assembled_function_dict