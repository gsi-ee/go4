LOCKGRD_NAME        = Go4LockGuard

## normally should be like this for every module, but can be specific

LOCKGRD_DIR         = $(LOCKGRD_NAME)

LOCKGRD_NOTLIBF     =

## must be similar for every module

LOCKGRD_H           = $(filter-out $(LOCKGRD_NOTLIBF) $(OLD_DICT), $(wildcard $(LOCKGRD_DIR)/*.$(HedSuf)))
LOCKGRD_S           = $(filter-out $(LOCKGRD_NOTLIBF) $(OLD_DICT), $(wildcard $(LOCKGRD_DIR)/*.$(SrcSuf)))
LOCKGRD_O           = $(LOCKGRD_S:.$(SrcSuf)=.$(ObjSuf))

LOCKGRD_DEP         =  $(LOCKGRD_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(LOCKGRD_DIR)/%.h, include/%.h, $(LOCKGRD_H))

LIBDEPENDENC       += $(LOCKGRD_DEP)

ifdef DOPACKAGE
LOCKGRD_DISTRFILES  = $(LOCKGRD_S) $(LOCKGRD_H) 
DISTRFILES         += $(LOCKGRD_DISTRFILES)
endif

##### local rules #####

include/%.h: $(LOCKGRD_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(LOCKGRD_O) $(LOCKGRD_DEP) 
