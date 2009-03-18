MBSAPI_NAME         = MbsAPI
MODULE_NAME         = $(MBSAPI_NAME)

## normally should be like this for every module, but can be specific

MBSAPI_DIR         = $(GO4SYS)/$(MBSAPI_NAME)

MBSAPI_NOTLIBF     = $(wildcard $(MBSAPI_DIR)/m_*.c)

MBSAPI_H           = $(filter-out $(MBSAPI_NOTLIBF), $(wildcard $(MBSAPI_DIR)/*.h))
MBSAPI_S           = $(filter-out $(MBSAPI_NOTLIBF), $(wildcard $(MBSAPI_DIR)/*.c))
MBSAPI_O           = $(MBSAPI_S:.c=.$(ObjSuf))

MBSAPI_DEP         = $(MBSAPI_O:.$(ObjSuf)=.$(DepSuf))


# used in the main Makefile

ALLHDRS +=  $(patsubst $(MBSAPI_DIR)/%.h, $(GO4SYS)/include/%.h, $(MBSAPI_H))

LIBDEPENDENC       += $(MBSAPI_DEP)

DEFINITIONS += -D_LARGEFILE64_SOURCE


ifdef DOPACKAGE
DISTRFILES         += $(MBSAPI_S) $(MBSAPI_H) $(MBSAPI_NOTLIBF)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(MBSAPI_DIR)/%.h
	@cp -f $< $@

all-$(MBSAPI_NAME):     $(MBSAPI_O)

clean-obj-$(MBSAPI_NAME):
		@rm -f $(MBSAPI_O)

clean-$(MBSAPI_NAME): clean-obj-$(MBSAPI_NAME)
		@rm -f $(MBSAPI_DEP)
