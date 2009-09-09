MBSAPI_NAME         = MbsAPI

## normally should be like this for every module, but can be specific

MBSAPI_DIR         = $(MBSAPI_NAME)

MBSAPI_NOTLIBF     = $(wildcard $(MBSAPI_DIR)/m_*.c)

MBSAPI_H           = $(filter-out $(MBSAPI_NOTLIBF), $(wildcard $(MBSAPI_DIR)/*.h))
MBSAPI_S           = $(filter-out $(MBSAPI_NOTLIBF), $(wildcard $(MBSAPI_DIR)/*.c))
MBSAPI_O           = $(MBSAPI_S:.c=.$(ObjSuf))

MBSAPI_DEP         = $(MBSAPI_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(MBSAPI_DIR)/%.h, include/%.h, $(MBSAPI_H))

LIBDEPENDENC   += $(MBSAPI_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(MBSAPI_S) $(MBSAPI_H) $(MBSAPI_NOTLIBF)
endif

##### local rules #####

include/%.h: $(MBSAPI_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

clean-bin::
	@rm -f $(MBSAPI_O) $(MBSAPI_DEP)

$(MBSAPI_DIR)/fLmd.$(ObjSuf) : DEFINITIONS += -D_LARGEFILE64_SOURCE

ifeq ($(GO4_WITH_RFIO),true)
$(MBSAPI_DIR)/f_evt.$(ObjSuf) : DEFINITIONS += -DRFIO
endif

