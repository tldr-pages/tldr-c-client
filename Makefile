CC			= clang
DEBUG		= -O3
INCLUDE		= -I/usr/local/include -I/usr/local/opt/curl/include
CFLAGS		= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe
LDFLAGS		= -lcurl
LDLIBS		= -L/usr/local/opt/curl/lib

SRC			= tldr.c
OBJ			= $(SRC:.c=.o)
BINS		= $(SRC:.c=)


.PHONE:		clean all

tldr:		tldr.o
	$(CC) $(LDFLAGS) -o tldr $(OBJ) $(LDLIBS)

tldr.o:		tldr.c tldr.h
	$(CC) $(CFLAGS) -c tldr.c

all:		tldr

clean:
	rm -rf $(OBJ) *~ $(BINS)

dist-clean:	clean
	rm -rf tldr
