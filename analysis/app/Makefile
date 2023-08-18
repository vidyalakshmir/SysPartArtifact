# Makefile for egalito applications

include ../tools/egalito/env.mk
CXXFLAGS    += -g -I ../tools/egalito/src/ -I src/
CLDFLAGS    += -L ../tools/egalito/src/$(BUILDDIR) -legalito \
	-Wl,-rpath=$(abspath ../tools/egalito/src/$(BUILDDIR)) \
	-Wl,-rpath=$(abspath ../tools/egalito/dep/capstone/install/lib)

LOOP_SOURCES 		  = src/loopanalysis.cpp 
TEST_LOOP_SOURCES	  = src/testLoops.cpp
SYSPART_SOURCES 	  = src/syspart.cpp src/testSyspart.cpp
NSS_SOURCES           = src/nss.cpp
IPCALLGRAPH_SOURCES   = src/ipcallgraph.cpp src/syspartUtility.cpp
NONRETURN_SOURCES 	  = src/nonreturnAnalysis.cpp
SYSPART_UTILITY_SOURCES = src/syspartUtility.cpp

exe-filename = $(foreach s,$1,$(BUILDDIR)$(dir $s)$(basename $(notdir $s)))
obj-filename = $(foreach s,$1,$(BUILDDIR)$(dir $s)$(basename $(notdir $s)).o)
dep-filename = $(foreach s,$1,$(BUILDDIR)$(dir $s)$(basename $(notdir $s)).d)

ETLOOP_SOURCES = $(TEST_LOOP_SOURCES) $(LOOP_SOURCES)
ETLOOP_OBJECTS = $(call obj-filename,$(ETLOOP_SOURCES))

ETSYSPART_SOURCES = $(SYSPART_SOURCES) $(IPCALLGRAPH_SOURCES) $(LOOP_SOURCES) $(NONRETURN_SOURCES) $(SYSPART_UTILITY_SOURCES) $(NSS_SOURCES)
ETSYSPART_OBJECTS = $(call obj-filename, $(ETSYSPART_SOURCES))

ALL_SOURCES = $(sort $(ETLOOP_SOURCES) $(ETSYSPART_SOURCES))

ALL_OBJECTS = $(call obj-filename,$(ALL_SOURCES))

BUILDTREE = $(sort $(dir $(ALL_OBJECTS) ))

ETLOOP		 = $(BUILDDIR)loops
ETSYSPART = $(BUILDDIR)syspart

OUTPUTS = $(ETLOOP) $(ETSYSPART)

# Default target
.PHONY: all
all: app-all .symlinks
	@true

app-all: $(OUTPUTS) | rebuild-src

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
Makefile: rebuild-src
endif

$(ALL_OBJECTS): | $(BUILDTREE)
$(BUILDTREE): ../tools/egalito/config/config.h
	@mkdir -p $@

../config/config.h:
	$(call short-make,../tools/egalito/config)

.symlinks: $(OUTPUTS)
	@touch .symlinks
	@ln -sf $(ETLOOP)
	@ln -sf $(ETSYSPART)
	@cp $(shell pwd)/../tools/egalito/src/$(BUILDDIR)libegalito.so $(BUILDDIR)

.PHONY: rebuild-src
rebuild-src:
	$(call short-make,../tools/egalito/src)

# Dependencies
DEPEND_FILES = $(call dep-filename,$(ALL_SOURCES))
-include $(DEPEND_FILES)

# Programs and libraries

$(ETLOOP): $(ETLOOP_OBJECTS)
	$(SHORT_LINK) $(CXXFLAGS) -o $@ $^ $(CLDFLAGS) -Wl,-rpath=$(abspath ../tools/egalito/src)
$(ETLOOP): ../tools/egalito/src/$(BUILDDIR)libegalito.so

$(ETSYSPART): $(ETSYSPART_OBJECTS)
	$(SHORT_LINK) $(CXXFLAGS) -o $@ $^ $(CLDFLAGS) -Wl,-rpath=$(abspath ../tools/egalito/src)
$(ETSYSPART): ../tools/egalito/src/$(BUILDDIR)libegalito.so

# Other targets
.PHONY: clean realclean
clean:
	-rm -rf $(BUILDDIR) .symlinks loops syspart
