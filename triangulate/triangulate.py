#!/usr/bin/env python3
import sys
from parse import parse_code
from assemble import assemble_code
from link import link_code

def handle_inputs():
    """
    Handle command line inputs
    """
    # Handle no arguments case immediately
    if (len(sys.argv) == 1):
        sys.exit("triangulate: fatal error: no input files.")
    
    input_args = sys.argv[1:]
    
    # print help text
    if ("-h" in input_args):
        print("Converts a ternary assembly .tas file into a ternary machine code (.tri) program.")
        print("Usage:")
        print("python3 ./triangulate.py [-debug] [input_file1] ... [input_fileN] -o [output_file]")
        # do nothing, even if there were other arguments
        sys.exit(1)
    
    # turn on debug printing if required
    debug_mode = False
    if ("-debug" in input_args):
        debug_mode = True
    
    # find input and output files
    if ("-o" in input_args):
        index = input_args.index("-o")
        try:
            out_file = input_args[index + 1]
        except IndexError:
            print("triangulate: fatal error: no output file specified.")
            sys.exit(1)
        
        in_files = []
        for arg in input_args:
            if arg[-4:] == ".tas":
                in_files.append(arg)
    else:
        out_file = "a.tri"
        in_files = []
        for arg in input_args:
            if arg[-4:] == ".tas":
                in_files.append(arg)
    
    if len(in_files) == 0:
        print("triangulate: fatal error: no input files given.")
        print("Input files must have a .tas extension.")
        sys.exit(1)

    filenames = in_files + [out_file]
    return filenames, debug_mode

def triangulate(input_code, debug_mode):
    """
    Main assembler function. Parses, assembles and links the input text file to a string of Trytes,
    ready for the computer to read.
    """
    parsed_code = parse_code(input_code, debug_mode)
    assembled_code = assemble_code(parsed_code, debug_mode)
    linked_code = link_code(assembled_code, debug_mode)
    assembly_string = ' '.join(linked_code)
    if debug_mode:
        print(assembly_string)
    return assembly_string

if __name__ == "__main__":
    filenames, debug_mode = handle_inputs()
    input_filenames = filenames[:-1]
    output_filename = filenames[-1]

    input_code = []
    for input_filename in input_filenames:
        with open(input_filename) as f:
            input_code += f.readlines()
    
    output_code = triangulate(input_code, debug_mode)

    # then write assembly_string to output file
    with open(output_filename, 'w') as writer:
        writer.write(output_code)