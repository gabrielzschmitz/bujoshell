include config.mk

# Macros for source files and object files
SRCS = bujoshell.c draw.c init.c input.c update.c util.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: bujoshell

bujoshell: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -rf bujoshell *.o *.d
