CXX			= clang++
DEBUG		= -O3
INCLUDE		= -I/usr/local/include -I/usr/local/opt/curl/include
CXXFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe
LDFLAGS		= -lcurl
LDLIBS		= -L/usr/local/opt/curl/lib

SRC			= tldr.cpp
OBJ			= $(SRC:.cpp=.o)
BINS		= $(SRC:.cpp=)
DSYM		= $(SRC:.cpp=.dSYM)


.PHONE:		clean all

tldr:		tldr.o
	$(CXX) $(LDFLAGS) -o tldr $(OBJ) $(LDLIBS)

tldr.o:		tldr.cpp tldr.h
	$(CXX) $(CXXFLAGS) -c tldr.cpp

all:		tldr

clean:
	rm -rf $(OBJ) *~ $(BINS) $(DSYM)

dist-clean:	clean
	rm -rf tldr
