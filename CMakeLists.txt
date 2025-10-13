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

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Go4)

if(MSVC)
# exclude /EHsc windows compiler options which overwritten by ROOT project
  string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

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
include(Go4InstallDirs)
include(Go4Configuration)
include(Go4Macros)
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
   set_source_files_properties(MbsAPI/f_evt.c PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/RawAPI)
   if(MSVC)
      set_source_files_properties(MbsAPI/f_evt.c ${RawAPI_allsources} PROPERTIES COMPILE_FLAGS /W0)
   else()
      set_source_files_properties(MbsAPI/f_evt.c ${RawAPI_allsources} PROPERTIES COMPILE_FLAGS -Wno-all)
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
   target_link_libraries(Go4Gui dabc::DabcBase)
else()
   set_source_files_properties(Go4Proxies/TGo4DabcProxy.cxx PROPERTIES COMPILE_DEFINITIONS WITHOUT_DABC)
endif()

# ================== Go4 Gui =================
if(gui)
  add_subdirectory(qt4)
endif(gui)

# ================== Go4 examples =================

if(examples)

   ExternalProject_Add(Go4FitExample
                     DEPENDS Go4Fit
                     CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}$<SEMICOLON>$ENV{CMAKE_PREFIX_PATH}
                     SOURCE_DIR ${CMAKE_SOURCE_DIR}/Go4FitExample
                     BINARY_DIR Go4FitExample
                     BUILD_ALWAYS true
                     INSTALL_COMMAND "")

   ExternalProject_Add(Go4ThreadManagerExample
                     DEPENDS Go4ThreadManager
                     CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}$<SEMICOLON>$ENV{CMAKE_PREFIX_PATH}
                     SOURCE_DIR ${CMAKE_SOURCE_DIR}/Go4ThreadManagerExample
                     BINARY_DIR Go4ThreadManagerExample
                     BUILD_ALWAYS true
                     INSTALL_COMMAND "")

   ExternalProject_Add(Go4TaskHandlerExample
                     DEPENDS Go4TaskHandler
                     CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}$<SEMICOLON>$ENV{CMAKE_PREFIX_PATH}
                     SOURCE_DIR ${CMAKE_SOURCE_DIR}/Go4TaskHandlerExample
                     BINARY_DIR Go4TaskHandlerExample
                     BUILD_ALWAYS true
                     INSTALL_COMMAND "")

   list(APPEND list_of_examples
      Go4ExampleSimple
      Go4Example1Step
      Go4Example2Step
      Go4ExampleAdvanced
      Go4ExampleUserSource
      Go4ExampleUserStore
      Go4ExampleMesh
      Go4EventServerExample
   )

   if(DABC_FOUND)
      list(APPEND list_of_examples Go4ExampleDabc)
   endif()

   foreach(ex ${list_of_examples})
      ExternalProject_Add(${ex}
                        DEPENDS Go4Analysis Go4AnalBase Go4Fit
                        BUILD_ALWAYS true
                        CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}$<SEMICOLON>$ENV{CMAKE_PREFIX_PATH} -DBUILDING_GO4=ON
                        SOURCE_DIR ${CMAKE_SOURCE_DIR}/${ex}
                        BINARY_DIR ${ex}
                        INSTALL_COMMAND "")
   endforeach()

endif(examples)

# ================== Produce Go4 headers ==========

get_property(__allHeaders GLOBAL PROPERTY GO4_HEADER_TARGETS)
add_custom_target(move_headers ALL DEPENDS ${__allHeaders})

# ================== Copy files, create old support files ==========

file(COPY ${CMAKE_SOURCE_DIR}/Go4License.txt DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/README.md DESTINATION ${CMAKE_BINARY_DIR})
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

file(GLOB go4_html5 "${CMAKE_SOURCE_DIR}/html5/*.*")
foreach(_html5 ${go4_html5})
   file(COPY ${_html5} DESTINATION ${CMAKE_BINARY_DIR}/html5)
