EXCEPT_NAME        = Go4Exceptions
MODULE_NAME         = $(EXCEPT_NAME)

## normally should be like this for every module, but can be specific

EXCEPT_DIR         = $(GO4SYS)/$(EXCEPT_NAME)

EXCEPT_NOTLIBF     = $(EXCEPT_DIR)/Go4Exceptions.h

## must be similar for every module

EXCEPT_H           = $(filter-out $(EXCEPT_NOTLIBF) $(EXCEPT_DH) $(EXCEPT_LINKDEF), $(wildcard $(EXCEPT_DIR)/*.$(HedSuf)))
EXCEPT_S           = $(filter-out $(EXCEPT_NOTLIBF) $(EXCEPT_DS), $(wildcard $(EXCEPT_DIR)/*.$(SrcSuf)))
EXCEPT_O           = $(EXCEPT_S:.$(SrcSuf)=.$(ObjSuf))

EXCEPT_DEP         =  $(EXCEPT_O:.$(ObjSuf)=.$(DepSuf))

#EXCEPT_LIB         =  $(GO4DLLPATH)/$(EXCEPT_LIBNAME).$(DllSuf).$(VERSSUF)

# used in the main Makefile

ALLHDRS +=  $(patsubst $(EXCEPT_DIR)/%.h, $(GO4SYS)/include/%.h, $(EXCEPT_H) $(EXCEPT_DIR)/Go4Exceptions.h)

LIBDEPENDENC       += $(EXCEPT_DEP)

ifdef DOPACKAGE
EXCEPT_DISTRFILES   = $(EXCEPT_S) $(EXCEPT_H) $(EXCEPT_NOTLIBF)
DISTRFILES         += $(EXCEPT_DISTRFILES)
endif


##### local rules #####

$(GO4SYS)/include/%.h: $(EXCEPT_DIR)/%.h
	@cp -f $< $@

all-$(EXCEPT_NAME): $(EXCEPT_O)

clean-obj-$(EXCEPT_NAME):
		@rm -f $(EXCEPT_O) 

clean-$(EXCEPT_NAME): clean-obj-$(EXCEPT_NAME)
		@rm -f $(EXCEPT_DEP)
