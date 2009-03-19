#general make file for any Go4 module

include $(GO4SYS)/Makefile.config

# for compile example under cygwin, 
# relative path to GO4SYS directory should be specified
ifdef GO4_WIN32
  GO4SYS = ..
endif   

# comment to switch on optimization
DOOPTIMIZATION = false

# to add some more definitions, which can be used in #ifdef inside user code,
# just uncomment and edit following line
#DEFINITIONS += -D__USERDEF__

# if there are include files in other directories,
# just uncomment and edit following file
#INCLUDES += -I/home/usr/special/includes

# if some special libraries must be linked to the analysis
# uncomment and edit following line
#LIBS_FULLSET += -L/home/usr/special/lib -lspecial

all: build

include Module.mk

build: all-$(MODULE_NAME)

clean-obj: clean-obj-$(MODULE_NAME)
	@echo "Clean objects in $(MODULE_NAME)"

clean: clean-$(MODULE_NAME)
	@echo "Clean everything in $(MODULE_NAME)"

ifdef DOMAP
ifeq ($(wildcard $(GO4MAP)),)
map:
	@echo "Map for Go4 classes not generated. Call make map in $(GO4SYS) directory"
else
map: map-$(MODULE_NAME)
	@echo "Map for $(MODULE_NAME) done"
endif
else
map:
	@echo "Map for $(MODULE_NAME) can not be generated with ROOT $(shell root-config --version)"
endif

ifdef DOPACKAGE
package:
	@tar chf $(MODULE_NAME).tar Module.mk
	@tar rhf $(MODULE_NAME).tar $(subst $(CURDIR),.,$(DISTRFILES))
	@tar rhf $(MODULE_NAME).tar Makefile --ignore-failed-read
	@gzip -f $(MODULE_NAME).tar
	@echo "$(MODULE_NAME).tar.gz done"
endif

include $(GO4SYS)/Makefile.rules
