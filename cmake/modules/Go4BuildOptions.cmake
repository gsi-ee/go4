
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

GO4_BUILD_OPTION(qt4 OFF "Use qt4 for GUI build")
GO4_BUILD_OPTION(hdf5 ON "Enable support of HDF5")
GO4_BUILD_OPTION(dabc ON "Enable support of DABC")

#--- The 'all' option switches ON major options---------------------------------------------------
if(all)
  set(hdf5_defvalue ON)
  set(dabc_defvalue ON)
endif()

#---Define at moment the options with the selected default values-----------------------------
GO4_APPLY_OPTIONS()

#---RPATH options-------------------------------------------------------------------------------
#  When building, don't use the install RPATH already (but later on when installing)
set(CMAKE_SKIP_BUILD_RPATH FALSE)         # don't skip the full RPATH for the build tree
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # use always the build RPATH for the build tree
set(CMAKE_MACOSX_RPATH TRUE)              # use RPATH for MacOSX
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # point to directories outside the build tree to the install RPATH
