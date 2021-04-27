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

project(Go4)

# include ROOT project
# try to use standard macros

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

#---Set paths where to put the libraries, executables and headers------------------------------
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
  foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
  endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif()

#--- Enable Folders in IDE like Visual Studio ---
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#--- where to search for cmake modules ----
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

#--- Load basic macros ---
include(Go4BuildOptions)
include(Go4Configuration)
include(Go4Macros)
include(Go4InstallDirs)
include(ExternalProject)

#--- Search for external packages ---
if(dabc)
   find_package(DABC QUIET)
   if(NOT DABC_FOUND)
      set(dabc OFF)
   endif()
endif()

if(hdf5)
   find_package(HDF5 COMPONENTS CXX QUIET)
   if(NOT HDF5_FOUND)
      set(hdf5 OFF)
   endif()
endif()


# ================= Go4Fit library ======================

add_subdirectory(Go4Fit)

# ================= Go4Base library ======================

add_subdirectory(Go4LockGuard)
add_subdirectory(Go4Log)
add_subdirectory(Go4Exceptions)
add_subdirectory(Go4CommandsBase)
add_subdirectory(Go4StatusBase)

set_source_files_properties(Go4Log/TGo4Log.cxx PROPERTIES COMPILE_DEFINITIONS "COMP_GO4SYS=\"${CMAKE_BINARY_DIR}\";COMP_GO4INC=\"${CMAKE_BINARY_DIR}/include\"")

GO4_STANDARD_LIBRARY(Go4Base
                     LINKDEF Go4Dict/Go4BaseLinkDef.h)

# ================= Go4ThreadManager library ======================

add_subdirectory(Go4ThreadManager)

# ================== Go4TaskHandler library =================

add_subdirectory(Go4Socket)
add_subdirectory(Go4Queue)
add_subdirectory(Go4TaskHandler)
add_subdirectory(Go4CommandsTaskHandler)

GO4_STANDARD_LIBRARY(Go4TaskHandler
                     LINKDEF Go4Dict/Go4TaskHandlerLinkDef.h
                     DEPENDENCIES Go4Base Go4ThreadManager)

# ================== Go4AnalBase library =================

add_subdirectory(MbsAPIbase)
add_subdirectory(Go4Event)
add_subdirectory(Go4EventServer)

if(HDF5_FOUND)
   add_subdirectory(Go4HDF5)
   set(hdf5_lib ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES})
   set(hdf5_def __GO4HDF5__)
endif()

add_subdirectory(Go4ConditionsBase)
add_subdirectory(Go4DynamicList)
add_subdirectory(Go4StatusAnalysis)
add_subdirectory(Go4Analysis)

if(MSVC)
   set(_win_lib ws2_32.lib)
#   $<$<PLATFORM_ID:Windows>:ws2_32.lib>
endif()

GO4_STANDARD_LIBRARY(Go4AnalBase
                     LINKDEF Go4Dict/Go4AnalysisBaseLinkDef.h
                     DEPENDENCIES Go4Base Go4ThreadManager Go4TaskHandler
                     LIBRARIES ${hdf5_lib} ${_win_lib}
                     DEFINITIONS ${hdf5_def} ${_win_def})

if(HDF5_FOUND)
   target_include_directories(Go4AnalBase PRIVATE ${HDF5_CXX_INCLUDE_DIRS})
endif()


# ================== Go4Analysis library =================

add_subdirectory(MbsAPI)
if(GO4_WITH_RFIO)
   add_subdirectory(RawAPI)
endif()
add_subdirectory(Go4HistogramServer)
add_subdirectory(Go4CommandsAnalysis)
add_subdirectory(Go4AnalysisClient)

if(ROOT_http_FOUND)
   add_subdirectory(Go4Http)
   set(http_def WITH_HTTP)
endif()

if(DABC_FOUND)
   set(dabc_def WITH_DABC)
endif()

# special definitions

if(GO4_WITH_RFIO)
   set_source_files_properties(MbsAPI/f_evt.c PROPERTIES COMPILE_DEFINITIONS RFIO)
   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set_source_files_properties(${RawAPI_allsources} PROPERTIES COMPILE_DEFINITIONS SYSTEM64)
   endif()
endif()

set_source_files_properties(MbsAPI/fLmd.c PROPERTIES COMPILE_DEFINITIONS _LARGEFILE64_SOURCE)

GO4_STANDARD_LIBRARY(Go4Analysis
                     LINKDEF Go4Dict/Go4AnalysisLinkDef.h
                     DEPENDENCIES Go4Fit Go4Base Go4ThreadManager Go4TaskHandler Go4AnalBase
                     LIBRARIES ${hdf5_lib}
                     DEFINITIONS ${hdf5_def})

if(HDF5_FOUND)
   target_include_directories(Go4Analysis PRIVATE ${HDF5_CXX_INCLUDE_DIRS})
endif()

# ================== go4analysis executable =================

add_executable(MainUserAnalysis Go4Analysis/MainUserAnalysis.cxx)
target_link_libraries(MainUserAnalysis Go4Base Go4ThreadManager Go4TaskHandler Go4AnalBase Go4Analysis)
target_include_directories(MainUserAnalysis PRIVATE ${CMAKE_BINARY_DIR}/include)
target_compile_definitions(MainUserAnalysis PRIVATE ${http_def} ${hdf5_def} ${dabc_def})
set_target_properties(MainUserAnalysis PROPERTIES OUTPUT_NAME go4analysis)
install(TARGETS MainUserAnalysis EXPORT ${CMAKE_PROJECT_NAME}Exports
                                 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT applications)

