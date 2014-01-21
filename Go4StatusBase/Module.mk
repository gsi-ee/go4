STATBASE_NAME        = Go4StatusBase

## normally should be like this for every module, but can be specific

STATBASE_DIR         = $(STATBASE_NAME)

STATBASE_NOTLIBF     =

## must be similar for every module

STATBASE_H           = $(filter-out $(STATBASE_NOTLIBF), $(wildcard $(STATBASE_DIR)/*.$(HedSuf)))
STATBASE_S           = $(filter-out $(STATBASE_NOTLIBF), $(wildcard $(STATBASE_DIR)/*.$(SrcSuf)))
STATBASE_O           = $(STATBASE_S:.$(SrcSuf)=.$(ObjSuf))

STATBASE_DEP         =  $(STATBASE_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(STATBASE_DIR)/%.h, include/%.h, $(STATBASE_H))

LIBDEPENDENC       += $(STATBASE_DEP)

ifdef DOPACKAGE
STATBASE_DISTRFILES = $(STATBASE_S) $(STATBASE_H)
DISTRFILES         += $(STATBASE_DISTRFILES)
endif


##### local rules #####

include/%.h: $(STATBASE_DIR)/%.h
	@cp -f $< $@

clean-bin::
	@rm -f $(STATBASE_O) $(STATBASE_DEP)
