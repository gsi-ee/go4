QT4ROOT_DIR       = $(GO4SYS)/qt4/Go4QtRoot

QT4ROOT_LIBNAME =  $(LIB_PREFIX)Go4Qt4Root

QT4ROOT_S       =  $(QT4ROOT_DIR)/QRootApplication.cpp \
                   $(QT4ROOT_DIR)/QRootCanvas.cpp \
                   $(QT4ROOT_DIR)/QRootWindow.cpp \
                   $(QT4ROOT_DIR)/QRootDialog.cpp

QT4ROOT_H       =  $(QT4ROOT_S:.cpp=.$(HedSuf))
QT4ROOT_O       =  $(QT4ROOT_S:.cpp=.$(ObjSuf))
QT4ROOT_DEP     =  $(QT4ROOT_O:.$(ObjSuf)=.$(DepSuf))

QT4ROOT_MOCS    =  $(QT4ROOT_DIR)/moc_QRootCanvas.cpp \
                   $(QT4ROOT_DIR)/moc_QRootWindow.cpp \
                   $(QT4ROOT_DIR)/moc_QRootDialog.cpp \
                   $(QT4ROOT_DIR)/moc_QRootApplication.cpp

QT4ROOT_MOCO    = $(QT4ROOT_MOCS:.cpp=.$(ObjSuf))

QT4ROOT_LIB     =  $(GO4DLLPATH)/$(QT4ROOT_LIBNAME).$(DllSuf)

QT4ROOT_PUBH    = $(patsubst $(QT4ROOT_DIR)/%.h, $(GO4SYS)/include/%.h, $(QT4ROOT_H))

# used in the main Makefile

GO4QT4DEP   += $(QT4ROOT_DEP)

GO4QT4HEADS += $(QT4ROOT_PUBH)

ifdef DOPACKAGE
DISTRFILES         += $(QT4ROOT_H) $(QT4ROOT_S)
DISTRFILES         += $(QT4ROOT_DIR)/CHANGELOG.txt
DISTRFILES         += $(QT4ROOT_DIR)/Module.mk
endif

##### local rules #####

ifdef GO4_QT4
$(GO4SYS)/include/%.h: $(QT4ROOT_DIR)/%.h
	@cp -f $< $@
endif


$(QT4ROOT_LIB):    $(QT4ROOT_O) $(QT4ROOT_MOCO)
	@$(MakeLibrary) $(QT4ROOT_LIBNAME) "$(QT4ROOT_O) $(QT4ROOT_MOCO)" $(GO4DLLPATH)

qt4-interface: $(QT4ROOT_LIB)

clean-qt4-interface:
	@rm -f $(QT4ROOT_O) $(QT4ROOT_MOCO)
	@$(CleanLib) $(QT4ROOT_LIBNAME) $(GO4DLLPATH)
	@rm -f $(QT4ROOT_MOCS) $(QT4ROOT_DEP) $(QT4ROOT_PUBH)

$(QT4ROOT_MOCS): $(QT4ROOT_DIR)/moc_%.cpp: $(QT4ROOT_DIR)/%.h
	$(MOC) $< -o $@
