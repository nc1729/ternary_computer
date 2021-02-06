import handle_instr
import sys

def function_length(function_body):
    """
    Get the length of an assembled function, disregarding jump destination tokens.
    """
    output = 0
    for token in function_body:
        if token[0] == "!":
            pass
        else:
            output += 1
    return output

def build_fn_start_dict(assembled_code):
    """
    Generate a dictionary of function names and the places in the assembly where they start.
    """
    fn_start_dict = {"main": 0} # start main function at beginning of output
    pointer = function_length(assembled_code["main"])
    for fn_name, fn_body in assembled_code.items():
        if fn_name != "main":
            fn_start_dict[fn_name] = pointer
            pointer += function_length(fn_body)
    return fn_start_dict

def handle_function_calls(assembled_code, fn_start_dict):
    """
    Loop through function bodies, find all CALL $func placeholders and replace them with
    actual addresses.
    """
    new_assembled_code = dict()
    for fn_name in assembled_code:
        new_assembled_fn = []
        for token in assembled_code[fn_name]:
            if token[0] == "$":
                referenced_fn_name = token[1:]
                referenced_fn_addr = fn_start_dict[referenced_fn_name]
                referenced_fn_addr = handle_instr.signed_value_to_tryte(referenced_fn_addr)
                token = referenced_fn_addr
                new_assembled_fn.append(token)
            else:
                new_assembled_fn.append(token)
        new_assembled_code[fn_name] = new_assembled_fn
    
    return new_assembled_code

def join_functions_into_list(assembled_code, fn_start_dict):
    """
    The assembled code dictionary is converted to a list of Trytes.
    """
    assm_list = []
    for fn_name in fn_start_dict:
        assm_list += assembled_code[fn_name]
    
    return assm_list

def handle_jump_instructions(assembled_code_list):
    """
    Handle labels left by jump statements.
    """

    # first loop through list and remove !jump destination labels
    # and store their positions (using length of new list)
    new_assm_list = []
    jump_label_dict = dict()
    for symbol in assembled_code_list:
        if symbol[0] == "!":
            # found a jump destination label, add it to dictionary
            # ignore it in new list
            jump_label_dict[symbol[1:]] = len(new_assm_list)
        else:
            new_assm_list.append(symbol)
    
    # store this list
    assm_list = [symbol for symbol in new_assm_list]
    new_assm_list = []
    ignore_next = False

    # now loop through new list and use the jump label dictionary
    # to point jump statements to where they need to go
    for index, symbol in enumerate(assm_list):
        if symbol[:2] == "JP":
            # found a jump instruction
            jump_label = assm_list[index + 1]
            dest = jump_label_dict.get(jump_label)
            if dest is None:
                print("No matching jump destination label for {}.".format(jump_label))
                sys.exit(1)
            else:
                if len(symbol) == 2:
                    # JP instruction
                    new_assm_list.append("0jj")
                elif symbol[-1] == "Z":
                    # JPZ instruction
                    new_assm_list.append("0j0")
                elif symbol[-1] == "N":
                    # JPN instruction
                    new_assm_list.append("0jA")
                elif symbol[-1] == "P":
                    # JPP instruction
                    new_assm_list.append("0ja")
                elif symbol[-1] == "S":
                    # JPS instruction
                    new_assm_list.append("0jm")
                else:
                    print("Unrecognised jump opcode - aborting...")
                    sys.exit(1)
                new_assm_list.append(handle_instr.signed_value_to_tryte(dest))
            ignore_next = True
        else:
            if ignore_next:
                ignore_next = False
            else:
                new_assm_list.append(symbol)
    
    return new_assm_list

def replace_calls_with_jump_and_stores(assembled_code_list):
    new_assembled_code_list = []
    for symbol in assembled_code_list:
        if symbol == "CALL":
            new_assembled_code_list.append("0jm")
        else:
            new_assembled_code_list.append(symbol)
    
    return new_assembled_code_list

def link_code(assembled_code, debug_mode):
    """
    Link assembled code dictionary - call from triangulate.py
    """
    fn_start_dict = build_fn_start_dict(assembled_code)
    if debug_mode:
        print("fn_start_dict:")
        print(fn_start_dict)
    assembled_code = handle_function_calls(assembled_code, fn_start_dict)
    if debug_mode:
        print("After handling function calls:")
        print(assembled_code)
    output_assembly = join_functions_into_list(assembled_code, fn_start_dict)
    if debug_mode:
        print("After joining functions:")
        print(output_assembly)
    output_assembly = handle_jump_instructions(output_assembly)
    if debug_mode:
        print("After handling jump instructions:")
        print(output_assembly)
    output_assembly = replace_calls_with_jump_and_stores(output_assembly)
    if debug_mode:
        print("Linked code:")
        print(output_assembly)
    return output_assembly
    