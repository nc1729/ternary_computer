import sys
def print_error(line_number, err_msg):
    print(("{}, line {}: " + err_msg).format(sys.argv[1], line_number))
    print("Compilation terminated.")
    sys.exit(1)