endforeach(_html5)

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
file(COPY ${CMAKE_SOURCE_DIR}/data/test.lmd DESTINATION ${CMAKE_BINARY_DIR}/data)

# install directories

install(DIRECTORY docs/ DESTINATION ${GO4_INSTALL_DOCSDIR} COMPONENT applications)
install(DIRECTORY etc/ DESTINATION ${GO4_INSTALL_ETCDIR} COMPONENT applications)
install(DIRECTORY html/ DESTINATION ${GO4_INSTALL_HTMLDIR} COMPONENT applications)
install(DIRECTORY html5/ DESTINATION ${GO4_INSTALL_HTML5DIR} COMPONENT applications)
install(DIRECTORY icons/ DESTINATION ${GO4_INSTALL_ICONSDIR} COMPONENT applications)
install(DIRECTORY macros/ DESTINATION ${GO4_INSTALL_MACROSDIR} COMPONENT applications)
install(DIRECTORY python/ DESTINATION ${GO4_INSTALL_PYTHONDIR} COMPONENT applications)
install(FILES CHANGES.txt Go4License.txt README.md DESTINATION ${GO4_INSTALL_MAINDIR} COMPONENT applications)
install(FILES ${CMAKE_SOURCE_DIR}/data/test.lmd DESTINATION ${GO4_INSTALL_MAINDIR}/data COMPONENT tests)

