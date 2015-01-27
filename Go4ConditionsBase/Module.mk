CONDBASE_NAME        = Go4ConditionsBase

## normally should be like this for every module, but can be specific

CONDBASE_DIR         = $(CONDBASE_NAME)

CONDBASE_NOTLIBF     =

## must be similar for every module

CONDBASE_H           = $(filter-out $(CONDBASE_NOTLIBF) $(OLD_DICT), $(wildcard $(CONDBASE_DIR)/*.$(HedSuf)))
CONDBASE_S           = $(filter-out $(CONDBASE_NOTLIBF) $(OLD_DICT), $(wildcard $(CONDBASE_DIR)/*.$(SrcSuf)))
CONDBASE_O           = $(CONDBASE_S:.$(SrcSuf)=.$(ObjSuf))

CONDBASE_DEP         =  $(CONDBASE_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(CONDBASE_DIR)/%.h, include/%.h, $(CONDBASE_H))

LIBDEPENDENC       += $(CONDBASE_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(CONDBASE_S) $(CONDBASE_H)
endif

ifdef GO4_WIN32
$(CONDBASE_O): CXXFLAGS += -DBUILDING_GO4BASE_DLL
endif

##### local rules #####

include/%.h: $(CONDBASE_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(CONDBASE_O) $(CONDBASE_DEP)
