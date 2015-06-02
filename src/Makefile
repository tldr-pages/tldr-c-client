CXX			= clang++
DEBUG		= -O3
INCLUDE		= -I/usr/local/include -I/usr/local/opt/curl/include
CXXFLAGS	= $(DEBUG) -Wall $(INCLUDE)
LDFLAGS		= -lcurl
LDLIBS		= -L/usr/local/opt/curl/lib

ifndef PREFIX
	PREFIX	= /usr/local
endif
SRC			= tldr.cpp utils.cpp
OBJ			= $(SRC:.cpp=.o)
DSYM		= $(SRC:.cpp=.dSYM)
BIN			= tldr
BINDIR		= $(PREFIX)/bin


all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) $(LDLIBS) $(OBJ) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<


install: all
	install -d $(BINDIR)
	install tldr $(BINDIR)

clean:
	rm -rf $(OBJ) *~ $(BINS) $(DSYM)

dist-clean:	clean
	rm -rf tldr
