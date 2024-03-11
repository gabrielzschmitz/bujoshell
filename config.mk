# Directories
PREFIX = /usr/local
APPPREFIX = $(PREFIX)/share/applications
DATAPREFIX = .local/share/bujoshell
INCLUDES = -I$(PREFIX)/include
LIBDIR = -L$(PREFIX)/lib

# Combine flags
CPPFLAGS += $(INCLUDES) $(DEPFLAGS)
LDFLAGS += $(LIBDIR)
ifdef __APPLE__
LDLIBS = -lncurses -lsqlite3
else
LDLIBS = $(shell pkg-config --libs ncursesw sqlite3)
endif

# Dependency flags
DEPFLAGS = -MD

# Compiler and flags
CC = gcc
TCCFLAGS = -Wwrite-strings
GCCFLAGS = -Wextra
DFLAGS = -DDATADIR=\"$(DATAPREFIX)\" -DDBFILE=\"$(DATAPREFIX)/bujoshell.db\"
CFLAGS = -std=c99 $(GCCFLAGS) -Wall -pedantic -O3 $(DFLAGS)
