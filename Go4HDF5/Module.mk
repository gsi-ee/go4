ifdef USEHDF5

HDF_NAME        = Go4HDF5


## normally should be like this for every module, but can be specific

HDF_DIR         = $(HDF_NAME)

HDF_NOTLIBF     =

## must be similar for every module

HDFPAR_H        = $(wildcard $(HDF_DIR)/*Parameter.$(HedSuf))
HDFPAR_S        = $(wildcard $(HDF_DIR)/*Parameter.$(SrcSuf))
HDFPAR_O        = $(HDFPAR_S:.$(SrcSuf)=.$(ObjSuf))

HDF_H           = $(filter-out $(HDF_NOTLIBF) $(HDFPAR_H) $(OLD_DICT), $(wildcard $(HDF_DIR)/*.$(HedSuf)))
HDF_S           = $(filter-out $(HDF_NOTLIBF) $(HDFPAR_S) $(OLD_DICT), $(wildcard $(HDF_DIR)/*.$(SrcSuf)))
HDF_O           = $(HDF_S:.$(SrcSuf)=.$(ObjSuf))

HDF_DEP         =  $(HDF_O:.$(ObjSuf)=.$(DepSuf))

HDFPAR_DEP      =  $(HDFPAR_O:.$(ObjSuf)=.$(DepSuf))


# used in the main Makefile

ALLHDRS +=  $(patsubst $(HDF_DIR)/%.h, include/%.h, $(HDF_H))
ALLHDRS +=  $(patsubst $(HDF_DIR)/%.h, include/%.h, $(HDFPAR_H))

LIBDEPENDENC       += $(HDF_DEP) $(HDFPAR_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(HDF_S) $(HDF_H) $(HDF_NOTLIBF)
DISTRFILES         += $(HDFPAR_S) $(HDFPAR_H) 
endif


##### local rules #####

include/%.h: $(HDF_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(HDF_O) $(HDF_DEP) 
	@rm -f $(HDFPAR_O) $(HDFPAR_DEP)
	
endif	
