QTROOT_NAME      = Go4QtRoot
MODULE_NAME      = $(QTROOT_NAME)

QTROOT_DIR       = $(GO4SYS)/qt3/$(QTROOT_NAME)

QTROOTI_LIBNAME =  $(LIB_PREFIX)Go4QtRoot

QTVERSUF = 3.5.0

QTROOTI_S       =  $(QTROOT_DIR)/qrootapplication.cpp \
                   $(QTROOT_DIR)/tqrootguifactory.cpp \
                   $(QTROOT_DIR)/tqcanvasimp.cpp \
                   $(QTROOT_DIR)/tqcanvasmenu.cpp \
                   $(QTROOT_DIR)/tqrootcanvas.cpp \
                   $(QTROOT_DIR)/tqrootwindow.cpp \
                   $(QTROOT_DIR)/tqrootdialog.cpp \
                   $(QTROOT_DIR)/tqapplication.cpp \
                   $(QTROOT_DIR)/qtframe.cpp

QTROOTI_H       =  $(QTROOTI_S:.cpp=.$(HedSuf))
QTROOTI_O       =  $(QTROOTI_S:.cpp=.$(ObjSuf))
QTROOTI_DEP     =  $(QTROOTI_O:.$(ObjSuf)=.$(DepSuf))

QTROOTI_MOCS    =  $(QTROOT_DIR)/moc_tqrootcanvas.cpp \
                   $(QTROOT_DIR)/moc_tqrootwindow.cpp \
                   $(QTROOT_DIR)/moc_tqcanvasmenu.cpp \
                   $(QTROOT_DIR)/moc_tqrootdialog.cpp \
                   $(QTROOT_DIR)/moc_qrootapplication.cpp \
                   $(QTROOT_DIR)/moc_qtframe.cpp

QTROOTI_MOCO    = $(QTROOTI_MOCS:.cpp=.$(ObjSuf))

QTROOTI_LIB     =  $(GO4DLLPATH)/$(QTROOTI_LIBNAME).$(DllSuf).$(QTVERSUF)


# used in the main Makefile

LIBDEPENDENC       += $(QTROOTI_DEP)

ALLHDRS +=  $(patsubst $(QTROOT_DIR)/%.h, $(GO4SYS)/include/%.h, $(QTROOTI_H))

ifdef DOPACKAGE
DISTRFILES         += $(QTROOTI_H) $(QTROOTI_S)
DISTRFILES         += $(QTROOT_DIR)/lockguard.h
DISTRFILES         += $(QTROOT_DIR)/CHANGELOG.txt
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(QTROOT_DIR)/%.h
	@cp -f $< $@

$(QTROOTI_LIB):    $(QTROOTI_O) $(QTROOTI_MOCO)
	@$(MakeQLib) $(QTVERSUF) $(QTROOTI_LIBNAME) "$(QTROOTI_O) $(QTROOTI_MOCO)" $(GO4DLLPATH)

all-$(QTROOT_NAME): $(QTROOTI_LIB)

clean-obj-$(QTROOT_NAME):
	@rm -f $(QTROOTI_O) $(QTROOTI_MOCO)
	@$(CleanQLib) $(QTVERSUF) $(QTROOTI_LIBNAME) $(GO4DLLPATH)

clean-$(QTROOT_NAME): clean-obj-$(QTROOT_NAME)
	@rm -f $(QTROOTI_MOCS) $(QTROOTI_DEP)

$(QTROOTI_MOCS): $(QTROOT_DIR)/moc_%.cpp: $(QTROOT_DIR)/%.h
	$(MOC) $< -o $@
