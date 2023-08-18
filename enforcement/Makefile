include ../analysis/tools/egalito/env.mk
CXXFLAGS    += -g -I ../analysis/tools/egalito/src/ -I src/
CLDFLAGS    += -L ../analysis/tools/egalito/src/$(BUILDDIR) -legalito \
	-Wl,-rpath=$(abspath ../analysis/tools/egalito/src/$(BUILDDIR)) \
	-Wl,-rpath=$(abspath ../analysis/tools/egalito/dep/capstone/install/lib)

SYSPART_ENFORCE_SOURCES = src/syspart_enforce.cpp
LOOP_SOURCES 		  = ../analysis/app/src/loopanalysis.cpp 
IPCALLGRAPH_SOURCES = ../analysis/app/src/ipcallgraph.cpp ../analysis/app/src/syspartUtility.cpp ../analysis/app/src/nss.cpp

exe-filename = $(foreach s,$1,$(BUILDDIR)$(dir $s)$(basename $(notdir $s)))
obj-filename = $(foreach s,$1,$(BUILDDIR)$(dir $s)$(basename $(notdir $s)).o)
dep-filename = $(foreach s,$1,$(BUILDDIR)$(dir $s)$(basename $(notdir $s)).d)

ETSYSPART_ENFORCE_SOURCES = $(SYSPART_ENFORCE_SOURCES) $(LOOP_SOURCES) $(IPCALLGRAPH_SOURCES)
ETSYSPART_ENFORCE_OBJECTS = $(call obj-filename, $(ETSYSPART_ENFORCE_SOURCES))

ALL_SOURCES = $(sort $(ETSYSPART_ENFORCE_SOURCES))

ALL_OBJECTS = $(call obj-filename,$(ALL_SOURCES))

BUILDTREE = $(sort $(dir $(ALL_OBJECTS) ))

ETSYSENFORCE = $(BUILDDIR)sysenforce

OUTPUTS =  $(ETSYSENFORCE)

# Default target
.PHONY: all
all: app-all .symlinks
	@true

app-all: $(OUTPUTS) | rebuild-src

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
Makefile: rebuild-src
endif

$(ALL_OBJECTS): | $(BUILDTREE)
$(BUILDTREE): ../analysis/tools/egalito/config/config.h
	@mkdir -p $@

../config/config.h:
	$(call short-make,../analysis/tools/egalito/config)


.symlinks: $(OUTPUTS)
	@touch .symlinks
	@ln -sf $(ETSYSENFORCE)
	@cp $(shell pwd)/../analysis/tools/egalito/src/$(BUILDDIR)libegalito.so $(BUILDDIR)

.PHONY: rebuild-src
rebuild-src:
	$(call short-make,../analysis/tools/egalito/src)

# Dependencies
DEPEND_FILES = $(call dep-filename,$(ALL_SOURCES))
-include $(DEPEND_FILES)

$(ETSYSENFORCE) : $(ETSYSPART_ENFORCE_OBJECTS)
	$(SHORT_LINK) $(CXXFLAGS) -o $@ $^ $(CLDFLAGS) -Wl,-rpath=$(abspath ../analysis/tools/egalito/src)
$(ETSYSENFORCE): ../analysis/tools/egalito/src/$(BUILDDIR)libegalito.so
