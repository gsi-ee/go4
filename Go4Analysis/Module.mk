GO4ANAL_NAME        = Go4Analysis

## normally should be like this for every module, but can be specific

GO4ANAL_DIR         = $(GO4ANAL_NAME)

VERSION_FILENAME    = TGo4Version

GO4ANAL_NOTLIBF     = $(GO4ANAL_DIR)/TGo4Analysis.h

## must be similar for every module

VERSION_H           = $(GO4ANAL_DIR)/$(VERSION_FILENAME).$(HedSuf)
VERSION_S           = $(GO4ANAL_DIR)/$(VERSION_FILENAME).$(SrcSuf)
VERSION_O           = $(GO4ANAL_DIR)/$(VERSION_FILENAME).$(ObjSuf)

GO4ANALEXE_S        = $(GO4ANAL_DIR)/MainUserAnalysis.$(SrcSuf)
GO4ANALEXE_O        = $(GO4ANAL_DIR)/MainUserAnalysis.$(ObjSuf)

GO4ANALEXE          = bin/go4analysis$(ExeSuf)

GO4ANAL_H          := $(filter-out $(GO4ANAL_NOTLIBF) $(VERSION_H) $(OLD_DICT), $(wildcard $(GO4ANAL_DIR)/*.$(HedSuf)))
GO4ANAL_S          := $(filter-out $(GO4ANAL_NOTLIBF) $(GO4ANALEXE_S) $(VERSION_S) $(OLD_DICT), $(wildcard $(GO4ANAL_DIR)/*.$(SrcSuf)))
GO4ANAL_O          := $(GO4ANAL_S:.$(SrcSuf)=.$(ObjSuf))

GO4ANAL_DEP         = $(GO4ANAL_O:.$(ObjSuf)=.$(DepSuf))

VERSION_DEP         = $(VERSION_O:.$(ObjSuf)=.$(DepSuf))
GO4ANALEXE_DEP      = $(GO4ANALEXE_O:.$(ObjSuf)=.$(DepSuf))


# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4ANAL_DIR)/%.h, include/%.h, $(GO4ANAL_H) $(VERSION_H) $(GO4ANAL_DIR)/TGo4Analysis.h)

LIBDEPENDENC       += $(GO4ANAL_DEP) $(VERSION_DEP) $(GO4ANALEXE_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4ANAL_S) $(GO4ANAL_H) 
DISTRFILES         += $(VERSION_H) $(VERSION_S) 
DISTRFILES         += $(GO4ANALEXE_S) $(GO4ANAL_NOTLIBF)
endif

##### local rules #####

include/%.h: $(GO4ANAL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(GO4ANALEXE): LDRPATHS += .

ifeq ($(USEDABC),true)
$(GO4ANALEXE_O) : CXXFLAGS += -DWITH_DABC
endif

ifeq ($(USEHTTPSERVER),true)
$(GO4ANALEXE_O) : CXXFLAGS += -DWITH_HTTP
endif

$(GO4ANALEXE): $(GO4ANALEXE_O) $(GO4AN_LIB)
	$(LD) $(LDFLAGS) $(GO4ANALEXE_O) $(LIBS_FULLSET) $(OutPutOpt) $(GO4ANALEXE)
	@echo "$@  done"

all:: $(GO4ANALEXE)

clean-bin::
	@rm -f $(GO4ANALEXE_O) $(GO4ANALEXE_DEP)
	@rm -f $(GO4ANAL_O) $(GO4ANAL_DEP) $(VERSION_O) $(VERSION_DEP)

clean::
	@rm -f $(VERSION_O) $(GO4ANALEXE)
