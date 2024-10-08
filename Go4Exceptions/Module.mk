EXCEPT_NAME        = Go4Exceptions

## normally should be like this for every module, but can be specific

EXCEPT_DIR         = $(EXCEPT_NAME)

EXCEPT_NOTLIBF     = $(EXCEPT_DIR)/Go4Exceptions.h

## must be similar for every module

EXCEPT_H           = $(filter-out $(EXCEPT_NOTLIBF) $(OLD_DICT), $(wildcard $(EXCEPT_DIR)/*.$(HedSuf)))
EXCEPT_S           = $(filter-out $(EXCEPT_NOTLIBF) $(OLD_DICT), $(wildcard $(EXCEPT_DIR)/*.$(SrcSuf)))
EXCEPT_O           = $(EXCEPT_S:.$(SrcSuf)=.$(ObjSuf))

EXCEPT_DEP         =  $(EXCEPT_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(EXCEPT_DIR)/%.h, include/%.h, $(EXCEPT_H) $(EXCEPT_DIR)/Go4Exceptions.h)

LIBDEPENDENC       += $(EXCEPT_DEP)

ifdef DOPACKAGE
EXCEPT_DISTRFILES   = $(EXCEPT_S) $(EXCEPT_H) $(EXCEPT_NOTLIBF)
DISTRFILES         += $(EXCEPT_DISTRFILES)
endif


##### local rules #####

include/%.h: $(EXCEPT_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@$(RM) $(EXCEPT_O) $(EXCEPT_DEP)
