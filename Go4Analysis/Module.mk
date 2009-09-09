GO4ANAL_NAME        = Go4Analysis

## normally should be like this for every module, but can be specific

GO4ANAL_DIR         = $(GO4ANAL_NAME)
GO4ANAL_LINKDEF     = $(GO4ANAL_DIR)/$(GO4ANAL_NAME)LinkDef.$(HedSuf)

VERSION_FILENAME    = TGo4Version

GO4ANAL_NOTLIBF     = $(GO4ANAL_DIR)/TGo4Analysis.h \
                      $(GO4ANAL_DIR)/anamacroinit.C \
                      $(GO4ANAL_DIR)/anamacroclose.C

## must be similar for every module

GO4ANAL_DICT        = $(GO4ANAL_DIR)/$(DICT_PREFIX)$(GO4ANAL_NAME)
GO4ANAL_DH          = $(GO4ANAL_DICT).$(HedSuf)
GO4ANAL_DS          = $(GO4ANAL_DICT).$(SrcSuf)
GO4ANAL_DO          = $(GO4ANAL_DICT).$(ObjSuf)


VERSION_H           = $(GO4ANAL_DIR)/$(VERSION_FILENAME).$(HedSuf)
VERSION_S           = $(GO4ANAL_DIR)/$(VERSION_FILENAME).$(SrcSuf)
VERSION_O           = $(GO4ANAL_DIR)/$(VERSION_FILENAME).$(ObjSuf)


GO4ANAL_H          := $(filter-out $(GO4ANAL_NOTLIBF) $(VERSION_H) $(GO4ANAL_DH) $(GO4ANAL_LINKDEF) $(GO4ANAL_OTHERF), $(wildcard $(GO4ANAL_DIR)/*.$(HedSuf)))
GO4ANAL_S          := $(filter-out $(GO4ANAL_NOTLIBF) $(VERSION_S) $(GO4ANAL_DS), $(wildcard $(GO4ANAL_DIR)/*.$(SrcSuf)))
GO4ANAL_O          := $(GO4ANAL_S:.$(SrcSuf)=.$(ObjSuf))

GO4ANAL_DEP         =  $(GO4ANAL_O:.$(ObjSuf)=.$(DepSuf))
GO4ANAL_DDEP        =  $(GO4ANAL_DO:.$(ObjSuf)=.$(DepSuf))

VERSION_DEP         =  $(VERSION_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4ANAL_DIR)/%.h, include/%.h, $(GO4ANAL_H) $(VERSION_H) $(GO4ANAL_DIR)/TGo4Analysis.h)

LIBDEPENDENC       += $(GO4ANAL_DEP) $(VERSION_DEP) $(GO4ANAL_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4ANAL_S) $(GO4ANAL_H) $(GO4ANAL_LINKDEF) $(VERSION_H) $(VERSION_S) $(GO4ANAL_NOTLIBF)
endif

##### local rules #####

include/%.h: $(GO4ANAL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(GO4ANAL_DS): $(GO4ANAL_H)  $(GO4ANAL_LINKDEF)
	@$(ROOTCINTGO4) $(GO4ANAL_H) $(GO4ANAL_LINKDEF)

clean-bin::
	@rm -f $(GO4ANAL_O) $(GO4ANAL_DO) $(VERSION_DEP)
	@rm -f $(GO4ANAL_DEP) $(GO4ANAL_DDEP) $(GO4ANAL_DS) $(GO4ANAL_DH)

clean::
	@rm -f $(VERSION_O)
