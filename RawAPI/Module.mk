## all RFIO API files copied from /GSI/staging/adsm/v60/ directory
## when using RFIO inside gsi, filename should look like:
##  rfio:lxgstore:/arch_name/file_name.lmd 

RAWAPI_NAME         = RawAPI

## normally should be like this for every module, but can be specific

RAWAPI_DIR         = $(RAWAPI_NAME)

RAWAPI_NOTLIBF     = 

ifeq ($(GO4_WITH_RFIO),true)

RAWAPI_H           = $(filter-out $(RAWAPI_NOTLIBF), $(wildcard $(RAWAPI_DIR)/*.h))
RAWAPI_S           = $(filter-out $(RAWAPI_NOTLIBF), $(wildcard $(RAWAPI_DIR)/*.c))
RAWAPI_O           = $(RAWAPI_S:.c=.$(ObjSuf))

RAWAPI_DEP         = $(RAWAPI_O:.$(ObjSuf)=.$(DepSuf))

endif

# used in the main Makefile

ALLHDRS +=  $(patsubst $(RAWAPI_DIR)/%.h, include/%.h, $(RAWAPI_H))

LIBDEPENDENC       += $(RAWAPI_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(RAWAPI_S) $(RAWAPI_H) $(RAWAPI_NOTLIBF)
endif


##### local rules #####

ifeq ($(shell uname -m),x86_64)
$(RAWAPI_O) : DEFINITIONS += -DSYSTEM64
endif

include/%.h: $(RAWAPI_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

clean-bin::
	@rm -f $(RAWAPI_O) $(RAWAPI_DEP)
