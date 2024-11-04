
set(go4_build_options)

#---------------------------------------------------------------------------------------------------
#---GO4_BUILD_OPTION( name defvalue [description] )
#---------------------------------------------------------------------------------------------------
function(GO4_BUILD_OPTION opt defvalue)
  if(ARGN)
    set(description ${ARGN})
  else()
    set(description " ")
  endif()
  set(${opt}_defvalue    ${defvalue} PARENT_SCOPE)
  set(${opt}_description ${description} PARENT_SCOPE)
  set(go4_build_options  ${go4_build_options} ${opt} PARENT_SCOPE)
endfunction()

#---------------------------------------------------------------------------------------------------
#---GO4_APPLY_OPTIONS()
#---------------------------------------------------------------------------------------------------
function(GO4_APPLY_OPTIONS)
  foreach(opt ${go4_build_options})
     option(${opt} "${${opt}_description}" ${${opt}_defvalue})
  endforeach()
endfunction()

#---------------------------------------------------------------------------------------------------
#---GO4_GET_OPTIONS(result ENABLED)
#---------------------------------------------------------------------------------------------------
function(GO4_GET_OPTIONS result)
  CMAKE_PARSE_ARGUMENTS(ARG "ENABLED" "" "" ${ARGN})
  set(enabled)
  foreach(opt ${go4_build_options})
    if(ARG_ENABLED)
      if(${opt})
        set(enabled "${enabled} ${opt}")
      endif()
    else()
      set(enabled "${enabled} ${opt}")
    endif()
  endforeach()
  set(${result} "${enabled}" PARENT_SCOPE)
endfunction()

#---------------------------------------------------------------------------------------------------
#---GO4_SHOW_ENABLED_OPTIONS()
#---------------------------------------------------------------------------------------------------
function(GO4_SHOW_ENABLED_OPTIONS)
  set(enabled_opts)
  GO4_GET_OPTIONS(enabled_opts ENABLED)
  foreach(opt ${enabled_opts})
    message(STATUS "Enabled support for: ${opt}")
  endforeach()
endfunction()

#--------------------------------------------------------------------------------------------------
#---Full list of options with their descriptions and default values
#   The default value can be changed as many times as we wish before calling GO4_APPLY_OPTIONS()
#--------------------------------------------------------------------------------------------------

GO4_BUILD_OPTION(qt5 OFF "Use qt5 for GUI build")
GO4_BUILD_OPTION(qt6 OFF "Use qt6 for GUI build (default)")
GO4_BUILD_OPTION(hdf5 ON "Enable support of HDF5")
GO4_BUILD_OPTION(dabc OFF "Enable support of DABC")
GO4_BUILD_OPTION(gui ON "Build go4 GUI components")

#--- The 'all' option switches ON major options---------------------------------------------------
if(all)
  set(hdf5_defvalue ON)
  set(dabc_defvalue ON)
endif()

if(NOT qt5 AND NOT qt6)
  find_package(Qt6 COMPONENTS Widgets QUIET)
  if(Qt6_FOUND)
    set(qt6_defvalue ON)
  else()
    set(qt5_defvalue ON)
  endif()
endif()


#---Define at moment the options with the selected default values-----------------------------
GO4_APPLY_OPTIONS()

if(qt5 AND qt6)
  if(GO4_QTVERSION STREQUAL Qt5)
    set(qt5 OFF CACHE STRING "Use qt5 for GUI build" FORCE)
  else()
    set(qt6 OFF CACHE STRING "Use qt6 for GUI build" FORCE)
  endif()
endif()


#---RPATH options-------------------------------------------------------------------------------
#  When building, don't use the install RPATH already (but later on when installing)
set(CMAKE_SKIP_RPATH FALSE) # define RPATHs both for the build tree (CMAKE_SKIP_BUILD_RPATH=FALSE) and for the install tree (CMAKE_SKIP_INSTALL_RPATH=FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # use install tree RPATH only for install targets
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE) # make RPATH entries within build tree (i.e. *not* the install tree) relative, and thus relocatable
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # any directory external to and used by this project will be added to RPATH
if(CMAKE_HOST_APPLE)
  set(CMAKE_MACOSX_RPATH TRUE)
endif()
