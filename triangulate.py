#!/usr/bin/env python3
import sys
import handle_instr


def handle_inputs():
    number_of_inputs = len(sys.argv) - 1
    if (number_of_inputs == 0):
        sys.exit("triangulate: fatal error: no input files.")
    elif (number_of_inputs == 1):
        if (sys.argv[1] == "-h"):
            print("Converts a ternary assembly .tas file into a ternary machine code (.tri) program.")
            print("Usage:")
            print("python3 ./triangulate.py [input_file] -o [output_file]")
            sys.exit(1)
        else:
            filenames = [sys.argv[1], "a.tri"]
            return filenames
    elif (number_of_inputs == 3):
        if (sys.argv[2] == "-o"):
            filenames = [sys.argv[1], sys.argv[3]]
            return filenames
        else:
            sys.exit("Invalid input.")
    else:
        sys.exit("Invalid input.")

def print_error(line_number, err_msg):
    print(("{}, line {}: " + err_msg).format(sys.argv[1], line_number))
    print("Compilation terminated.")
    sys.exit(1)

def store_strings(code):
    code_as_string = "".join(code)
    stored_strings = []
    string_starts = []
    string_ends = []
    # loop through code string - when we see a " char, note it
    in_string = False
    line_number = 1
    for pos, char in enumerate(code_as_string):
        if char == "\"":
            if in_string:
                in_string = False
                string_ends.append(pos)
            else:
                in_string = True
                string_starts.append(pos)
        if char == "\n":
            if in_string:
                print_error(line_number, "Unterminated string. Strings cannot be longer than one line.")
            else:
                line_number += 1

    # now we know the locations of the strings, extract them (with their " chars)
    for i in range(len(string_starts)):
        stored_strings.append(code_as_string[(string_starts[i]):(string_ends[i] + 1)])
    
    # and replace them in the code with placeholders
    for i in range(len(string_starts)):
        code_as_string = code_as_string.replace(stored_strings[i], "string{}".format(i))
    
    # lastly, split code into list of strings again
    output_code = code_as_string.split("\n")

    return output_code, stored_strings
        

def handle_special_chars(stored_strings):
    new_stored_strings = []
    for string in stored_strings:
        str_len = len(string)
        i = 0
        new_stored_string = ""
        while i < str_len:
            if string[i] == "\\":
                # check next character to see if it's special
                special_chars = {"a" : "\a", "b": "\b", "f": "\f", "n": "\n", 
                "t": "\t", "v": "\v", "\\": "\\", "\"": "\""}
                if string[i+1] in special_chars:
                    new_stored_string += special_chars[string[i+1]]
                    i += 2                    
                else:
                    new_stored_string += "\\"
                    i += 1
            else:
                new_stored_string += string[i]
                i += 1
        new_stored_strings.append(new_stored_string)
    return new_stored_strings


def restore_strings(stored_strings, code):
    restored_code = []
    index = 0
    for line in code:
        restored_line = []
        for token in line:
            if token == "string" + str(index):
                restored_line.append(stored_strings[index].strip("\""))
                index += 1
            else:
                restored_line.append(token)
        restored_code.append(restored_line)
    
    return restored_code

    

def remove_comments(code):
    # anything in lines contained in '#' chars will be ignored by assembler
    no_comment_code = []
    for line in code:
        comment = False
        char_list = []
        for char in line:
            if char == '#':
                if comment:
                    comment = False
                else:
                    comment = True
            elif char == '\n':
                # remove new line chars
                pass
            elif not comment:
                char_list.append(char)
        new_line = "".join(char_list)
        no_comment_code.append(new_line)
    return no_comment_code



            

def remove_empty_lines(code):
    output_code = []
    for line in code:
        if len(line) != 0:
            output_code.append(line)
    return output_code


def split_lines_into_tokens(code):
    # each line is a set of whitespace separated tokens, this function
    # returns a list of lists of these tokens
    output_code = []
    for line in code:
        output_code.append(line.split())
    
    return output_code

def remove_commas(code):
    # remove trailing commas from tokens
    output_code = []
    for line in code:
        new_line = []
        for token in line:
            new_token = token.strip(',')
            new_line.append(new_token)
        output_code.append(new_line)
    return output_code

