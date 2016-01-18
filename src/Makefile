CC			?= clang
DEBUG		?= -g -O0 -march=native

# Include Directories
INCLUDE		:= -I/usr/local/include
INCLUDE		+= -I/usr/include
INCLUDE		+= -I/usr/local/opt/curl/include
INCLUDE		+= -I/usr/local/opt/libzip/include
INCLUDE		+= $(shell pkg-config --cflags libzip)

# Compiler Flags
CFLAGS		:= $(DEBUG)
CFLAGS		+= -Wall -Wextra -fno-strict-aliasing -std=c89 -pedantic
CFLAGS		+= $(INCLUDE)

# Linker Flags
LDFLAGS		:= -lcurl -lzip -lm

# Lib Search Directories
LDLIBS		:= -L/usr/local/opt/curl/lib
LDLIBS		+= -L/usr/local/opt/libzip/lib


PREFIX		?= /usr/local
SRC			:= tldr.c
OBJ			:= $(SRC:.c=.o)
DSYM		:= $(SRC:.c=.o.dSYM)
BIN			:= tldr
BINDIR		:= $(PREFIX)/bin


all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<


install: all
	install -d $(BINDIR)
	install tldr $(BINDIR)


.PHONY: clean dist-clean

clean:
	rm -rf $(OBJ) *~ $(BINS) $(DSYM)

dist-clean:	clean
	rm -rf $(BIN)

