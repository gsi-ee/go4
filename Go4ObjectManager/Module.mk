GO4OBJM_NAME     = Go4ObjectManager

## normally should be like this for every module, but can be specific

GO4OBJM_DIR         = $(GO4OBJM_NAME)

GO4OBJM_NOTLIBF     =

## must be similar for every module

GO4OBJM_H           = $(filter-out $(GO4OBJM_NOTLIBF), $(wildcard $(GO4OBJM_DIR)/*.$(HedSuf)))
GO4OBJM_S           = $(filter-out $(GO4OBJM_NOTLIBF), $(wildcard $(GO4OBJM_DIR)/*.$(SrcSuf)))
GO4OBJM_O           = $(GO4OBJM_S:.$(SrcSuf)=.$(ObjSuf))

GO4OBJM_DEP         =  $(GO4OBJM_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4OBJM_DIR)/%.h, include/%.h, $(GO4OBJM_H))

LIBDEPENDENC       += $(GO4OBJM_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4OBJM_S) $(GO4OBJM_H)
DISTRFILES         += $(GO4OBJM_DIR)/*.C
endif

##### local rules #####

include/%.h: $(GO4OBJM_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(GO4OBJM_O) $(GO4OBJM_DEP)
