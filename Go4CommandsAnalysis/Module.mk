CMDANAL_NAME        = Go4CommandsAnalysis

## normally should be like this for every module, but can be specific

CMDANAL_DIR         = $(CMDANAL_NAME)

CMDANAL_NOTLIBF     = $(CMDANAL_DIR)/Go4CommandsAnalysis.h

## must be similar for every module

CMDANAL_H           = $(filter-out $(CMDANAL_NOTLIBF) $(OLD_DICT), $(wildcard $(CMDANAL_DIR)/*.$(HedSuf)))
CMDANAL_S           = $(filter-out $(CMDANAL_NOTLIBF) $(OLD_DICT), $(wildcard $(CMDANAL_DIR)/*.$(SrcSuf)))
CMDANAL_O           = $(CMDANAL_S:.$(SrcSuf)=.$(ObjSuf))

CMDANAL_DEP         =  $(CMDANAL_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(CMDANAL_DIR)/%.h, include/%.h, $(CMDANAL_DIR)/TGo4AnalysisCommandList.h)

LIBDEPENDENC       += $(CMDANAL_DEP) 

ifdef DOPACKAGE
DISTRFILES         += $(CMDANAL_S) $(CMDANAL_H) $(CMDANAL_NOTLIBF)
endif

##### local rules #####

include/%.h: $(CMDANAL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(CMDANAL_O) $(CMDANAL_DEP)

