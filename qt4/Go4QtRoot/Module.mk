QT4ROOT_DIR       = $(GO4SYS)/qt4/Go4QtRoot

QT4ROOT_LIBNAME =  $(LIB_PREFIX)Go4Qt4Root

QT4ROOT_VERSUF = 4.0.0

QT4ROOT_S       =  $(QT4ROOT_DIR)/qrootapplication.cpp \
                   $(QT4ROOT_DIR)/tqrootguifactory.cpp \
                   $(QT4ROOT_DIR)/tqcanvasimp.cpp \
                   $(QT4ROOT_DIR)/tqcanvasmenu.cpp \
                   $(QT4ROOT_DIR)/tqrootcanvas.cpp \
                   $(QT4ROOT_DIR)/tqrootwindow.cpp \
                   $(QT4ROOT_DIR)/tqrootdialog.cpp \
                   $(QT4ROOT_DIR)/tqapplication.cpp \
                   $(QT4ROOT_DIR)/qtframe.cpp

QT4ROOT_H       =  $(QT4ROOT_S:.cpp=.$(HedSuf))
QT4ROOT_O       =  $(QT4ROOT_S:.cpp=.$(ObjSuf))
QT4ROOT_DEP     =  $(QT4ROOT_O:.$(ObjSuf)=.$(DepSuf))

QT4ROOT_MOCS    =  $(QT4ROOT_DIR)/moc_tqrootcanvas.cpp \
                   $(QT4ROOT_DIR)/moc_tqrootwindow.cpp \
                   $(QT4ROOT_DIR)/moc_tqcanvasmenu.cpp \
                   $(QT4ROOT_DIR)/moc_tqrootdialog.cpp \
                   $(QT4ROOT_DIR)/moc_qrootapplication.cpp \
                   $(QT4ROOT_DIR)/moc_qtframe.cpp

QT4ROOT_MOCO    = $(QT4ROOT_MOCS:.cpp=.$(ObjSuf))

QT4ROOT_LIB     =  $(GO4DLLPATH)/$(QT4ROOT_LIBNAME).$(DllSuf).$(QT4ROOT_VERSUF)

QT4ROOT_PUBH    = $(patsubst $(QT4ROOT_DIR)/%.h, $(GO4SYS)/include/%.h, $(QT4ROOT_H))

# used in the main Makefile

GO4QT4DEP   += $(QT4ROOT_DEP)

GO4QT4HEADS += $(QT4ROOT_PUBH)

ifdef DOPACKAGE
DISTRFILES         += $(QT4ROOT_H) $(QT4ROOT_S)
DISTRFILES         += $(QT4ROOT_DIR)/lockguard.h
DISTRFILES         += $(QT4ROOT_DIR)/CHANGELOG.txt
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(QT4ROOT_DIR)/%.h
	@cp -f $< $@

$(QT4ROOT_LIB):    $(QT4ROOT_O) $(QT4ROOT_MOCO)
	@$(MakeQLib) $(QT4ROOT_VERSUF) $(QT4ROOT_LIBNAME) "$(QT4ROOT_O) $(QT4ROOT_MOCO)" $(GO4DLLPATH)

qt4-interface: $(QT4ROOT_LIB)

clean-qt4-interface:
	@rm -f $(QT4ROOT_O) $(QT4ROOT_MOCO)
	@$(CleanQLib) $(QT4ROOT_VERSUF) $(QT4ROOT_LIBNAME) $(GO4DLLPATH)
	@rm -f $(QT4ROOT_MOCS) $(QT4ROOT_DEP) $(QT4ROOT_PUBH)

$(QT4ROOT_MOCS): $(QT4ROOT_DIR)/moc_%.cpp: $(QT4ROOT_DIR)/%.h
	$(MOC) $< -o $@
