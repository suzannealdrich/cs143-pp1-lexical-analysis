## 
## Simple makefile for CS143 programming projects
##


.PHONY: clean strip
# Set the default target. When you make with no arguments,
# this will be the target built.
TARGET = pp1
default: $(TARGET)

# "make pure" will build a ppN.purify version of the executable
# which will execute much more slowly but have Purify's runtime
# memory protection checking on. Might be useful for debugging
pure: $(TARGET).purify

# Set up the list of source and object files
SRCS = utility.cc main.cc declaration.cc hash.c
# OBJS can deal with either .cc or .c files listed in SRCS
OBJS = lex.yy.o $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))
JUNK =  $(OBJS) lex.yy.c y.tab.c y.tab.h y.output *.core core $(TARGET).purify purify.log
# Define the tools we are going to use
CC= g++
LD = g++
LEX = flex
YACC = bison

# Set up the necessary flags for the tools

# We want most warnings, but lex/yacc may generated some static symbols
# we don't use, so turn on unused warnings to avoid that clutter
# Also STL has some signed/unsigned comparisons we want to supress
WITH_DEBUG = -g
CFLAGS = $(WITH_DEBUG) -Wall -Wno-unused -Wno-sign-compare

# The -d flag tells lex to set up for debugging. Can turn on/off by
# setting value of global yy_flex_debug
LEXFLAGS = -d

# The -d flag tells yacc to generate header with token types
# The -v flag writes out a verbose description of the states and conflicts
# The -t flag turns on debugging capability
# The -y flag means imitate yacc's output file naming conventions
YACCFLAGS = -dvty

# Link with standard c library, math library, and lex library
LIBS = -lc -lm -ll

# Rules for various parts of the target
lex.yy.o: lex.yy.c 
	$(CC) $(CFLAGS) -c -o lex.yy.o lex.yy.c

lex.yy.c: scanner.l 
	$(LEX) $(LEXFLAGS) scanner.l

.cc.o: $*.cc
	$(CC) $(CFLAGS) -c -o $@ $*.cc

$(TARGET) : $(OBJS)
	$(LD) -o $@ $(OBJS) $(LIBS)

$(TARGET).purify : $(OBJS)
	purify -log-file=purify.log -cache-dir=/tmp/$(USER) $(LD) -o $@ $(OBJS) $(LIBS)

# This target is to build small for testing (no debugging info), removes
# all intermediate products, too
strip : $(TARGET)
	strip $(TARGET)
	rm -rf $(JUNK)

#
# make depend will set up the header file dependencies for the 
# assignment.  You should make depend whenever you add a new header
# file to the project
#
depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

clean:
	rm -f $(JUNK) $(TARGET)

