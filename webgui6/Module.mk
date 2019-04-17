ifdef withweb

## normally should be like this for every module, but can be specific

WEBGUI6_DIR         = webgui6

WEBGUI6_LINKDEF     = $(WEBGUI6_DIR)/LinkDef.h
WEBGUI6_LIBNAME     = $(LIB_PREFIX)WebGui6Full

## must be similar for every module

WEBGUI6_DICT        = $(WEBGUI6_DIR)/$(DICT_PREFIX)WebGui6
WEBGUI6_DH          = $(WEBGUI6_DICT).$(HedSuf)
WEBGUI6_DS          = $(WEBGUI6_DICT).$(SrcSuf)
WEBGUI6_DO          = $(WEBGUI6_DICT).$(ObjSuf)

WEBGUI6_H           = $(filter-out $(WEBGUI6_DH) $(WEBGUI6_LINKDEF), $(wildcard $(WEBGUI6_DIR)/*.$(HedSuf)))
WEBGUI6_S           = $(filter-out $(WEBGUI6_DS), $(wildcard $(WEBGUI6_DIR)/*.$(SrcSuf)))
WEBGUI6_O           = $(WEBGUI6_S:.$(SrcSuf)=.$(ObjSuf))

WEBGUI6_DEP         = $(WEBGUI6_O:.$(ObjSuf)=.$(DepSuf))
WEBGUI6_DDEP        = $(WEBGUI6_DO:.$(ObjSuf)=.$(DepSuf))

WEBGUI6_LIB         = $(GO4DLLPATH)/$(WEBGUI6_LIBNAME).$(DllSuf)

WEBGUI6_DEPLIB     = $(ROOTLIBPATH)/libGpad.$(DllSuf) \
                     $(ROOTLIBPATH)/libRHTTP.$(DllSuf) \
                     $(ROOTLIBPATH)/libWebGui6.$(DllSuf)

BUILDGO4LIBS += $(WEBGUI6_LIB) 

$(WEBGUI6_LIB):   MAKELIB_SET = -L$(ROOTLIBPATH) -lWebGui6

# used in the main Makefile

ALLHDRS +=  $(patsubst $(WEBGUI6_DIR)/%.h, include/%.h, $(WEBGUI6_H))

LIBDEPENDENC    += $(WEBGUI6_DEP) $(WEBGUI6_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(WEBGUI6_S) $(WEBGUI6_H) $(WEBGUI6_LINKDEF)
endif

##### local rules #####

include/%.h: $(WEBGUI6_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@


$(WEBGUI6_DS): $(WEBGUI6_H) $(WEBGUI6_LINKDEF)
	@$(ROOTCINTGO4) $(WEBGUI6_LIB) $(WEBGUI6_H) $(WEBGUI6_LINKDEF)

$(WEBGUI6_LIB):  $(WEBGUI6_O) $(WEBGUI6_DO) 
	@$(MakeLibrary) $(WEBGUI6_LIBNAME) "$(WEBGUI6_O) $(WEBGUI6_DO)" $(GO4DLLPATH) $(WEBGUI6_LINKDEF) "$(WEBGUI6_DEPLIB)" $(WEBGUI6_DS) "$(WEBGUI6_H)"



#$(GO4ANBASE_DS): $(GO4ANBASE_H) $(GO4ANBASE_LINKDEF) $(GO4SYS)/build/dummy.d
#	@$(ROOTCINTGO4) $(GO4ANBASE_LIB) $(GO4ANBASE_H) $(GO4ANBASE_LINKDEF)

#$(GO4ANBASE_LIB): $(GO4ANBASE_O) $(GO4ANBASE_DEPLIB) $(WEBGUI6_DEPLIB)
#	@$(MakeLibrary) $(GO4ANBASE_LIBNAME) "$(GO4ANBASE_O)" $(GO4DLLPATH) "$(GO4ANBASE_LINKDEF)" "$(GO4ANBASE_DEPLIB)" $(GO4ANBASE_DS) "$(GO4ANBASE_H)"



clean-bin::
	@rm -f $(WEBGUI6_O) $(WEBGUI6_DO)
	@rm -f $(WEBGUI6_DEP) $(WEBGUI6_DDEP) $(WEBGUI6_DS) $(WEBGUI6_DH)

clean:: clean-bin
	@$(CleanLib) $(WEBGUI6_LIBNAME) $(WEBGUI6_DIR)

endif
