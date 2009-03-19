QT3ROOT_DIR       = $(GO4SYS)/qt3/Go4QtRoot

QT3ROOT_LIBNAME =  $(LIB_PREFIX)Go4Qt3Root

QT3ROOT_VERSUF = 3.5.0

QT3ROOT_S       =  $(QT3ROOT_DIR)/qrootapplication.cpp \
                   $(QT3ROOT_DIR)/tqrootguifactory.cpp \
                   $(QT3ROOT_DIR)/tqcanvasimp.cpp \
                   $(QT3ROOT_DIR)/tqcanvasmenu.cpp \
                   $(QT3ROOT_DIR)/tqrootcanvas.cpp \
                   $(QT3ROOT_DIR)/tqrootwindow.cpp \
                   $(QT3ROOT_DIR)/tqrootdialog.cpp \
                   $(QT3ROOT_DIR)/tqapplication.cpp \
                   $(QT3ROOT_DIR)/qtframe.cpp

QT3ROOT_H       =  $(QT3ROOT_S:.cpp=.$(HedSuf))
QT3ROOT_O       =  $(QT3ROOT_S:.cpp=.$(ObjSuf))
QT3ROOT_DEP     =  $(QT3ROOT_O:.$(ObjSuf)=.$(DepSuf))

QT3ROOT_MOCS    =  $(QT3ROOT_DIR)/moc_tqrootcanvas.cpp \
                   $(QT3ROOT_DIR)/moc_tqrootwindow.cpp \
                   $(QT3ROOT_DIR)/moc_tqcanvasmenu.cpp \
                   $(QT3ROOT_DIR)/moc_tqrootdialog.cpp \
                   $(QT3ROOT_DIR)/moc_qrootapplication.cpp \
                   $(QT3ROOT_DIR)/moc_qtframe.cpp

QT3ROOT_MOCO    = $(QT3ROOT_MOCS:.cpp=.$(ObjSuf))

QT3ROOT_LIB     =  $(GO4DLLPATH)/$(QT3ROOT_LIBNAME).$(DllSuf).$(QT3ROOT_VERSUF)

QT3ROOT_PUBH    = $(patsubst $(QT3ROOT_DIR)/%.h, $(GO4SYS)/include/%.h, $(QT3ROOT_H))

# used in the main Makefile

GO4QT3DEP   += $(QT3ROOT_DEP)

GO4QT3HEADS += $(QT3ROOT_PUBH)

ifdef DOPACKAGE
DISTRFILES         += $(QT3ROOT_H) $(QT3ROOT_S)
DISTRFILES         += $(QT3ROOT_DIR)/lockguard.h
DISTRFILES         += $(QT3ROOT_DIR)/CHANGELOG.txt
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(QT3ROOT_DIR)/%.h
	@cp -f $< $@

$(QT3ROOT_LIB):    $(QT3ROOT_O) $(QT3ROOT_MOCO)
	@$(MakeQLib) $(QT3ROOT_VERSUF) $(QT3ROOT_LIBNAME) "$(QT3ROOT_O) $(QT3ROOT_MOCO)" $(GO4DLLPATH)

qt3-interface: $(QT3ROOT_LIB)

clean-qt3-interface:
	@rm -f $(QT3ROOT_O) $(QT3ROOT_MOCO)
	@$(CleanQLib) $(QT3ROOT_VERSUF) $(QT3ROOT_LIBNAME) $(GO4DLLPATH)
	@rm -f $(QT3ROOT_MOCS) $(QT3ROOT_DEP) $(QT3ROOT_PUBH)

$(QT3ROOT_MOCS): $(QT3ROOT_DIR)/moc_%.cpp: $(QT3ROOT_DIR)/%.h
	$(MOC) $< -o $@
