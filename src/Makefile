CC			= clang
DEBUG		= -g -O0

# Include directories
INCLUDE		:= -I/usr/local/include
INCLUDE		+= -I/usr/include
INCLUDE		+= -I/usr/local/opt/curl/include
INCLUDE		+= -I/usr/local/opt/libzip/include
INCLUDE		+= -I/usr/local/Cellar/libzip/1.0.1/include
INCLUDE		+= -I/usr/local/Cellar/libzip/1.0.1/lib/libzip/include
INCLUDE		+= $(shell pkg-config --cflags libzip)

CCFLAGS		= $(DEBUG) -Wall -Wextra -fno-strict-aliasing -march=native -std=c89 -pedantic $(INCLUDE)
LDFLAGS		= -lcurl -lzip -lm
LDLIBS		= -L/usr/local/opt/curl/lib

ifndef PREFIX
	PREFIX	= /usr/local
endif
SRC			= tldr.c
OBJ			= $(SRC:.c=.o)
DSYM		= $(SRC:.c=.dSYM)
BIN			= tldr
BINDIR		= $(PREFIX)/bin


all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o $@

.c.o:
	$(CC) $(CCFLAGS) -c $<


install: all
	install -d $(BINDIR)
	install tldr $(BINDIR)

clean:
	rm -rf $(OBJ) *~ $(BINS) $(DSYM)

dist-clean:	clean
	rm -rf tldr
