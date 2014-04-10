ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifeq ($(USEDABC),true)


ifdef GO4PACKAGE
EXAMPDABC_DIR         = Go4ExampleDabc
else
EXAMPDABC_DIR         = .
endif

EXAMPDABC_LINKDEF     = $(EXAMPDABC_DIR)/XXXLinkDef.$(HedSuf)
EXAMPDABC_LIBNAME     = $(GO4_USERLIBNAME)

## must be similar for every module

EXAMPDABC_DICT        = $(EXAMPDABC_DIR)/$(DICT_PREFIX)XXX
EXAMPDABC_DH          = $(EXAMPDABC_DICT).$(HedSuf)
EXAMPDABC_DS          = $(EXAMPDABC_DICT).$(SrcSuf)
EXAMPDABC_DO          = $(EXAMPDABC_DICT).$(ObjSuf)

EXAMPDABC_H           = $(filter-out $(EXAMPDABC_DH) $(EXAMPDABC_LINKDEF), $(wildcard $(EXAMPDABC_DIR)/*.$(HedSuf)))
EXAMPDABC_S           = $(filter-out $(EXAMPDABC_DS), $(wildcard $(EXAMPDABC_DIR)/*.$(SrcSuf)))
EXAMPDABC_O           = $(EXAMPDABC_S:.$(SrcSuf)=.$(ObjSuf))

EXAMPDABC_DEP         = $(EXAMPDABC_O:.$(ObjSuf)=.$(DepSuf))
EXAMPDABC_DDEP        = $(EXAMPDABC_DO:.$(ObjSuf)=.$(DepSuf))

EXAMPDABC_LIB         = $(EXAMPDABC_DIR)/$(EXAMPDABC_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMPDABC_DEP) $(EXAMPDABC_DDEP) $(EXAMPDABC_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMPDABC_S) $(EXAMPDABC_H) $(EXAMPDABC_LINKDEF)
DISTRFILES         += $(EXAMPDABC_DIR)/Readme.txt
endif

$(EXAMPDABC_O) $(EXAMPDABC_DEP) $(EXAMPDABC_DO) $(EXAMPDABC_DDEP)  : INCLUDES += -I$(DABCINCPATH)

$(EXAMPDABC_LIB) : LDRPATHS += $(DABCLIBPATH)
$(EXAMPDABC_LIB) : LDFLAGS += -L$(DABCLIBPATH) -lDabcBase -lDabcMbs

##### local rules #####

all::  $(EXAMPDABC_LIB)


$(EXAMPDABC_LIB):   $(EXAMPDABC_O) $(EXAMPDABC_DO) $(ANAL_LIB_DEP)
	@$(MakeLibrary) $(EXAMPDABC_LIBNAME) "$(EXAMPDABC_O) $(EXAMPDABC_DO)" $(EXAMPDABC_DIR) $(EXAMPDABC_LINKDEF) "$(ANAL_LIB_DEP)" $(EXAMPDABC_DS) "$(EXAMPDABC_H)"

$(EXAMPDABC_DS): $(EXAMPDABC_H) $(EXAMPDABC_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMPDABC_LIB) $(EXAMPDABC_H) $(EXAMPDABC_LINKDEF)



clean-bin::
	@rm -f $(EXAMPDABC_O) $(EXAMPDABC_DO)
	@rm -f $(EXAMPDABC_DEP) $(EXAMPDABC_DDEP) $(EXAMPDABC_DS) $(EXAMPDABC_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMPDABC_LIBNAME) $(EXAMPDABC_DIR)

endif

include $(GO4SYS)/Makefile.rules
