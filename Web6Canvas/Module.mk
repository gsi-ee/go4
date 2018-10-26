WEB6CANV_NAME     = Web6Canvas

## normally should be like this for every module, but can be specific

WEB6CANV_DIR         = $(WEB6CANV_NAME)

WEB6CANV_NOTLIBF     =

## must be similar for every module

WEB6CANV_LINKDEF     = $(WEB6CANV_DIR)/LinkDef.h
WEB6CANV_DICT        = $(WEB6CANV_DIR)/$(DICT_PREFIX)Web6Canvas
WEB6CANV_DH          = $(WEB6CANV_DICT).$(HedSuf)
WEB6CANV_DS          = $(WEB6CANV_DICT).$(SrcSuf)
WEB6CANV_DO          = $(WEB6CANV_DICT).$(ObjSuf)
WEB6CANV_DDEP       =  $(WEB6CANV_DICT).$(DepSuf)

WEB6CANV_H           = $(filter-out $(WEB6CANV_NOTLIBF) $(WEB6CANV_DH) $(WEB6CANV_LINKDEF), $(wildcard $(WEB6CANV_DIR)/*.$(HedSuf)))
WEB6CANV_S           = $(filter-out $(WEB6CANV_NOTLIBF) $(WEB6CANV_DS), $(wildcard $(WEB6CANV_DIR)/*.$(SrcSuf)))
WEB6CANV_O           = $(WEB6CANV_S:.$(SrcSuf)=.$(ObjSuf))

WEB6CANV_DEP         =  $(WEB6CANV_O:.$(ObjSuf)=.$(DepSuf))

ifdef DOPACKAGE
DISTRFILES         += $(WEB6CANV_S) $(WEB6CANV_H)
endif

ifdef withweb

WEB6CANV_LIBNAME = $(LIB_PREFIX)Web6Canv
WEB6CANV_LIB     = $(GO4DLLPATH)/$(WEB6CANV_LIBNAME).$(DllSuf)

BUILDGO4LIBS += $(WEB6CANV_LIB)

# used in the main Makefile

ALLHDRS +=  $(patsubst $(WEB6CANV_DIR)/%.h, include/%.h, $(WEB6CANV_H))

LIBDEPENDENC       += $(WEB6CANV_DEP) $(WEB6CANV_DDEP)


$(WEB6CANV_DS): $(WEB6CANV_H) $(WEB6CANV_LINKDEF) $(GO4SYS)/build/dummy.d
	@$(ROOTCINTGO4) $(WEB6CANV_LIB) $(WEB6CANV_H) $(WEB6CANV_LINKDEF)

$(WEB6CANV_LIB): $(WEB6CANV_O) $(WEB6CANV_DO) $(GO4BGUI_DEPLIB)
	@$(MakeLibrary) $(WEB6CANV_LIBNAME) "$(WEB6CANV_O) $(WEB6CANV_DO)" $(GO4DLLPATH) "$(WEB6CANV_LINKDEF)" "$(GO4BGUI_DEPLIB)" $(WEB6CANV_DS) "$(WEB6CANV_H)"

clean-mainlibs::
	@$(CleanLib) $(WEB6CANV_LIBNAME) $(GO4DLLPATH)

clean-bin::
	@rm -f $(WEB6CANV_DICT).*  
	@rm -f $(WEB6CANV_O) $(WEB6CANV_DEP)


##### local rules #####

include/%.h: $(WEB6CANV_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

endif

	