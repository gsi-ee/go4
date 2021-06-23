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

include(InstallRequiredSystemLibraries)

#----------------------------------------------------------------------------------------------------
# General packaging setup - variable relavant to all package formats
#
set(CPACK_PACKAGE_DESCRIPTION "Go4 project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Go4 project")
set(CPACK_PACKAGE_VENDOR "Go4 project")
set(CPACK_PACKAGE_VERSION ${GO4_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${GO4_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${GO4_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${GO4_PATCH_VERSION})

string(REGEX REPLACE "^([0-9]+).*$" "\\1" CXX_MAJOR ${CMAKE_CXX_COMPILER_VERSION})
string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\2" CXX_MINOR ${CMAKE_CXX_COMPILER_VERSION})

#---Resource Files-----------------------------------------------------------------------------------
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/Go4License.txt")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_BINARY_DIR}/README.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_BINARY_DIR}/README.md")

#---Source package settings--------------------------------------------------------------------------
set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/tests
    "~$"
    "/CVS/"
    "/.svn/"
    "/\\\\\\\\.svn/"
    "/.git/"
    "/\\\\\\\\.git/"
    "\\\\\\\\.swp$"
    "\\\\\\\\.swp$"
    "\\\\.swp"
    "\\\\\\\\.#"
    "/#"
)
set(CPACK_SOURCE_STRIP_FILES "")

#---Binary package setup-----------------------------------------------------------------------------
if(MSVC)
  if (MSVC_VERSION LESS 1900)
    math(EXPR VS_VERSION "${VC_MAJOR} - 6")
  elseif(MSVC_VERSION LESS 1910)
    math(EXPR VS_VERSION "${VC_MAJOR} - 5")
  elseif(MSVC_VERSION LESS 1919)
    math(EXPR VS_VERSION "${VC_MAJOR} - 4")
  elseif(MSVC_VERSION LESS 1925)
    math(EXPR VS_VERSION "${VC_MAJOR} - 3")
  endif()
  set(COMPILER_NAME_VERSION ".vc${VS_VERSION}")
else()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(COMPILER_NAME_VERSION "-gcc${CXX_MAJOR}.${CXX_MINOR}")
  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(COMPILER_NAME_VERSION "-clang${CXX_MAJOR}${CXX_MINOR}")
  endif()
endif()

#---Processor architecture---------------------------------------------------------------------------

set(arch ${CMAKE_SYSTEM_PROCESSOR})

#---OS and version-----------------------------------------------------------------------------------
if(APPLE)
  execute_process(COMMAND sw_vers "-productVersion"
                  COMMAND cut -d . -f 1-2
                  OUTPUT_VARIABLE osvers OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(OS_NAME_VERSION macos-${osvers}-${CMAKE_SYSTEM_PROCESSOR})
elseif(WIN32)
  set(OS_NAME_VERSION win32)
else()
  execute_process(COMMAND lsb_release -is OUTPUT_VARIABLE osid OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND lsb_release -rs OUTPUT_VARIABLE osvers OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(osid MATCHES Ubuntu)
    string(REGEX REPLACE "([0-9]+)[.].*" "\\1" osvers "${osvers}")
    set(OS_NAME_VERSION Linux-ubuntu${osvers}-${arch})
  elseif(osid MATCHES Scientific)
    string(REGEX REPLACE "([0-9]+)[.].*" "\\1" osvers "${osvers}")
    set(OS_NAME_VERSION Linux-slc${osvers}-${arch})
  elseif(osid MATCHES Fedora)
    string(REGEX REPLACE "([0-9]+)" "\\1" osvers "${osvers}")
    set(OS_NAME_VERSION Linux-fedora${osvers}-${arch})
  elseif(osid MATCHES CentOS)
    string(REGEX REPLACE "([0-9]+)[.].*" "\\1" osvers "${osvers}")
    set(OS_NAME_VERSION Linux-centos${osvers}-${arch})
  else()
    set(OS_NAME_VERSION Linux-${osid}${osvers}${arch})
  endif()
endif()
#---Build type---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
  if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
     string(TOLOWER .${CMAKE_BUILD_TYPE} BUILD_TYPE_FOR_NAME)
  endif()
endif()

set(CPACK_PACKAGE_RELOCATABLE True)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "go4_v${GO4_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "go4_v${GO4_VERSION}.${OS_NAME_VERSION}${COMPILER_NAME_VERSION}${BUILD_TYPE_FOR_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "go4" "Go4")

if(WIN32)
  set(CPACK_GENERATOR "ZIP;NSIS")
  set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
elseif(APPLE)
  set(CPACK_GENERATOR "TGZ;productbuild")
  set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
else()
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
endif()

#----------------------------------------------------------------------------------------------------
# Finally, generate the CPack per-generator options file and include the
# base CPack configuration.
#
configure_file(cmake/modules/Go4CPackOptions.cmake.in Go4CPackOptions.cmake @ONLY)
set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/Go4CPackOptions.cmake)
include(CPack)

#----------------------------------------------------------------------------------------------------
# Define components and installation types (after CPack included!)
#
cpack_add_install_type(full      DISPLAY_NAME "Full Installation")
cpack_add_install_type(minimal   DISPLAY_NAME "Minimal Installation")
cpack_add_install_type(developer DISPLAY_NAME "Developer Installation")

cpack_add_component(root
    DISPLAY_NAME "ROOT framework"
    DESCRIPTION "ROOT compiled with Qt5Web support"
     INSTALL_TYPES full developer)

cpack_add_component(qt5
    DISPLAY_NAME "Qt5.15.2 libs"
    DESCRIPTION "Subset of Qt5.15.2 only to run Go4 and ROOT webgui"
     INSTALL_TYPES full developer)

cpack_add_component(applications
    DISPLAY_NAME "Go4 Applications"
    DESCRIPTION "Go4 executables such as go4.exe"
     INSTALL_TYPES full minimal developer)

cpack_add_component(libraries
    DISPLAY_NAME "Go4 Libraries"
    DESCRIPTION "All Go4 libraries and dictionaries"
     INSTALL_TYPES full minimal developer)

cpack_add_component(headers
    DISPLAY_NAME "C++ Headers"
    DESCRIPTION "These are needed to do any development"
     INSTALL_TYPES full developer)

cpack_add_component(tests
    DISPLAY_NAME "Go4 Tests and Tutorials"
    DESCRIPTION "These are needed to do any test and tutorial"
     INSTALL_TYPES full developer)
