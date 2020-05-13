#-----------------------------------------------------------------------
#       The GSI Online Offline Object Oriented (Go4) Project
#         Experiment Data Processing at EE department, GSI
#-----------------------------------------------------------------------
# Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
#                     Planckstr. 1, 64291 Darmstadt, Germany
# Contact:            http://go4.gsi.de
#-----------------------------------------------------------------------
# This software can be used under the license agreements as stated
# in Go4License.txt file which is part of the distribution.
#-----------------------------------------------------------------------

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(go4)

# include ROOT project
# try to use standard macros

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

#---Set paths where to put the libraries, executables and headers------------------------------
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


#--- Enable Folders in IDE like Visual Studio ---
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#--- where to search for cmake modules ----
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

#--- Load some basic macros ---
include(Go4Configuration)
include(Go4Macros)
include(Go4InstallDirs)

#--- Search for external packages ---
find_package(DABC)

# ================= Go4Fit library ======================

add_subdirectory(Go4Fit)

# ================= Go4Base library ======================

add_subdirectory(Go4LockGuard)
add_subdirectory(Go4Log)
add_subdirectory(Go4Exceptions)
add_subdirectory(Go4CommandsBase)
add_subdirectory(Go4StatusBase)

get_property(__headers GLOBAL PROPERTY Go4Base_headers)
get_property(__sources GLOBAL PROPERTY Go4Base_sources)

GO4_STANDARD_LIBRARY(Go4Base
                     LINKDEF Go4Dict/Go4BaseLinkDef.h
                     HEADERS ${__headers}
                     SOURCES ${__sources})

# ================= Go4ThreadManager library ======================

add_subdirectory(Go4ThreadManager)

# ================== Go4TaskHandler library ================= 

add_subdirectory(Go4Socket)
add_subdirectory(Go4Queue)
add_subdirectory(Go4TaskHandler)
add_subdirectory(Go4CommandsTaskHandler)

get_property(__headers GLOBAL PROPERTY Go4TaskHandler_headers)
get_property(__sources GLOBAL PROPERTY Go4TaskHandler_sources)

GO4_STANDARD_LIBRARY(Go4TaskHandler
                     LINKDEF Go4Dict/Go4TaskHandlerLinkDef.h
                     HEADERS ${__headers}
                     SOURCES ${__sources}
                     DEPENDENCIES Go4Base Go4ThreadManager)

# ================== Go4AnalysisBase library ================= 

add_subdirectory(MbsAPIbase)
add_subdirectory(Go4Event)
add_subdirectory(Go4EventServer)
if(hdf5)
   add_subdirectory(Go4HDF5)
endif()
add_subdirectory(Go4ConditionsBase)
add_subdirectory(Go4DynamicList)
add_subdirectory(Go4StatusAnalysis)
add_subdirectory(Go4Analysis)

get_property(__headers GLOBAL PROPERTY Go4AnalysisBase_headers)
get_property(__sources GLOBAL PROPERTY Go4AnalysisBase_sources)

GO4_STANDARD_LIBRARY(Go4AnalysisBase
                     LINKDEF Go4Dict/Go4AnalysisBaseLinkDef.h
                     HEADERS ${__headers}
                     SOURCES ${__sources}
                     DEPENDENCIES Go4Base Go4ThreadManager Go4TaskHandler
                     DEFINITIONS Linux)

# ================== Go4Analysis library ================= 

add_subdirectory(MbsAPI)
add_subdirectory(RawAPI)
add_subdirectory(Go4HistogramServer)
add_subdirectory(Go4CommandsAnalysis)
add_subdirectory(Go4AnalysisClient)


get_property(__headers GLOBAL PROPERTY Go4Analysis_headers)
get_property(__sources GLOBAL PROPERTY Go4Analysis_sources)

# special definitions
set_source_files_properties(MbsAPI/f_evt.c PROPERTIES COMPILE_DEFINITIONS RFIO)
set_source_files_properties(MbsAPI/fLmd.c PROPERTIES COMPILE_DEFINITIONS _LARGEFILE64_SOURCE)
set_source_files_properties(${RawAPI_allsources} PROPERTIES COMPILE_DEFINITIONS SYSTEM64)

GO4_STANDARD_LIBRARY(Go4Analysis
                     LINKDEF Go4Dict/Go4AnalysisLinkDef.h
                     HEADERS ${__headers}
                     SOURCES ${__sources}
                     DEPENDENCIES Go4Base Go4ThreadManager Go4TaskHandler Go4AnalysisBase
                     DEFINITIONS Linux)

# ================== Go4Gui library ================= 

add_subdirectory(Go4ObjectManager)
add_subdirectory(Go4Display)
add_subdirectory(Go4Proxies)

get_property(__headers GLOBAL PROPERTY Go4Gui_headers)
get_property(__sources GLOBAL PROPERTY Go4Gui_sources)

GO4_STANDARD_LIBRARY(Go4Gui
                     LINKDEF Go4Dict/Go4GuiLinkDef.h
                     HEADERS ${__headers}
                     SOURCES ${__sources}
                     DEPENDENCIES Go4Base Go4ThreadManager Go4TaskHandler Go4AnalysisBase
                     DEFINITIONS Linux)

if(DABC_FOUND)
   target_link_libraries(Go4Gui ${DABC_LIBRARY})
   target_include_directories(Go4Gui PRIVATE ${DABC_INCLUDE_DIR})
else()
   set_source_files_properties(Go4Proxies/TGo4DabcProxy.cxx PROPERTIES COMPILE_DEFINITIONS WITHOUT_DABC)
endif()



get_property(__allHeaders GLOBAL PROPERTY GO4_HEADER_TARGETS)
add_custom_target(move_headers ALL DEPENDS ${__allHeaders})


# ================== Go4 examples ================= 

add_subdirectory(Go4Example2Step)


