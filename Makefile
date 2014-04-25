CXX			= clang++
DEBUG		= -O3
INCLUDE		= -I/usr/local/include -I/usr/local/opt/curl/include
CXXFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe
LDFLAGS		= -lcurl
LDLIBS		= -L/usr/local/opt/curl/lib

ifndef PREFIX
	PREFIX	= /usr/local
endif
SRC			= tldr.cpp
OBJ			= $(SRC:.cpp=.o)
BINS		= $(SRC:.cpp=)
DSYM		= $(SRC:.cpp=.dSYM)
BINDIR		= $(PREFIX)/bin


.PHONY:		clean all install

tldr:		tldr.o
	$(CXX) $(LDFLAGS) -o tldr $(OBJ) $(LDLIBS)

tldr.o:		tldr.cpp tldr.h
	$(CXX) $(CXXFLAGS) -c tldr.cpp

all:		tldr

install:	all
	install -d $(BINDIR)
	install tldr $(BINDIR)

clean:
	rm -rf $(OBJ) *~ $(BINS) $(DSYM)

dist-clean:	clean
	rm -rf tldr