if(MSVC)
   if(qt6)
      get_filename_component(_qt_dir_ "${Qt6_DIR}/../../.." ABSOLUTE)
   else()
      get_filename_component(_qt_dir_ "${Qt5_DIR}/../../.." ABSOLUTE)
   endif()

   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/go4login.bat.in
                  ${CMAKE_BINARY_DIR}/go4login.bat @ONLY NEWLINE_STYLE CRLF)
   configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/go4.bat.in
                  ${CMAKE_BINARY_DIR}/go4.bat COPYONLY)

   install(FILES ${CMAKE_SOURCE_DIR}/cmake/scripts/go4login.bat
                 ${CMAKE_BINARY_DIR}/go4.bat
                 DESTINATION ${GO4_INSTALL_MAINDIR}
                 COMPONENT applications)

   # install full ROOT distribution, ROOT_DIR pointing to cmake/ subdir of ROOT installation

   get_filename_component(_root_dir_ "${ROOT_DIR}/.." ABSOLUTE)
   install(DIRECTORY ${_root_dir_}/ DESTINATION ${GO4_INSTALL_MAINDIR}/root COMPONENT root)

   if(qt6)
      # install selected files from Qt6 - only run go4gui
      install(FILES ${_qt_dir_}/bin/opengl32sw.dll
                    ${_qt_dir_}/bin/qt.conf
                    ${_qt_dir_}/bin/Qt6Core.dll
                    ${_qt_dir_}/bin/Qt6Gui.dll
                    ${_qt_dir_}/bin/Qt6Network.dll
                    ${_qt_dir_}/bin/Qt6Positioning.dll
                    ${_qt_dir_}/bin/Qt6PrintSupport.dll
                    ${_qt_dir_}/bin/Qt6Qml.dll
                    ${_qt_dir_}/bin/Qt6QmlModels.dll
                    ${_qt_dir_}/bin/Qt6Quick.dll
                    ${_qt_dir_}/bin/Qt6QuickWidgets.dll
                    ${_qt_dir_}/bin/Qt6WebChannel.dll
                    ${_qt_dir_}/bin/Qt6WebEngineCore.dll
                    ${_qt_dir_}/bin/Qt6WebEngineWidgets.dll
                    ${_qt_dir_}/bin/Qt6Widgets.dll
                    ${_qt_dir_}/bin/QtWebEngineProcess.exe
                    ${_qt_dir_}/bin/QtWebEngineProcessd.exe
                    DESTINATION ${GO4_INSTALL_MAINDIR}/qt6/bin
                    COMPONENT qt6)

      install(DIRECTORY ${_qt_dir_}/plugins/ DESTINATION ${GO4_INSTALL_MAINDIR}/qt6/plugins COMPONENT qt6)
      install(DIRECTORY ${_qt_dir_}/resources/ DESTINATION ${GO4_INSTALL_MAINDIR}/qt6/resources COMPONENT qt6)
      install(DIRECTORY ${_qt_dir_}/translations/ DESTINATION ${GO4_INSTALL_MAINDIR}/qt6/translations COMPONENT qt6)
   else()
      # install selected files from Qt5 - only run go4gui
      install(FILES ${_qt_dir_}/bin/libEGL.dll
                     ${_qt_dir_}/bin/libGLESv2.dll
                     ${_qt_dir_}/bin/qt.conf
                     ${_qt_dir_}/bin/Qt5Core.dll
                     ${_qt_dir_}/bin/Qt5Gui.dll
                     ${_qt_dir_}/bin/Qt5Network.dll
                     ${_qt_dir_}/bin/Qt5Positioning.dll
                     ${_qt_dir_}/bin/Qt5PrintSupport.dll
                     ${_qt_dir_}/bin/Qt5Qml.dll
                     ${_qt_dir_}/bin/Qt5QmlModels.dll
                     ${_qt_dir_}/bin/Qt5Quick.dll
                     ${_qt_dir_}/bin/Qt5QuickWidgets.dll
                     ${_qt_dir_}/bin/Qt5WebChannel.dll
                     ${_qt_dir_}/bin/Qt5WebEngine.dll
                     ${_qt_dir_}/bin/Qt5WebEngineCore.dll
                     ${_qt_dir_}/bin/Qt5WebEngineWidgets.dll
                     ${_qt_dir_}/bin/Qt5Widgets.dll
                     ${_qt_dir_}/bin/QtWebEngineProcess.exe
                     ${_qt_dir_}/bin/QtWebEngineProcessd.exe
                     DESTINATION ${GO4_INSTALL_MAINDIR}/qt5/bin
                     COMPONENT qt5)

      install(DIRECTORY ${_qt_dir_}/plugins/ DESTINATION ${GO4_INSTALL_MAINDIR}/qt5/plugins COMPONENT qt5)
      install(DIRECTORY ${_qt_dir_}/resources/ DESTINATION ${GO4_INSTALL_MAINDIR}/qt5/resources COMPONENT qt5)
      install(DIRECTORY ${_qt_dir_}/translations/ DESTINATION ${GO4_INSTALL_MAINDIR}/qt5/translations COMPONENT qt5)
   endif()

   # add all tests sources
   foreach(ex ${examples})
      install(DIRECTORY ${ex}/ DESTINATION ${GO4_INSTALL_MAINDIR}/examples/${ex} COMPONENT tests)
   endforeach()

   # and add binaries of Go4ExampleSimple to have test application
   install(DIRECTORY ${CMAKE_BINARY_DIR}/Go4ExampleSimple/Release/ DESTINATION ${GO4_INSTALL_MAINDIR}/test COMPONENT tests)
   install(FILES ${CMAKE_BINARY_DIR}/Go4ExampleSimple/libGo4UserAnalysis.rootmap
                 ${CMAKE_BINARY_DIR}/Go4ExampleSimple/libGo4UserAnalysis_rdict.pcm
                 ${CMAKE_BINARY_DIR}/Go4ExampleSimple/module.modulemap
                 ${CMAKE_SOURCE_DIR}/Go4ExampleSimple/TXXXProc.h
                 ${CMAKE_SOURCE_DIR}/Go4ExampleSimple/TXXXParam.h
                 DESTINATION ${GO4_INSTALL_MAINDIR}/test COMPONENT tests)

endif()

GO4_SHOW_ENABLED_OPTIONS()

#---Packaging-----------------------------
include(Go4CPack)
