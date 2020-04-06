##Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

##Gets a list of source files from /src
src = $(wildcard src/*.c)
lib = $(wildcard lib/gumbo/*.c) \
    $(wildcard lib/deque/*.c) \
    $(wildcard lib/url_parser/*.c)

CC=gcc
CFLAGS= -g -Wall -std=gnu99 -I -O3.
DEPS = $(src:.h) $(lib:.h)
OBJ = $(src:.c=.o) $(lib:.c=.o)
EXE = crawler

##Create .o files from .c files. Searches for .c files with same .o names given in OBJ
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

##Create executable linked file from object files.
$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

##Delete object files
clean:
	/bin/rm $(OBJ)

##Performs clean (i.e. delete object files) and deletes executable
clobber: clean
	/bin/rm $(EXE)
