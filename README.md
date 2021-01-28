# ternary_computer
## Introduction
This project is a virtual machine that runs on ternary logic - all machine instructions and data are encoded in trits - which can either be +, 0, or -. There are 9 trits in a tryte, which is the basic unit that programs are able to manipulate. The machine will be able to store programs and data on some kind of "disk", and one of these programs will hopefully be a kind of primitive OS that might boot to a shell and respond to user commands. We'll see

## A brief introduction to balanced ternary
Balanced ternary has the standard place value system, but with +, 0, - ; representing 1, 0 and -1 respectively. Counting in balanced ternary, we have: +, +-, +0, ++, +--, +-0, +-+, +0-, +00, +0+, ++-, ++0, +++, etc. Mathematical operations work as you'd expect; for example, take 23 + 12:

23 == +0--

12 == 0++0

      ----

      ++0- == 35. Carrying works as you'd think (+) + (+) = (-), carry +. Basically base 3 arithmetic, but shifted.

Subtraction is easy; just take the number you're subtracting and flip all its trits; 12 = ++, -12 = --. Then add. Multiplication and division work with the same algorithms from standard maths.
### Septavingtesmal representation
We need a concise way of representing the contents of a Tryte. Obviously we can use 9 characters in the ternary representation:
0t+0-+++00-

but a more concise way would be to use base 27 arithmetic, similarly to hexadecimal's use for representing bytes.
The most obvious choice for base 27 would be the character set
0123456789ABCDEFGHIJKLMNOPQ
so say, 600 could be written as (22 * 27) + 6 = M6.
But this computer uses a balanced septavingtesmal system? i.e.
MLKJIHGFEDCBA0abcdefghijklm
representing the values -13 <= 0 <= 13. Then 600 = 729 - 243 + 81 + 27 + 9 - 3 = +-+++-0 = 27 * 27 - (5 * 27) + 6 = aEf. This has the advantage of being easily readable from the balanced ternary representation: 00+;-++;+-0, with 00+ = a, -++ = E, +-0 = f. Changing the sign of a balanced ternary number is then just a case of flipping the case of the string, and leaving the zeroes alone.
A ternary machine code file consists of a bunch of Trytes
KAg 00c Ifd mMB ...
and so on.
### Ternary logic
I don't know so much about this, but you can extend boolean logic to balanced ternary too; + maps to true, - maps to false, and 0 maps to 'unknown'. This might have practical applications; at any rate ternary logic is a lot richer than standard boolean logic. For example, there are 2^(2+2) = 16 possible binary logic operators with two inputs, there are 3^(3+3) = 19683 possible ternary logic operators with two inputs! We can make all the standard logic ops, AND, OR, NOT, XOR, NAND, NOR, etc, plus many more.

## Progress so far
- Trytes and Trints (three Trytes stuck together, forming an 27-trit integer with values in the range -(3^27 - 1)/2 <= n <= (3^27 - 1)/2.) implemented with most operations defined.
- CPU class written with 27 Tryte registers (which can be operated in groups of three as Trints) and operations defined on them
- Memory implemented- fixed at 3^9 = 19,683 Trytes now. Addresses run from $MMM-$mmm
- In lieu of an actual file system, disk filenames can be set as command line arguments. Up to 27 disks can be used at one time, with up to 19,683 Trytes of addressable disk space on each.
- An assembler (not quite finished) written in Python, converting more human readable instructions to ternary machine code.

## To do
- Fix implementation of jump instructions in assembler; these should be able to be relative to source code (as in, JP 20 would mean jump to line 20, or perhaps implement labels) rather than compiled code
- Add documentation for ternary assembly language
- Test by writing more programs
- Write a simple shell? Support larger disks? Upgrade console to understand ANSI colour codes?

## How to run
Pull the repository, run 'make'. Executable will be written to ./build/release. Run 'make debug' to turn debug flags on.
The assembler is Python 3 code, and is run with the command
`python triangulate.py SOURCE-FILE -o OUTPUT-FILE`
If you like, use .tas extension for ternary assembly source files, and .tri for ternary machine code files.
Once your assembly is assembled, run the computer with the command
`./build/release/ternary_computer DISK0.tri DISK1.tri ...`
Disk 0 should be your assembled source file; Disks 1 and up can be data files, or extra programs- access these with a MOUNT n command.