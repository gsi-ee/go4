CMDTASKHANDL_NAME        = Go4CommandsTaskHandler

## normally should be like this for every module, but can be specific

CMDTASKHANDL_DIR         = $(CMDTASKHANDL_NAME)

CMDTASKHANDL_NOTLIBF     =

## must be similar for every module

CMDTASKHANDL_H           = $(filter-out $(CMDTASKHANDL_NOTLIBF) $(OLD_DICT), $(wildcard $(CMDTASKHANDL_DIR)/*.$(HedSuf)))
CMDTASKHANDL_S           = $(filter-out $(CMDTASKHANDL_NOTLIBF) $(OLD_DICT), $(wildcard $(CMDTASKHANDL_DIR)/*.$(SrcSuf)))
CMDTASKHANDL_O           = $(CMDTASKHANDL_S:.$(SrcSuf)=.$(ObjSuf))

CMDTASKHANDL_DEP         =  $(CMDTASKHANDL_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(CMDTASKHANDL_DIR)/%.h, include/%.h, $(CMDTASKHANDL_H))

LIBDEPENDENC       += $(CMDTASKHANDL_DEP)

ifdef DOPACKAGE
CMDTASKHANDL_DISTRFILES = $(CMDTASKHANDL_S) $(CMDTASKHANDL_H) $(CMDTASKHANDL_NOTLIBF)
DISTRFILES         += $(CMDTASKHANDL_DISTRFILES)
endif

##### local rules #####

include/%.h: $(CMDTASKHANDL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(CMDTASKHANDL_O) $(CMDTASKHANDL_DEP)

