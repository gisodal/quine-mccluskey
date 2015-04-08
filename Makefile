# ------------------------------------------------------------------------------
# Makefile
# ------------------------------------------------------------------------------

# To use this makefile on any source tree, only change the user variables
# below. The directory structure should abide by the following tree:
# 	- directory src      : put source files here (*.[c,cc,cpp])
# 	- directory include  : put header files here (*.h)

# ------------------------------------------------------------------------------
# User variables
# ------------------------------------------------------------------------------
# see 'Makefile' for more information

# project name
PROJECT =

# project version
VERSION    = 1
SUBVERSION = 0
PATCHLEVEL = 0

# library and include paths (space separated value)
LIBRARY_DIR =
INCLUDE_DIR =

# static and shared libraries to be linked (space separated values)
STATIC_LIBRARIES =
SHARED_LIBRARIES =

# install
PREFIX = $(shell cd "$( dirname "$0" )" && cd ../.. && pwd)

# compiler
CC       = g++
EXT      = cc
CXXFLAGS = -std=c++11
CFLAGS   = -w
CDFLAGS  = -ggdb -Wall -Wextra -D DEBUG -Wno-format -Wno-write-strings -Wno-unused-function -Wno-system-headers
O        = -O3

# ------------------------------------------------------------------------------
# environment variables
# ------------------------------------------------------------------------------

# use bash instead of sh
SHELL=/bin/bash

# directories
BDIR = bin
LDIR = lib
ODIR = obj
SDIR = src
IDIR = include
TDIR = tar
UDIR = usr
DIR  = $(shell cd "$( dirname "$0" )" && pwd)
ARCH = $(shell getconf LONG_BIT)

# containting directory is default project name
ifeq ($(PROJECT),)
	PROJECT=$(shell basename $(DIR))
endif

# install dir
ifeq ($(PREFIX),)
	PREFIX=usr
endif

