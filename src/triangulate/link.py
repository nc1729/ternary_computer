import handle_instr
import sys

def function_length(function_body):
    """
    Get the length of an assembled function.
    """
    output = 0
    for assembled_statement in function_body:
        output += assembled_statement[-1]
    return output

def build_fn_start_dict(assembled_code_dict):
    """
    Generate a dictionary of function names and the places in the assembly where they start.
    """
    fn_start_dict = {"main": 0} # start main function at beginning of output
    pointer = function_length(assembled_code_dict["main"])
    for fn_name, fn_body in assembled_code_dict.items():
        if fn_name != "main":
            fn_start_dict[fn_name] = pointer
            pointer += function_length(fn_body)
    return fn_start_dict

def handle_function_calls(assembled_code_dict, fn_start_dict):
    """
    Loop through function bodies, find all CALL $func placeholders and replace them with
    actual addresses.
    """
    new_assembled_code = dict()
    for fn_name in assembled_code_dict:
        new_assembled_fn = []
        for assembled_statement in assembled_code_dict[fn_name]:
            if assembled_statement[0][0] == "CALL":
                referenced_fn_name = assembled_statement[0][1][1:]
                referenced_fn_addr = fn_start_dict[referenced_fn_name]
                referenced_fn_addr = handle_instr.signed_value_to_tryte(referenced_fn_addr)
                new_assembled_statement = [[]]
                new_assembled_statement[0].append("0jm") # replace call with jump and store
                new_assembled_statement[0].append(referenced_fn_addr)
                new_assembled_statement.append(2)
                new_assembled_fn.append(new_assembled_statement)
            else:
                new_assembled_fn.append(assembled_statement)
        new_assembled_code[fn_name] = new_assembled_fn
    
    return new_assembled_code

def join_functions_into_list(assembled_code_dict, fn_start_dict):
    """
    The dictionary of assembled functions is converted to a list of assembled statements.
    """
    assembled_code_list = []
    for fn_name in fn_start_dict:
        assembled_code_list += assembled_code_dict[fn_name]
    
    return assembled_code_list

def handle_jump_instructions(assembled_code_list):
    """
    Handle labels left by jump statements.
    """
    # first traverse lists looking for !jump destination labels
    # and store their locations
    new_assembled_code_list = []
    assembled_code_length = 0
    jump_label_dict = dict()
    for statement in assembled_code_list:
        if statement[-1] == 0:
            # statement has no assembled length, must be a jump label
            jump_label_dict[statement[0][0][1:]] = assembled_code_length
        else:
            # a normal instruction, add it to our new list and update length
            new_assembled_code_list.append(statement)
            assembled_code_length += statement[-1]
    
    # now search for jump instructions and assemble them, attaching their
    # destinations in jump_label_dict
    assembled_code_list = [statement for statement in new_assembled_code_list]
    new_assembled_code_list = []
    for statement in assembled_code_list:
        instr = statement[0][0]
        if instr[:2] == "JP":
            new_statement = []
            jump_label = statement[0][1]
            dest = jump_label_dict.get(jump_label)
            if dest is None:
                print("No matching jump destination label for {}.".format(jump_label))
                sys.exit(1)
            new_statement.append(statement[0])
            if len(instr) == 2:
                # found a JP instruction
                new_statement[0][0] = "0jj"
            elif instr[2] == "Z":
                # found a JPZ instruction
                new_statement[0][0] = "0j0"
            elif instr[2] == "P":
                # found a JPP instruction
                new_statement[0][0] = "0ja"
            elif instr[2] == "N":
                # found a JPN instruction
                new_statement[0][0] = "0jA"
            elif instr[2] == "S":
                # found a JPS instruction
                new_statement[0][0] = "0jm"
            else:
                print("Unrecognised jump opcode - aborting...")
                sys.exit(1)
            new_statement[0][1] = handle_instr.signed_value_to_tryte(dest)
            new_statement.append(statement[-1])
            new_assembled_code_list.append(new_statement)
        else:
            new_assembled_code_list.append(statement)
    
    return new_assembled_code_list

def make_output_list(assembled_code_list):
    """
    Collect bodies of assembled statements into a single list.
    """
    output_list = []
    for statement in assembled_code_list:
        output_list += statement[0]
    
    return output_list

def link_code(assembled_code_dict, debug_mode):
    """
    Link assembled code dictionary - call from triangulate.py
    """
    fn_start_dict = build_fn_start_dict(assembled_code_dict)
    if debug_mode:
        print("fn_start_dict:")
        print(fn_start_dict)
    assembled_code_dict = handle_function_calls(assembled_code_dict, fn_start_dict)
    if debug_mode:
        print("After handling function calls:")
        print(assembled_code_dict)
    assembled_statements_list = join_functions_into_list(assembled_code_dict, fn_start_dict)
    if debug_mode:
        print("After joining functions:")
        print(assembled_statements_list)
    assembled_statements_list = handle_jump_instructions(assembled_statements_list)
    if debug_mode:
        print("After handling jump instructions:")
        print(assembled_statements_list)
    output_assembly_list = make_output_list(assembled_statements_list)
    if debug_mode:
        print("Linked code:")
        print(output_assembly_list)
    return output_assembly_list
    