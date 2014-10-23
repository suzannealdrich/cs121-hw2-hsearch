# Makefile for CS121

# Make sure we're getting the right version of gcc
CC 		= /usr/pubsw/bin/gcc

# Comment out the next line to turn off debugging (for gdb)
DEBUG		= -g

# Comment out the next line to turn off optimization
OPTIMIZE	= -O3

# Comment out to turn off profiling
# PROFILING	= -pg

# set additional include and library search directories
# INCLUDEDIR	= -I/usr/pubsw/include/tcl/ -I/usr/pubsw/X11/include
LIBDIR		= 

# Possible CCFLAGS include:
#	-Wall     -- Make the compiler generate warnings for questionable
#			code. (This should always be on.)
#
#	$(DEBUG)  -- Look at the DEBUG flag for debugging.  This should 
#			always be included as well.  If you want to turn
#			off debugging, do it in the DEBUG definition line.
#
#	$(INCLUDEDIR) -- Look for include files in these directories too.
#
#	$(LINKDIR) -- Look for libraries in these directories too.
#

# CFLAGS are all the flags to send during compilation and linking
CFLAGS 	= -Wall $(INCLUDEDIR) $(DEBUG) $(PROFILING) $(OPTIMIZE)
# LDFLAGS are all the flags to send during linking
LDFLAGS 	= $(DEBUG) $(PROFILING) $(LIBDIR) 
# CCLIBS are all the libraries to be linked
# -lm is the math library
CCLIBS		= -lm

PROG = hsearch
HDRS = pqueue.h
SRCS = hsearch.c pqueue.c
OBJS = $(SRCS:.c=.o)

all : $(PROG)

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(LDFLAGS) $(OBJS) $(CCLIBS)
 
hsearch.o: hsearch.c pqueue.c pqueue.h

clean : 
	@echo "Removing all object files..."
	rm -f core $(PROG) $(OBJS) *~
