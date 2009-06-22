GO4SOCKET_NAME        = Go4Socket

## normally should be like this for every module, but can be specific

GO4SOCKET_DIR         = $(GO4SYS)/$(GO4SOCKET_NAME)

GO4SOCKET_NOTLIBF     =

## must be similar for every module

GO4SOCKET_H           = $(filter-out $(GO4SOCKET_NOTLIBF) $(GO4SOCKET_DH) $(GO4SOCKET_LINKDEF), $(wildcard $(GO4SOCKET_DIR)/*.$(HedSuf)))
GO4SOCKET_S           = $(filter-out $(GO4SOCKET_NOTLIBF) $(GO4SOCKET_DS), $(wildcard $(GO4SOCKET_DIR)/*.$(SrcSuf)))
GO4SOCKET_O           = $(GO4SOCKET_S:.$(SrcSuf)=.$(ObjSuf))

GO4SOCKET_DEP         =  $(GO4SOCKET_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4SOCKET_DIR)/%.h, $(GO4SYS)/include/%.h, $(GO4SOCKET_H))

LIBDEPENDENC       += $(GO4SOCKET_DEP)

ifdef DOPACKAGE
GO4SOCKET_DISTRFILES = $(GO4SOCKET_S) $(GO4SOCKET_H)
DISTRFILES         += $(GO4SOCKET_DISTRFILES)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(GO4SOCKET_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean::
	@rm -f $(GO4SOCKET_O)
	@rm -f $(GO4SOCKET_DEP)
