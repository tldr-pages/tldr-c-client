CC			?= gcc
DEBUG		?= -ggdb -O0 -march=native
OUTPUT		:= -MMD -MP

# Include Directories
INCLUDE		:= $(shell pkg-config --cflags libzip)
INCLUDE		+= -I/usr/include
INCLUDE		+= -I/usr/local/include
INCLUDE		+= -I/usr/local/opt/curl/include
INCLUDE		+= -I/usr/local/opt/libzip/include

# Compiler Flags
CFLAGS		:= $(CFLAGS) $(DEBUG) $(INCLUDE)
CFLAGS		+= -W -Wall -Wextra -pedantic -ansi
CFLAGS		+= -Wstrict-overflow -fno-strict-aliasing

# Linker Flags
LDFLAGS		:= $(LDFLAGS)
LDFLAGS		+= -lcurl -lzip -lm

# Lib Search Directories
LDLIBS		:= $(LDLIBS)
LDLIBS		+= -L/usr/lib
LDLIBS		+= -L/usr/local/lib
LDLIBS		+= -L/usr/local/opt/curl/lib
LDLIBS		+= -L/usr/local/opt/libzip/lib


PREFIX		?= /usr/local
SRC			:= $(wildcard *.c)
OBJ			:= $(SRC:.c=.o)
DEP			:= $(SRC:.c=.d)
DSYM		:= $(SRC:.c=.dSYM)
BIN			:= tldr
BINDIR		:= $(PREFIX)/bin

.PHONY: clean format
.DEFAULT_GOAL := all


all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(OUTPUT) -o $@

$(BIN): $(OBJ)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@


install: all
	install -d $(BINDIR)
	install tldr $(BINDIR)

clean:
	rm -rf $(OBJ) *~ $(BINS) $(DSYM) $(DEP) $(BIN)

format:
	astyle --options=.astylerc *.c

