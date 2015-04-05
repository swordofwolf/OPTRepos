CFLAGS=-g -Wall -std=c99
YACC=bison --debug -d -l -o parser.tab.c
LEX=flex
LFLAGS=-i -o lex.yy.c --noline
CC=gcc

# Things that get included in our Yacc file
INCLUDED_FILES = \
	codegen.h \
	subexpression.h \
	symtable.h \
	types.h \
	labels.h

# The various .o files that are needed for executables.
OBJECT_FILES = parser.tab.o codegen.o lex.yy.o symtable.o types.o labels.o

default: crabCompiler

crabCompiler: $(OBJECT_FILES)
	$(LINK.o) -o $@ $^

parser.tab.o: parser.tab.c $(INCLUDED_FILES)

parser.tab.c: parser.y
	$(YACC) $(YFLAGS) $^

lex.yy.c: lexer.l
	$(LEX) $(LFLAGS) $^

clean-all:
	make clean
	-rm -f crabCompiler
	-rm in
	-rm output
	-rm *~ 
clean:
	-rm -f *.o
	-rm -f parser.output
	-rm -f parser.tab.*
	-rm -f lex.yy.c