def add_line_numbers(code):
    # Append line numbers to parsed statements (for error location)
    index = 1
    for line in code:
        if len(line) > 0:
            line.append(index)
        index += 1
    return code


def parse(code):
    # Turn source into list of lists of type [token, token, ..., token, line_number]
    code, stored_strings = store_strings(code)
    code = remove_comments(code)
    code = split_lines_into_tokens(code)
    code = remove_commas(code)
    code = add_line_numbers(code)
    code = remove_empty_lines(code)
    stored_strings = handle_special_chars(stored_strings)
    code = restore_strings(stored_strings, code)
    return code

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
        
    # append "0E0" to each internal function - it's a pop and jump
    if function_name != "main":
        assembled_fn += ["0E0"]
    else:
        # if we hit the end of main, we have to halt
        assembled_fn += ["000"]

    return assembled_fn

def link(assembled_fn_dict):
    fn_names = assembled_fn_dict.keys()
    fn_start_dict = {"main": 0} # start main function at beginning of output
    pointer = len(assembled_fn_dict["main"])
    for fn_name, fn_body in assembled_fn_dict.items():
        if fn_name != "main":
            fn_start_dict[fn_name] = pointer
            pointer += len(fn_body)
    
    # loop through function bodies, find all $func placeholders and replace them 
    # with actual addresses
    new_assembled_fn_dict = dict()
    for fn_name in assembled_fn_dict:
        new_assembled_fn = []
        for token in assembled_fn_dict[fn_name]:
            if token[0] == "$":
                referenced_fn_name = token[1:]
                referenced_fn_addr = fn_start_dict[referenced_fn_name]
                referenced_fn_addr = handle_instr.unsigned_value_to_tryte(referenced_fn_addr)
                token = referenced_fn_addr
                new_assembled_fn.append(token)
            else:
                new_assembled_fn.append(token)
        new_assembled_fn_dict[fn_name] = new_assembled_fn
    
    # now join functions into one big list
    assm_list = []
    for fn_name in fn_start_dict:
        assm_list += new_assembled_fn_dict[fn_name]
    
    # finally, handle labels left by jump statements
    # first loop through list and remove !jump destination labels
    # and store their positions (using length of new list)
    new_assm_list = []
    jump_label_dict = dict()
    for symbol in assm_list:
        if symbol[0] == "!":
            # found a jump destination label, add it to dictionary
            # ignore it in new list
            jump_label_dict[symbol[1:]] = len(new_assm_list)
        else:
            new_assm_list.append(symbol)
    
    # overwrite assm_list
    assm_list = [symbol for symbol in new_assm_list]
    new_assm_list = []
    ignore_next = False

    # now loop through new list and use the jump label dictionary
    # to point jump statements to where they need to go
    for index, symbol in enumerate(assm_list):
        if symbol[:2] == "0j":
            # found a jump instruction
            if symbol[2] == "M":
                # pop and jump doesn't have a label
                new_assm_list.append(symbol)
            else:
                jump_label = assm_list[index + 1]
                dest = jump_label_dict.get(jump_label)
                if dest is None:
                    print("No matching jump destination label for {}.".format(jump_label))
                    sys.exit(1)
                else:
                    new_assm_list.append(symbol)
                    new_assm_list.append(handle_instr.signed_value_to_tryte(dest))
            ignore_next = True
        else:
            if ignore_next:
                ignore_next = False
            else:
                new_assm_list.append(symbol)
    
    return new_assm_list


def string_maker(linked_code):
    return ' '.join(linked_code)


if __name__ == "__main__":
    filenames = handle_inputs()
    input_filename = filenames[0]
    output_filename = filenames[1]
    with open(input_filename) as f:
        input_code = f.readlines()

    # remove comments and empty lines
    parsed_code = parse(input_code)
    # break parsed code up into functions
    function_dict = function_dict_maker(parsed_code)
    # assemble these functions separately
    assembled_function_dict = dict()
    for fn in function_dict:
        assembled_function_dict[fn] = assemble_function(function_dict[fn], fn)

    # now link
    linked_code = link(assembled_function_dict)
    # make it a string
    assembly_string = string_maker(linked_code)

    # then write assembly_string to output file
    with open(output_filename, 'w') as writer:
        writer.write(assembly_string)