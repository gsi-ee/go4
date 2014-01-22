ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
EXAMPMESH_DIR         = Go4ExampleMesh
else
EXAMPMESH_DIR         = .
endif

EXAMPMESH_LINKDEF     = $(EXAMPMESH_DIR)/MeshLinkDef.$(HedSuf)
EXAMPMESH_LIBNAME     = $(GO4_USERLIBNAME)

## must be similar for every module

EXAMPMESH_DICT        = $(EXAMPMESH_DIR)/$(DICT_PREFIX)Mesh
EXAMPMESH_DH          = $(EXAMPMESH_DICT).$(HedSuf)
EXAMPMESH_DS          = $(EXAMPMESH_DICT).$(SrcSuf)
EXAMPMESH_DO          = $(EXAMPMESH_DICT).$(ObjSuf)
EXAMPMESH_D6          = $(EXAMPMESH_DICT)$(DICT_R6SUFF)

EXAMPMESH_H           = $(filter-out $(EXAMPMESH_DH) $(EXAMPMESH_LINKDEF), $(wildcard $(EXAMPMESH_DIR)/*.$(HedSuf)))
EXAMPMESH_S           = $(filter-out $(EXAMPMESH_DS), $(wildcard $(EXAMPMESH_DIR)/*.$(SrcSuf)))
EXAMPMESH_O           = $(EXAMPMESH_S:.$(SrcSuf)=.$(ObjSuf))

EXAMPMESH_DEP         =  $(EXAMPMESH_O:.$(ObjSuf)=.$(DepSuf))
EXAMPMESH_DDEP        =  $(EXAMPMESH_DO:.$(ObjSuf)=.$(DepSuf))

EXAMPMESH_LIB         =  $(EXAMPMESH_DIR)/$(EXAMPMESH_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMPMESH_DEP) $(EXAMPMESH_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMPMESH_S) $(EXAMPMESH_H) $(EXAMPMESH_LINKDEF)
endif

##### local rules #####

all::     $(EXAMPMESH_LIB)

$(EXAMPMESH_LIB):   $(EXAMPMESH_O) $(EXAMPMESH_DO)
	@$(MakeLibrary) $(EXAMPMESH_LIBNAME) "$(EXAMPMESH_O) $(EXAMPMESH_DO)" $(EXAMPMESH_DIR) $(EXAMPMESH_LINKDEF) "$(ANAL_LIB_DEP)"

$(EXAMPMESH_DS): $(EXAMPMESH_H)  $(EXAMPMESH_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMPMESH_LIB) $(EXAMPMESH_H) $(EXAMPMESH_LINKDEF)

clean-bin::
	@rm -f $(EXAMPMESH_O) $(EXAMPMESH_DO) $(EXAMPMESH_D6)
	@rm -f $(EXAMPMESH_DEP) $(EXAMPMESH_DDEP) $(EXAMPMESH_DS) $(EXAMPMESH_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMPMESH_LIBNAME) $(EXAMPMESH_DIR)

include $(GO4SYS)/Makefile.rules
