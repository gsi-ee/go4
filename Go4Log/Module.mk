GO4LOG_NAME        = Go4Log

## normally should be like this for every module, but can be specific

GO4LOG_DIR         = $(GO4LOG_NAME)

GO4LOG_NOTLIBF     =

## must be similar for every module

GO4LOG_H           = $(filter-out $(GO4LOG_NOTLIBF), $(wildcard $(GO4LOG_DIR)/*.$(HedSuf)))
GO4LOG_S           = $(filter-out $(GO4LOG_NOTLIBF), $(wildcard $(GO4LOG_DIR)/*.$(SrcSuf)))
GO4LOG_O           = $(GO4LOG_S:.$(SrcSuf)=.$(ObjSuf))

GO4LOG_DEP         =  $(GO4LOG_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile


ALLHDRS +=  $(patsubst $(GO4LOG_DIR)/%.h, include/%.h, $(GO4LOG_H))

LIBDEPENDENC       += $(GO4LOG_DEP) 

ifdef DOPACKAGE
GO4LOG_DISTRFILES   = $(GO4LOG_S) $(GO4LOG_H)
DISTRFILES         += $(GO4LOG_DISTRFILES)
endif

##### local rules #####

include/%.h: $(GO4LOG_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

ifdef GO4_WIN32
$(GO4LOG_O) : DEFINITIONS += -DCOMP_GO4SYS="\"$(shell cygpath -m $(GO4TOPPATH))\""
else
$(GO4LOG_O) : DEFINITIONS += -DCOMP_GO4SYS="\"$(GO4TOPPATH)\""
endif

clean-bin::
	@rm -f $(GO4LOG_O) $(GO4LOG_DEP) 