# ================== Go4Gui library =================

add_subdirectory(Go4ObjectManager)
add_subdirectory(Go4Display)
add_subdirectory(Go4Proxies)

GO4_STANDARD_LIBRARY(Go4Gui
                     LINKDEF Go4Dict/Go4GuiLinkDef.h
                     DEPENDENCIES Go4Fit Go4Base Go4ThreadManager Go4TaskHandler Go4AnalBase
                     LIBRARIES ${ROOT_Gui_LIBRARY})

if(DABC_FOUND)
   target_link_libraries(Go4Gui ${DabcBase_LIBRARY})
   target_include_directories(Go4Gui PRIVATE ${DABC_INCLUDE_DIR})
else()
   set_source_files_properties(Go4Proxies/TGo4DabcProxy.cxx PROPERTIES COMPILE_DEFINITIONS WITHOUT_DABC)
endif()

# ================== Go4 Gui =================
if(ROOT_VERSION VERSION_LESS 6.23.00)
  if(ROOT_qt5web_FOUND)
    add_subdirectory(webgui6)
  endif()
endif()

add_subdirectory(qt4)

# ================== Go4 examples =================

list(APPEND examples
   Go4FitExample
   Go4ThreadManagerExample
   Go4TaskHandlerExample
   Go4EventServerExample
   Go4Example1Step
   Go4Example2Step
   Go4ExampleAdvanced
   Go4ExampleMesh
   Go4ExampleSimple
   Go4ExampleUserSource
   Go4ExampleUserStore
)

if(DABC_FOUND)
   list(APPEND examples Go4ExampleDabc)
endif()

foreach(ex ${examples})
   ExternalProject_Add(${ex}
                       DEPENDS Go4Analysis
                       CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}
                       SOURCE_DIR ${CMAKE_SOURCE_DIR}/${ex}
                       BINARY_DIR ${ex}
                       INSTALL_COMMAND "")
endforeach()

# this is direct include of the examples
# foreach(ex ${examples})
#    add_subdirectory(${ex})
# endforeach()

# ================== Produce Go4 headers ==========

get_property(__allHeaders GLOBAL PROPERTY GO4_HEADER_TARGETS)
add_custom_target(move_headers ALL DEPENDS ${__allHeaders})

# ================== Copy files, create old support files ==========

file(COPY ${CMAKE_SOURCE_DIR}/Go4License.txt DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/README.txt DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/CHANGES.txt DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/docs/Go4FitTutorial.pdf DESTINATION ${CMAKE_BINARY_DIR}/docs)
file(COPY ${CMAKE_SOURCE_DIR}/docs/Go4Introduction.pdf DESTINATION ${CMAKE_BINARY_DIR}/docs)
file(COPY ${CMAKE_SOURCE_DIR}/docs/Go4Reference.pdf DESTINATION ${CMAKE_BINARY_DIR}/docs)

file(GLOB go4_icons "${CMAKE_SOURCE_DIR}/icons/*.*")
foreach(_icon ${go4_icons})
   file(COPY ${_icon} DESTINATION ${CMAKE_BINARY_DIR}/icons)
endforeach(_icon)


file(GLOB go4_html "${CMAKE_SOURCE_DIR}/html/*.*")
foreach(_html ${go4_html})
   file(COPY ${_html} DESTINATION ${CMAKE_BINARY_DIR}/html)
endforeach(_html)

file(GLOB go4_macros "${CMAKE_SOURCE_DIR}/macros/*.C")
foreach(_macro ${go4_macros})
   file(COPY ${_macro} DESTINATION ${CMAKE_BINARY_DIR}/macros)
endforeach(_macro)

file(COPY ${CMAKE_SOURCE_DIR}/python DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/etc/go4.prefs DESTINATION ${CMAKE_BINARY_DIR}/etc)
file(COPY ${CMAKE_SOURCE_DIR}/etc/Go4LaunchPrefs.txt DESTINATION ${CMAKE_BINARY_DIR}/etc)
file(COPY ${CMAKE_SOURCE_DIR}/etc/Go4ClientStartup.sh DESTINATION ${CMAKE_BINARY_DIR}/etc)
file(COPY ${CMAKE_SOURCE_DIR}/etc/Go4ServerStartup.sh DESTINATION ${CMAKE_BINARY_DIR}/etc)
file(COPY ${CMAKE_SOURCE_DIR}/etc/Go4ShowPdf.sh DESTINATION ${CMAKE_BINARY_DIR}/etc)
file(COPY ${CMAKE_SOURCE_DIR}/etc/htdigest.txt DESTINATION ${CMAKE_BINARY_DIR}/etc)

# install directories

install(DIRECTORY docs/ DESTINATION ${GO4_INSTALL_DOCSDIR})
install(DIRECTORY etc/ DESTINATION ${GO4_INSTALL_ETCDIR})
install(DIRECTORY html/ DESTINATION ${GO4_INSTALL_HTMLDIR})
install(DIRECTORY icons/ DESTINATION ${GO4_INSTALL_ICONSDIR})
install(DIRECTORY macros/ DESTINATION ${GO4_INSTALL_MACROSDIR})
install(FILES CHANGES.txt
              Go4License.txt
              README.txt
              DESTINATION ${GO4_INSTALL_MAINDIR})

if(NOT MSVC)
endif()



GO4_SHOW_ENABLED_OPTIONS()