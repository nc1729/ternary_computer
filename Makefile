# --This is a Makefile from StackOverflow I adapted--
# First define variables used in the Makefile, then start defining rules

# Usage
# make

# Notation (for my reference)
# $@ - macro that refers to the target (the rule name)
# $^ - macro that refers to all dependencies
# $< - macro that refers to first dependency
# % -  macro to make a pattern that we want to watch in both the target and the dependency
# @ at the start of a command - stop the console from repeating it


#
# Compiler flags
#
CC = g++
CFLAGS = -Wall -Werror -Wextra

#
# Project files
#
SRCS = Tryte.cpp test.cpp main.cpp CPU.cpp Console.cpp Float.cpp FPU.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = ternary_computer

#
# Debug build settings
#
DBGDIR = build/debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -DDEBUG

#
# Release build settings
#
RELDIR = build/release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O2 -DNDEBUG

# Makes Makefile always see these as tasks, rather than potential files
.PHONY: all clean debug prep debug_prep release_prep release remake

# Default build
all: release_prep release

#
# Debug rules
#
debug: debug_prep $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^

$(DBGDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXE) $^

$(RELDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
debug_prep:
	@mkdir -p $(DBGDIR)

release_prep:
	@mkdir -p $(RELDIR)

prep:
	@mkdir -p $(DBGDIR) $(RELDIR)

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)