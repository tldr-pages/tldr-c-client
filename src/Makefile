CC			= clang
DEBUG		= -g -O0
INCLUDE		= -I/usr/local/include -I/usr/local/opt/curl/include $(shell pkg-config --cflags libzip)
CCFLAGS		= $(DEBUG) -Wall -Wextra -fno-strict-aliasing -march=native -std=c89 -pedantic $(INCLUDE)
LDFLAGS		= -lcurl -lzip
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
