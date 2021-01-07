all: qtrootexample

ROOTINCPATH := $(shell root-config --incdir)
ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLIBS    := $(shell root-config --libs)
ROOTGLIBS   := $(shell root-config --glibs)

ifneq ($(findstring -std=c++11,$(ROOTCFLAGS)),)
   QMAKEOPTFLAG := "CONFIG+=c++11"
endif
ifneq ($(findstring -std=c++14,$(ROOTCFLAGS)),)
    QMAKEOPTFLAG := "CONFIG+=c++14"
endif
ifneq ($(findstring -std=c++17,$(ROOTCFLAGS)),)
    QMAKEOPTFLAG := "CONFIG+=c++17"
endif


Makefile.qt: qtrootexample.pro
	@echo "Generating Makefile.qt"
	qmake-qt5 qtrootexample.pro -o Makefile.qt $(QMAKEOPTFLAG) "LIBS+=$(ROOTGLIBS)" "INCLUDEPATH+=$(ROOTINCPATH)" "DEPENDPATH+=$(ROOTINCPATH)" 

qtrootexample : Makefile.qt
	@echo "Compiling qtrootexample..."
	$(MAKE) -f Makefile.qt
