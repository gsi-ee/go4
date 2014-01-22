GO4DABC_NAME        = Go4Dabc

## normally should be like this for every module, but can be specific

GO4DABC_DIR         = $(GO4DABC_NAME)
GO4DABC_LINKDEF     = $(GO4DABC_DIR)/LinkDef.$(HedSuf)


## must be similar for every module

GO4DABC_NOTLIBF = 

GO4DABC_DICT        = $(GO4DABC_DIR)/$(DICT_PREFIX)$(GO4DABC_NAME)
GO4DABC_DH          = $(GO4DABC_DICT).$(HedSuf)
GO4DABC_DS          = $(GO4DABC_DICT).$(SrcSuf)
GO4DABC_DO          = $(GO4DABC_DICT).$(ObjSuf)
GO4DABC_D6          = $(GO4DABC_DICT)$(DICT_R6SUFF)

GO4DABC_H           = $(filter-out $(GO4DABC_NOTLIBF) $(GO4DABC_DH) $(GO4DABC_LINKDEF), $(wildcard $(GO4DABC_DIR)/*.$(HedSuf)))
GO4DABC_S           = $(filter-out $(GO4DABC_NOTLIBF) $(GO4DABC_DS), $(wildcard $(GO4DABC_DIR)/*.$(SrcSuf)))
GO4DABC_O           = $(GO4DABC_S:.$(SrcSuf)=.$(ObjSuf))


GO4DABC_DEP         = $(GO4DABC_O:.$(ObjSuf)=.$(DepSuf))
GO4DABC_DDEP        = $(GO4DABC_DO:.$(ObjSuf)=.$(DepSuf))

ifdef DOPACKAGE
DISTRFILES         += $(GO4DABC_S) $(GO4DABC_H) $(GO4DABC_LINKDEF)
endif

GO4DABC_LIBNAME = $(LIB_PREFIX)Go4Dabc
GO4DABC_LIB     = $(GO4DLLPATH)/$(GO4DABC_LIBNAME).$(DllSuf)


ifeq ($(USEDABC),true)

DABCBASE_LIB = $(wildcard $(DABCLIBPATH)/libDabcBase.$(DllSuf)) 
DABCHTTP_LIB = $(wildcard $(DABCLIBPATH)/libDabcHttp.$(DllSuf)) 
DABCROOT_LIB = $(wildcard $(DABCLIBPATH)/libDabcRoot.$(DllSuf)) 

ifeq ($(DABCROOT_LIB),)

$(warning  DABC compiled without ROOT therefore Go4Sniffer cannot be compiled)

else

# used in the main Makefile

#ALLHDRS +=  $(patsubst $(GO4DABC_DIR)/%.h, include/%.h, $(GO4DABC_H))

LIBDEPENDENC     += $(GO4DABC_DEP) $(GO4DABC_DDEP) 

BUILDGO4LIBS += $(GO4DABC_LIB)

##### local rules #####

#include/%.h: $(GO4DABC_DIR)/%.h
#	@echo "Copy header $@ ..." 
#	@cp -f $< $@

$(GO4DABC_O) $(GO4DABC_DEP): INCLUDES += -I$(DABCINCPATH)

$(GO4DABC_LIB) : LDRPATHS += $(DABCLIBPATH)

$(GO4DABC_DS): $(GO4DABC_DIR)/TGo4Dabc.h  $(GO4DABC_LINKDEF)
	@$(ROOTCINTGO4) $(GO4DABC_LIB) $(GO4DABC_DIR)/TGo4Dabc.h $(GO4DABC_LINKDEF)

$(GO4DABC_LIB): $(GO4DABC_O) $(GO4DABC_DO)
	@$(MakeLibrary) $(GO4DABC_LIBNAME) "$(GO4DABC_O) $(GO4DABC_DO)" $(GO4DLLPATH) "$(GO4DABC_LINKDEF)" "$(DABCROOT_LIB) $(DABCHTTP_LIB) $(DABCBASE_LIB) $(GO4ANBASE_LIB) $(GO4TSKH_LIB) $(THRDMNGR_LIB) $(GO4BASE_LIB) $(GO4FIT_LIB) $(BASIC_LIB_DEP)"

endif

else

endif

clean-bin::
	@rm -f $(GO4DABC_O) $(GO4DABC_DEP)
	@rm -f $(GO4DABC_DH) $(GO4DABC_DS) $(GO4DABC_DO) $(GO4DABC_D6) $(GO4DABC_DDEP) 

clean::

