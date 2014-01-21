EVENTSERV_NAME        = Go4EventServer
EVENTSERVPAR_NAME     = Go4EventServerPar

## normally should be like this for every module, but can be specific

EVENTSERV_DIR         = $(EVENTSERV_NAME)

EVENTSERV_NOTLIBF     = $(EVENTSERV_DIR)/random-coll.h $(EVENTSERV_DIR)/random-coll.c 

## must be similar for every module

EVENTSERVPAR_H        = $(wildcard $(EVENTSERV_DIR)/*Parameter.$(HedSuf))
EVENTSERVPAR_S        = $(wildcard $(EVENTSERV_DIR)/*Parameter.$(SrcSuf))
EVENTSERVPAR_O        = $(EVENTSERVPAR_S:.$(SrcSuf)=.$(ObjSuf))

EVENTSERV_H           = $(filter-out $(EVENTSERV_NOTLIBF) $(EVENTSERVPAR_H), $(wildcard $(EVENTSERV_DIR)/*.$(HedSuf)))
EVENTSERV_S           = $(filter-out $(EVENTSERV_NOTLIBF) $(EVENTSERVPAR_S), $(wildcard $(EVENTSERV_DIR)/*.$(SrcSuf)))
EVENTSERV_O           = $(EVENTSERV_S:.$(SrcSuf)=.$(ObjSuf))
EVENTSERV_O          += $(EVENTSERV_DIR)/random-coll.$(ObjSuf)

EVENTSERV_DEP         =  $(EVENTSERV_O:.$(ObjSuf)=.$(DepSuf))

EVENTSERVPAR_DEP      =  $(EVENTSERVPAR_O:.$(ObjSuf)=.$(DepSuf))


# used in the main Makefile

ALLHDRS +=  $(patsubst $(EVENTSERV_DIR)/%.h, include/%.h, $(EVENTSERV_H))
ALLHDRS +=  $(patsubst $(EVENTSERV_DIR)/%.h, include/%.h, $(EVENTSERVPAR_H))

LIBDEPENDENC       += $(EVENTSERV_DEP) $(EVENTSERVPAR_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(EVENTSERV_S) $(EVENTSERV_H) $(EVENTSERV_NOTLIBF)
DISTRFILES         += $(EVENTSERVPAR_S) $(EVENTSERVPAR_H) 
endif


##### local rules #####

include/%.h: $(EVENTSERV_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(EVENTSERV_O) $(EVENTSERV_DEP) 
	@rm -f $(EVENTSERVPAR_O) $(EVENTSERVPAR_DEP)
