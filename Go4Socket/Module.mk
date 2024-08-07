GO4SOCKET_NAME        = Go4Socket

## normally should be like this for every module, but can be specific

GO4SOCKET_DIR         = $(GO4SOCKET_NAME)

GO4SOCKET_NOTLIBF     =

## must be similar for every module

GO4SOCKET_H           = $(filter-out $(GO4SOCKET_NOTLIBF) $(OLD_DICT), $(wildcard $(GO4SOCKET_DIR)/*.$(HedSuf)))
GO4SOCKET_S           = $(filter-out $(GO4SOCKET_NOTLIBF) $(OLD_DICT), $(wildcard $(GO4SOCKET_DIR)/*.$(SrcSuf)))
GO4SOCKET_O           = $(GO4SOCKET_S:.$(SrcSuf)=.$(ObjSuf))

GO4SOCKET_DEP         =  $(GO4SOCKET_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4SOCKET_DIR)/%.h, include/%.h, $(GO4SOCKET_H))

LIBDEPENDENC       += $(GO4SOCKET_DEP)

ifdef DOPACKAGE
GO4SOCKET_DISTRFILES = $(GO4SOCKET_S) $(GO4SOCKET_H)
DISTRFILES         += $(GO4SOCKET_DISTRFILES)
endif

##### local rules #####

include/%.h: $(GO4SOCKET_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@$(RM) $(GO4SOCKET_O) $(GO4SOCKET_DEP)
