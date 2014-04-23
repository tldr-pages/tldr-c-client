CC			= clang
DEBUG		= -O3
INCLUDE		= -I/usr/local/include
CCFLAGS		= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

SRC			= tldr.c
OBJ			= $(SRC:.c=.o)
BINS		= $(SRC:.c=)


.PHONE:		clean all

tldr:		tldr.c
	$(CC) $(CCFLAGS) tldr.c -o tldr

all:		tldr

clean:
	rm -rf $(OBJ) *~ $(BINS)
