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
include(Go4Macros)
include(Go4InstallDirs)


#--- Adding build directories ---
add_subdirectory(Go4Fit)

# Go4Base library
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


get_property(__allHeaders GLOBAL PROPERTY GO4_HEADER_TARGETS)
add_custom_target(move_headers ALL DEPENDS ${__allHeaders})





