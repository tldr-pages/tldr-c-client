# General
CC			= gcc
LD			= gcc
RM			= rm -rf
RMDIR		= rmdir
INSTALL		= install

# Config
TARGET		= tldr
SRCDIR		= src
OBJDIR		= obj
BINDIR		= .
MANDIR		= man

# CFLAGS, LDFLAGS, CPPFLAGS, PREFIX can be overriden on CLI
CFLAGS		:= -ggdb -O0 -ftrapv
CPPFLAGS	:=
LDFLAGS		:=
PREFIX		:= /usr/local
TARGET_ARCH :=


# Compiler Flags
ALL_CFLAGS		:= $(CFLAGS)
ALL_CFLAGS		+= -Wall -Wextra -pedantic
ALL_CFLAGS		+= -fno-strict-aliasing
ALL_CFLAGS		+= -Wuninitialized -Winit-self -Wfloat-equal
ALL_CFLAGS		+= -Wshadow -Wc++-compat -Wcast-qual -Wcast-align
ALL_CFLAGS		+= -Wconversion -Wsign-conversion -Wno-cast-qual
ALL_CFLAGS		+= -Wno-multichar -Wpacked -Wstrict-overflow -Wvla
ALL_CFLAGS		+= -Wformat -Wno-format-zero-length -Wstrict-prototypes
ifeq ($(CC),clang)
ALL_CFLAGS		+= -Wno-unknown-warning-option
endif

# Version Generation
HAS_GIT			:= $(shell type git > /dev/null 2>&1 && echo "1" || echo "0")
IS_GITREPO		:= $(shell [ -d .git ] && echo "1" || echo "0")
ifeq (0,$(filter 0,$(HAS_GIT) $(IS_GITREPO)))
VER				:= v1.6.0
else
VER				:= $(shell git describe --tags --always --dirty)
endif

# Set Flags
ALL_CPPFLAGS	:= $(CPPFLAGS) -DVERSION='"$(VER)"'
ALL_LDFLAGS		:= $(LDFLAGS) -L/usr/lib
ALL_LDLIBS		:= -lc -lm -lcurl -lzip
ALL_CPPFLAGS	+= -D_GNU_SOURCE
ALL_CPPFLAGS	+= $(shell pkg-config --cflags libzip)

ALL_CPPFLAGS	+= -I/usr/include
ALL_CPPFLAGS	+= -I/usr/local/include
ALL_LDFLAGS		+= -L/usr/local/lib
ifneq (,$(wildcard /opt/homebrew/.*))
	ALL_CCPFLAGS	+= -I/opt/homebrew/include
	ALL_CPPFLAGS	+= -I/opt/homebrew/lib
endif

ifneq (,$(wildcard /usr/local/opt/curl/.*))
	ALL_CPPFLAGS	+= -I/usr/local/opt/curl/include
	ALL_LDFLAGS		+= -L/usr/local/opt/curl/lib
endif
ifneq (,$(wildcard /opt/homebrew/opt/curl/.))
	ALL_CPPFLAGS	+= -I/opt/homebrew/opt/curl/include
	ALL_LDFLAGS		+= -L/opt/homebrew/opt/curl/lib
endif

ifneq (,$(wildcard /usr/local/opt/libzip/.))
	ALL_CPPFLAGS	+= -I/usr/local/opt/libzip/include
	ALL_LDFLAGS		+= -L/usr/local/opt/libzip/lib
endif
ifneq (,$(wildcard /opt/homebrew/opt/libzip/.))
	ALL_CPPFLAGS	+= -I/opt/homebrew/opt/libzip/include
	ALL_LDFLAGS		+= -L/opt/homebrew/opt/libzip/lib
endif

# Source, Binaries, Dependencies
SRC			:= $(shell find $(SRCDIR) -type f -name '*.c')
OBJ			:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRC:.c=.o))
DEP			:= $(OBJ:.o=.d)
BIN			:= $(BINDIR)/$(TARGET)
-include $(DEP)

# Man Pages
MANSRC		:= $(shell find $(MANDIR) -type f -name '*.1')
MANPATH		:= $(PREFIX)/share/man/man1


# Verbosity Control, ala automake
V 			= 0

# Verbosity for CC
REAL_CC 	:= $(CC)
CC_0 		= @echo "CC $<"; $(REAL_CC)
CC_1 		= $(REAL_CC)
CC 			= $(CC_$(V))

# Verbosity for LD
REAL_LD 	:= $(LD)
LD_0 		= @echo "LD $@"; $(REAL_LD)
LD_1 		= $(REAL_LD)
LD 			= $(LD_$(V))

# Verbosity for RM
REAL_RM 	:= $(RM)
RM_0 		= @echo "Cleaning..."; $(REAL_RM)
RM_1 		= $(REAL_RM)
RM 			= $(RM_$(V))

# Verbosity for RMDIR
REAL_RMDIR 	:= $(RMDIR)
RMDIR_0 	= @$(REAL_RMDIR)
RMDIR_1 	= $(REAL_RMDIR)
RMDIR 		= $(RMDIR_$(V))



# Build Rules
.PHONY: clean format lint infer
.DEFAULT_GOAL := all

all: setup $(BIN)
setup: dir
remake: clean all

dir:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)


$(BIN): $(OBJ)
	$(LD) $(ALL_LDFLAGS) $^ $(ALL_LDLIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(ALL_CFLAGS) $(ALL_CPPFLAGS) -c -MMD -MP -o $@ $<


install: all $(MANSRC)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) $(BIN) $(PREFIX)/bin
	$(INSTALL) -d $(MANPATH)
	$(INSTALL) $(MANSRC) $(MANPATH)

uninstall:
	$(RM) $(PREFIX)/bin/$(TARGET)
	$(RM) $(MANPATH)/$(TARGET).1

clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
	$(RMDIR) $(OBJDIR) $(BINDIR) 2> /dev/null; true

format:
	astyle --options=.astylerc $(SRCDIR)/*.c
	astyle --options=.astylerc $(SRCDIR)/*.h

lint:
	oclint -report-type html -o report.html \
		-enable-clang-static-analyzer \
		-enable-global-analysis \
		-disable-rule=GotoStatement \
		-max-priority-1 1000 \
		-max-priority-2 1000 \
		-max-priority-3 1000 \
		src/*.c src/*.h -- $(ALL_CPPFLAGS) -c
	cppcheck --enable=all \
		-I/usr/local/include \
		-I/usr/local/opt/curl/include \
		-I/usr/local/opt/libzip/include \
		--language=c \
		--std=c89 \
		--inconclusive \
		src/*.c src/*.h
	splint +posixlib +gnuextensions \
		-Du_int64_t=unsigned\ long\ long \
		-Du_int32_t=unsigned\ int \
		-D__int64_t=long\ long \
		-D__uint64_t=unsigned\ long\ long \
		-D__int32_t=int \
		-D__uint32_t=unsigned\ int \
		-D__int16_t=short \
		-D__uint16_t=unsigned\ short \
		-D__darwin_natural_t=long \
		-D__darwin_time_t=long \
		-D__darwin_size_t=unsigned\ long \
		-D__darwin_ssize_t=long \
		-D__darwin_intptr_t=unsigned\ long \
		-D__darwin_clock_t=unsigned\ long \
		-I/usr/local/include \
		-I/usr/local/opt/curl/include \
		-I/usr/local/opt/libzip/include \
		-I/usr/local/Cellar/libzip/1.1/lib/libzip/include \
		src/*.c src/*.h

infer: clean
	infer -- make


