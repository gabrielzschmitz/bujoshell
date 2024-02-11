# Compiler and flags
CC = tcc
TCCFLAGS = -Wunsupported -Wwrite-strings
GCCFLAGS = -Wextra
CFLAGS = -std=c99 $(TCCFLAGS) -Wall -pedantic -O3 $(DFLAGS)

# Directories
PREFIX = /usr/local
INCLUDES = -I$(PREFIX)/include
LIBDIR = -L$(PREFIX)/lib

# Dependency flags
DEPFLAGS = -MD

# Combine flags
CPPFLAGS += $(INCLUDES) $(DEPFLAGS)
LDFLAGS += $(LIBDIR)
LDLIBS = $(shell pkg-config --libs ncursesw sqlite3)
