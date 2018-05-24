DIR := $(shell cd "$( dirname "$0" )" && pwd)
WMC := $(shell cd $(DIR)/../.. && pwd)

ifeq ($(PREFIX),)
	PREFIX=$(WMC)
endif

DEFAULTBUILDDIR := build
DEBUGBUILDDIR	:= debug
RELEASEBUILDDIR	:= release
SOURCEDIR 		:= $(DIR)

MAKEFLAGS += --no-print-directory
CMAKEFLAGS += -DCMAKE_INSTALL_PREFIX:PATH=$(PREFIX)

.PHONY: build debug release clean help

build: $(DEFAULTBUILDDIR)/CMakeCache.txt
	@$(MAKE) -C $(DEFAULTBUILDDIR) install

release: CMAKEFLAGS += -DCMAKE_BUILD_TYPE=Release
release: $(RELEASEBUILDDIR)/CMakeCache.txt
	@$(MAKE) -C $(RELEASEBUILDDIR) install

debug: CMAKEFLAGS += -DCMAKE_BUILD_TYPE=Debug
debug: $(DEBUGBUILDDIR)/CMakeCache.txt
	@$(MAKE) -C $(DEBUGBUILDDIR) install

%/CMakeCache.txt:
	@mkdir -p $*
	@cd $* && cmake $(CMAKEFLAGS) $(SOURCEDIR)

clean:
	@rm -rf $(DEBUGBUILDDIR) $(DEFAULTBUILDDIR) $(RELEASEBUILDDIR)

help:
	@echo "Usage:"
	@echo "    make [option]"
	@echo ""
	@echo "Options:"
	@echo "    build*    : create build directory (cmake) and compile"
	@echo "    release   : same as 'build', but with NDEBUG"
	@echo "    debug     : same as 'build', but with debug symbols"
	@echo "    clean     : remove build directories"
	@echo ""
	@echo "    * = default"
	@echo ""