# sources, objects and dependecies
SRCS = $(wildcard $(SDIR)/*.$(EXT))
OBJS = $(patsubst $(SDIR)/%.$(EXT),$(ODIR)/%.o,$(SRCS))
LIBOBJS = $(filter-out $(ODIR)/main.o, $(OBJS))
DEPS = $(OBJS:.o=.d)

# library / include paths
INCLUDE_DIR += $(IDIR)
LIB = $(foreach d, $(LIBRARY_DIR),-L$d)
INC = $(foreach d, $(INCLUDE_DIR),-I$d)

# shared/static libraries to link
STATIC = $(foreach l, $(STATIC_LIBRARIES),-l$l)
SHARED = $(foreach l, $(SHARED_LIBRARIES),-l$l)

ifneq ($(STATIC),)
	LIB += -Wl,-Bstatic $(STATIC)
endif

ifneq ($(SHARED),)
	LIB += -Wl,-Bdynamic $(SHARED)
else ifneq ($(STATIC),)
	LIB += -Wl,-Bdynamic
endif

STATICLIB = lib$(PROJECT).a
DYNAMICLIB = lib$(PROJECT).so.$(VERSION).$(SUBVERSION).$(PATCHLEVEL)

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------

# rules not representing files
.PHONY: all $(PROJECT) install build rebuild debug odebug static dynamic debug-library library profile assembly clean tarball lines help

# default rule
$(PROJECT): build

# include file dependencies
-include $(DEPS)

# main compile rules
build: $(BDIR)/$(PROJECT)

rebuild: clean build

# compile with debug symbols
debug: CFLAGS = $(CDFLAGS)
debug: O = -O0
debug: build

# compile with optimizations and debug symbols
odebug: CFLAGS = $(CDFLAGS)
odebug: build

# compile with profile
profile: O = -O0 -pg
profile: build

# install to PREFIX
install: build
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/lib$(ARCH)
	cp $(BDIR)/$(PROJECT) $(PREFIX)/bin
	cp $(LDIR)/* $(PREFIX)/lib$(ARCH)

# create libraries
debug-library:
	$(MAKE) library CFLAGS='$(CDFLAGS)' O='-O0'

library:
	@echo '==== Creating static library ===='
	$(RM) -r $(ODIR)
	$(MAKE) static
	@echo '==== Creating dynamic library ===='
	$(RM) -r $(ODIR)
	$(MAKE) dynamic
	$(RM) -r $(ODIR)

# create static library
static: $(LDIR)/$(STATICLIB)

$(LDIR)/$(STATICLIB): $(ODIR) $(LIBOBJS) $(LDIR)
	ar rcs $(LDIR)/$(STATICLIB) $(LIBOBJS)

# create dynamic library
dynamic: $(LDIR)/$(DYNAMICLIB)

$(LDIR)/$(DYNAMICLIB): CFLAGS += -fPIC
$(LDIR)/$(DYNAMICLIB): $(ODIR) $(LIBOBJS) $(LDIR)
	gcc -shared -Wl,-soname,lib$(PROJECT).so.$(VERSION) -o $(LDIR)/$(DYNAMICLIB) $(LIBOBJS)
	ln -sf $(DYNAMICLIB) $(LDIR)/lib$(PROJECT).so
	ln -sf $(DYNAMICLIB) $(LDIR)/lib$(PROJECT).so.$(VERSION)
	ln -sf $(DYNAMICLIB) $(LDIR)/lib$(PROJECT).so.$(VERSION).$(SUBVERSION)

# compile to assembly
assembly: CFLAGS += -Wa,-a,-ad
assembly: build

# create object files and dependencies
$(ODIR)/%.o: $(SDIR)/%.$(EXT)
	$(CC) -o $@ -c $< $(O) $(CXXFLAGS) $(CFLAGS) $(INC) -MMD

# create (link) executable binary
$(BDIR)/$(PROJECT): $(ODIR) $(OBJS) $(BDIR)
	$(CC) -o $@ $(OBJS) $(LIB) $(DYNAMIC)

# create directories
$(LDIR):
	mkdir $(LDIR)

$(BDIR):
	mkdir $(BDIR)

$(ODIR):
	mkdir $(ODIR)

$(IDIR):
	mkdir $(IDIR)

$(TDIR):
	mkdir $(TDIR)

# create a tarball from source files
tarball: TARFILE = $$(echo $(TDIR)/$(PROJECT)_$$(date +"%Y_%m_%d_%H_%M_%S") | tr -d ' ').tar.xz
tarball: $(TDIR)
	@XZ_OPT="-9" tar --exclude=".*" -cvJf $(TARFILE) $(IDIR) $(SDIR) Makefile README*; echo;
	@if [ -f $(TARFILE) ]; then                    \
	     echo "Created file: $(TARFILE)";          \
	 else                                          \
	     echo "Tarball '$(TARFILE)' not created";  \
	 fi;

# print how many lines of code to compile
lines:
	@wc -l include/* src/*

# cleanup
clean:
	$(RM) -r $(ODIR) $(BDIR) $(LDIR)

# echo make options
help:
	@echo "Usage     :"
	@echo "    make [option]"
	@echo ""
	@echo "Options   :"
	@echo "    build*   : compile"
	@echo "    rebuild  : recompile"
	@echo "    debug    : compile with debug symbols"
	@echo "    odebug   : compile with optimizations and debug symbols"
	@echo "    lines    : print #lines of code to compile"
	@echo "    library  : create libraries"
	@echo "    install  : install project at PREFIX"
	@echo "    clean    : remove object files and binary"
	@echo "    tarball  : create tarball of source files"
	@echo ""
	@echo "    * = default"
	@echo ""
	@echo "Directory hierarchy :"
	@echo "    src      : source files (*.[c,cc,cpp])"
	@echo "    include  : header files (*.h)"
	@echo "    obj      : object and dependency files"
	@echo "    lib      : static/shared libraries"
	@echo "    bin      : executabe binary"
	@echo "    tar      : tarballs of source"

