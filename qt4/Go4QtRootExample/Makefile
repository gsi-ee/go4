all: qtrootexample

ROOTINCPATH := $(shell root-config --incdir)
ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLIBS    := $(shell root-config --libs)
ROOTGLIBS   := $(shell root-config --glibs)

ifneq ($(findstring -std=c++11,$(ROOTCFLAGS)),)
   QMAKEOPTFLAGS := "CONFIG+=c++11"
endif
ifneq ($(findstring -std=c++14,$(ROOTCFLAGS)),)
    QMAKEOPTFLAGS := "CONFIG+=c++14"
endif
ifneq ($(findstring -std=c++17,$(ROOTCFLAGS)),)
    QMAKEOPTFLAGS := "CONFIG+=c++17"
endif
ifneq ($(findstring -std=c++20,$(ROOTCFLAGS)),)
    QMAKEOPTFLAGS := "CONFIG+=c++20"
endif

WITHGED = yes

ifneq ($(WITHGED), no)
   ROOTGLIBS += -lGed
else
   QMAKEOPTFLAGS += "QMAKE_CXXFLAGS+=-D__NOGO4GED__"
endif

HDRS = ExampleWidget.h 
SRCS = ExampleWidget.cpp ExampleMain.cpp  
UIS = ExampleWidget.ui

Makefile.qt: qtrootexample.pro $(UIS)
	@echo "Generating Makefile.qt"
	qmake-qt5 qtrootexample.pro -o Makefile.qt $(QMAKEOPTFLAGS) "LIBS+=$(ROOTGLIBS)" "INCLUDEPATH+=$(ROOTINCPATH)" "DEPENDPATH+=$(ROOTINCPATH)" 

qtrootexample: Makefile.qt $(HDRS) $(SRCS)
	@echo "Compiling qtrootexample..."
	$(MAKE) -f Makefile.qt

clean:
	@echo "Clean generated files"
	rm -f Makefile.qt ui_ExampleWidget.h .qmake.stash qtrootexample
	rm -rf .obj .moc 
