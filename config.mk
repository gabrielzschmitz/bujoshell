# Compiler and flags
CC = gcc
TCCFLAGS = -Wwrite-strings
GCCFLAGS = -Wextra
CFLAGS = -std=c99 $(GCCFLAGS) -Wall -pedantic -O3 $(DFLAGS)

# Directories
PREFIX = /usr/local
APPPREFIX = $(PREFIX)/share/applications
INCLUDES = -I$(PREFIX)/include
LIBDIR = -L$(PREFIX)/lib

# Dependency flags
DEPFLAGS = -MD

# Combine flags
CPPFLAGS += $(INCLUDES) $(DEPFLAGS)
LDFLAGS += $(LIBDIR)
LDLIBS = $(shell pkg-config --libs ncursesw sqlite3)
