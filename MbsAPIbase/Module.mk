MBSAPIBASE_NAME     = MbsAPIbase

## normally should be like this for every module, but can be specific

MBSAPIBASE_DIR         = $(GO4SYS)/$(MBSAPIBASE_NAME)

MBSAPIBASE_NOTLIBF     = $(wildcard $(MBSAPIBASE_DIR)/m_*.c)

MBSAPIBASE_H           = $(filter-out $(MBSAPIBASE_NOTLIBF), $(wildcard $(MBSAPIBASE_DIR)/*.h))
MBSAPIBASE_S           = $(filter-out $(MBSAPIBASE_NOTLIBF), $(wildcard $(MBSAPIBASE_DIR)/*.c))
MBSAPIBASE_O           = $(MBSAPIBASE_S:.c=.$(ObjSuf))

MBSAPIBASE_DEP         = $(MBSAPIBASE_O:.$(ObjSuf)=.$(DepSuf))


# used in the main Makefile

ALLHDRS +=  $(patsubst $(MBSAPIBASE_DIR)/%.h, $(GO4SYS)/include/%.h, $(MBSAPIBASE_H))

LIBDEPENDENC       += $(MBSAPIBASE_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(MBSAPIBASE_S) $(MBSAPIBASE_H) $(MBSAPIBASE_NOTLIBF)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(MBSAPIBASE_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

clean::
	@rm -f $(MBSAPIBASE_O)
	@rm -f $(MBSAPIBASE_DEP)
