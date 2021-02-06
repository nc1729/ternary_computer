from error import print_error

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
            if str(token)[0:6] == "string":
                index = int(str(token)[6:])
                restored_line.append(stored_strings[index].strip("\""))
            else:
                restored_line.append(token)
        restored_code.append(restored_line)
    
    return restored_code

    

def remove_comments(code):
    # anything in lines contained in '#' chars will be ignored by assembler
    no_comment_code = []
    block_comment = False
    for line in code:
        comment = False
        if "###" in line:
            if block_comment:
                block_comment = False
            else:
                block_comment = True
        else:
            if not block_comment:
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

def parse_code(code, debug_mode):
    # Turn source into list of lists of type [token, token, ..., token, line_number]
    code, stored_strings = store_strings(code)
    if debug_mode:
        print("After storing strings:")
        print(code)
        print(stored_strings)
    code = remove_comments(code)
    if debug_mode:
        print("After removing comments:")
        print(code)
    code = split_lines_into_tokens(code)
    if debug_mode:
        print("After splitting lines:")
        print(code)
    code = remove_commas(code)
    if debug_mode:
        print("After removing commas:")
        print(code)
    code = add_line_numbers(code)
    if debug_mode:
        print("After adding line numbers:")
        print(code)
    code = remove_empty_lines(code)
    if debug_mode:
        print("After removing empty lines:")
        print(code)
    stored_strings = handle_special_chars(stored_strings)
    code = restore_strings(stored_strings, code)
    if debug_mode:
        print("Parsed code:")
        print(code)
    return code