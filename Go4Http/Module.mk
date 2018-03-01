GO4HTTP_NAME        = Go4Http

## normally should be like this for every module, but can be specific

GO4HTTP_DIR         = $(GO4HTTP_NAME)
GO4HTTP_LINKDEF     = $(GO4HTTP_DIR)/LinkDef.$(HedSuf)


## must be similar for every module

GO4HTTP_NOTLIBF = 

GO4HTTP_DICT        = $(GO4HTTP_DIR)/$(DICT_PREFIX)$(GO4HTTP_NAME)
GO4HTTP_DH          = $(GO4HTTP_DICT).$(HedSuf)
GO4HTTP_DS          = $(GO4HTTP_DICT).$(SrcSuf)
GO4HTTP_DO          = $(GO4HTTP_DICT).$(ObjSuf)

GO4HTTP_H           = $(filter-out $(GO4HTTP_NOTLIBF) $(GO4HTTP_DH) $(GO4HTTP_LINKDEF) $(OLD_DICT), $(wildcard $(GO4HTTP_DIR)/*.$(HedSuf)))
GO4HTTP_S           = $(filter-out $(GO4HTTP_NOTLIBF) $(GO4HTTP_DS) $(OLD_DICT), $(wildcard $(GO4HTTP_DIR)/*.$(SrcSuf)))
GO4HTTP_O           = $(GO4HTTP_S:.$(SrcSuf)=.$(ObjSuf))


GO4HTTP_DEP         = $(GO4HTTP_O:.$(ObjSuf)=.$(DepSuf))
GO4HTTP_DDEP        = $(GO4HTTP_DO:.$(ObjSuf)=.$(DepSuf))

ifdef DOPACKAGE
DISTRFILES         += $(GO4HTTP_S) $(GO4HTTP_H) $(GO4HTTP_LINKDEF)
endif

GO4HTTP_LIBNAME = $(LIB_PREFIX)Go4Http
GO4HTTP_LIB     = $(GO4DLLPATH)/$(GO4HTTP_LIBNAME).$(DllSuf)

ifeq ($(USEHTTPSERVER),true)

ROOTHTTP_LIB = $(ROOTLIBPATH)/libRHTTP.$(DllSuf) 

GO4HTTP_LIBSET  = $(LIBS_FULLSET) -lRHTTP

ifeq ($(USEHTTPSNIFF),true)
ROOTHTTPSNIFF_LIB = $(ROOTLIBPATH)/libRHTTPSniff.$(DllSuf)
GO4HTTP_LIBSET += -lRHTTPSniff
endif

# used in the main Makefile

#ALLHDRS +=  $(patsubst $(GO4HTTP_DIR)/%.h, include/%.h, $(GO4HTTP_H))

LIBDEPENDENC += $(GO4HTTP_DEP) $(GO4HTTP_DDEP) 

BUILDGO4LIBS += $(GO4HTTP_LIB)

##### local rules #####

#include/%.h: $(GO4HTTP_DIR)/%.h
#	@echo "Copy header $@ ..." 
#	@cp -f $< $@

GO4HTTP_DEPLIB = $(ROOTHTTP_LIB) $(ROOTHTTPSNIFF_LIB) $(ANAL_LIB_DEP)

$(GO4HTTP_LIB):  MAKELIB_SET = $(GO4HTTP_LIBSET)

#required for static constants from analysis object manager
ifeq ($(GO4_OS),Win32)
$(GO4HTTP_O) : DEFINITIONS += -DGO4_EXPORT
endif

$(GO4HTTP_DS): $(GO4HTTP_H) $(GO4HTTP_LINKDEF) $(GO4SYS)/build/dummy.d
	@$(ROOTCINTGO4) $(GO4HTTP_LIB) $(GO4HTTP_H) $(GO4HTTP_LINKDEF)

$(GO4HTTP_LIB): $(GO4HTTP_O) $(GO4HTTP_DO) $(GO4HTTP_DEPLIB)
	@$(MakeLibrary) $(GO4HTTP_LIBNAME) "$(GO4HTTP_O) $(GO4HTTP_DO)" $(GO4DLLPATH) "$(GO4HTTP_LINKDEF)" "$(GO4HTTP_DEPLIB)" $(GO4HTTP_DS) $(GO4HTTP_DIR)/TGo4Sniffer.h

endif

clean-bin::
	@rm -f $(GO4HTTP_O) $(GO4HTTP_DEP)
	@rm -f $(GO4HTTP_DH) $(GO4HTTP_DS) $(GO4HTTP_DO) $(GO4HTTP_DDEP) 

clean::

