set(LIBS_BASESET ${ROOT_LIBRARIES} ${ROOT_XMLIO_LIBRARY})

if(WIN32)
  set(libprefix lib)
  set(libsuffix .dll)
elseif(APPLE)
  set(libprefix ${CMAKE_SHARED_LIBRARY_PREFIX})
  if(CMAKE_PROJECT_NAME STREQUAL Go4)
    set(libsuffix .so)
  else()
    set(libsuffix ${CMAKE_SHARED_LIBRARY_SUFFIX})
  endif()
else()
  set(libprefix ${CMAKE_SHARED_LIBRARY_PREFIX})
  set(libsuffix ${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

set(GO4_LIBRARY_PROPERTIES
    SUFFIX ${libsuffix}
    PREFIX ${libprefix}
    IMPORT_PREFIX ${libprefix})

#---------------------------------------------------------------------------------------------------
#---GO4_INSTALL_HEADERS([hdr1 hdr2 ...])
#---------------------------------------------------------------------------------------------------
function(GO4_INSTALL_HEADERS)
  cmake_parse_arguments(ARG "MAKESUB" "" "" ${ARGN})
  string(REPLACE ${CMAKE_SOURCE_DIR} "" tgt ${CMAKE_CURRENT_SOURCE_DIR})
  string(MAKE_C_IDENTIFIER move_header${tgt} tgt)
  if(ARG_MAKESUB)
     file(RELATIVE_PATH subdir ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  foreach (include_file ${ARG_UNPARSED_ARGUMENTS})
    set (src ${CMAKE_CURRENT_SOURCE_DIR}/${include_file})
    set (dst ${CMAKE_BINARY_DIR}/include/${include_file})
    add_custom_command(
      OUTPUT ${dst}
      COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
      COMMENT "Copying header ${src} to ${CMAKE_BINARY_DIR}/include"
      DEPENDS ${src})
    list(APPEND dst_list ${dst})

    install(FILES ${dst} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers)

    if(ARG_MAKESUB)
       set (dst2 ${CMAKE_BINARY_DIR}/include/${subdir}/${include_file})
       add_custom_command(
          OUTPUT ${dst2}
          COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst2}
          COMMENT "Copying header ${src} to ${CMAKE_BINARY_DIR}/include/${subdir}"
          DEPENDS ${src})
       list(APPEND dst_list ${dst2})
       install(FILES ${dst} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${subdir} COMPONENT headers)
    endif()
  endforeach()
  add_custom_target(${tgt} DEPENDS ${dst_list})
  set_property(GLOBAL APPEND PROPERTY GO4_HEADER_TARGETS ${tgt})
endfunction()

#---------------------------------------------------------------------------------------------------
#---GO4_SOURCES(libname
#               HEADERS header1 header2    :
#               SOURCES src1 src2          :
#)
#---------------------------------------------------------------------------------------------------
function(GO4_SOURCES libname)
  cmake_parse_arguments(ARG "" "" "HEADERS;SOURCES" ${ARGN})

  file(GLOB dir RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

  set_property(GLOBAL APPEND PROPERTY ${libname}_incdirs ${CMAKE_CURRENT_SOURCE_DIR})

  if(ARG_HEADERS)
    foreach(hdr ${ARG_HEADERS})
#      set_property(GLOBAL APPEND PROPERTY ${libname}_headers ${dir}/${hdr})
      set_property(GLOBAL APPEND PROPERTY ${libname}_headers ${hdr})
    endforeach()
  endif()

  if(ARG_SOURCES)
    foreach(src ${ARG_SOURCES})
      set_property(GLOBAL APPEND PROPERTY ${libname}_sources ${dir}/${src})
    endforeach()
  endif()
endfunction()


#---------------------------------------------------------------------------------------------------
#---GO4_LINK_LIBRARY(libname
#                     SOURCES src1 src2          :
#                     LIBRARIES lib1 lib2        : direct linked libraries
#                     DEFINITIONS def1 def2      : library definitions
#)
function(GO4_LINK_LIBRARY libname)
   cmake_parse_arguments(ARG "NOEXPORT" "" "SOURCES;LIBRARIES;DEFINITIONS" ${ARGN})

   add_library(${libname} SHARED ${ARG_SOURCES})

   if(MSVC AND NOT ARG_NOEXPORT)
      set_target_properties(${libname} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
   endif()

   set_target_properties(${libname} PROPERTIES ${GO4_LIBRARY_PROPERTIES})

   target_compile_definitions(${libname} PUBLIC ${ARG_DEFINITIONS})

   target_link_libraries(${libname} ${ARG_LIBRARIES})
endfunction()


#---------------------------------------------------------------------------------------------------
#---GO4_STANDARD_LIBRARY(libname
#                           LINKDEF linkdef            :
#                           HEADERS header1 header2    : header files, configured via GO4_SOURCES macro
#                           SOURCES src1 src2          : source files, configured via GO4_SOURCES macro
#                           INCDIRS dir1 dir2          : include dirs, configured via GO4_SOURCES macro
#                           DEPENDENCIES lib1 lib2     : dependend go4 libraries
#                           LIBRARIES lib1 lib2        : direct linked libraries
#                           DEFINITIONS def1 def2      : library definitions
#                           NOINSTALL                  : avoid installation, used for go4 internal examples
#)
#---------------------------------------------------------------------------------------------------
function(GO4_STANDARD_LIBRARY libname)
  cmake_parse_arguments(ARG "NOINSTALL" "LINKDEF" "HEADERS;SOURCES;INCDIRS;DEPENDENCIES;LIBRARIES;DEFINITIONS" ${ARGN})

  if(NOT ARG_SOURCES AND NOT ARG_HEADERS)
     get_property(ARG_HEADERS GLOBAL PROPERTY ${libname}_headers)
     get_property(ARG_SOURCES GLOBAL PROPERTY ${libname}_sources)
  endif()

  if(NOT ARG_INCDIRS)
     get_property(ARG_INCDIRS GLOBAL PROPERTY ${libname}_incdirs)
  endif()

  GO4_LINK_LIBRARY(${libname}
                   SOURCES ${ARG_SOURCES}
                   DEFINITIONS -D${GO4_PLATFORM} ${GO4_DEFINITIONS} ${ARG_DEFINITIONS}
                   LIBRARIES ${LIBS_BASESET} ${ARG_DEPENDENCIES} ${ARG_LIBRARIES})

  if(CMAKE_PROJECT_NAME STREQUAL Go4)
     add_dependencies(${libname} move_headers ${ARG_DEPENDENCIES})
  endif()

  target_include_directories(${libname} PRIVATE ${CMAKE_BINARY_DIR}/include ${ARG_INCDIRS})

  if(ARG_LINKDEF AND ARG_HEADERS)
     ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS}
                              MODULE ${libname}
                              LINKDEF ${ARG_LINKDEF}
                              DEPENDENCIES ${ARG_DEPENDENCIES})
  endif()

  if (NOT ARG_NOINSTALL)
    install(TARGETS ${libname} EXPORT ${CMAKE_PROJECT_NAME}Exports
                               RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries
                               LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
                               ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
  endif()

endfunction()

#-------------------------------------------------------------------------------
function(GO4_TARGETNAME_FROM_FILE resultvar)
  string(REPLACE "${CMAKE_SOURCE_DIR}"   "" relativepath ${CMAKE_CURRENT_SOURCE_DIR})
  if(relativepath)
     string(REPLACE "/" "-" targetname ${relativepath})
  endif()
  set(${resultvar} "${targetname}" PARENT_SCOPE)
endfunction(GO4_TARGETNAME_FROM_FILE)

#---------------------------------------------------------------------------------------------------
#---GO4_USER_ANALYSIS([NAME libname]             : library name, default Go4UserAnalysis
#                     LINKDEF linkdef            :
#                     HEADERS header1 header2    :
#                     SOURCES src1 src2          :
#                     DEFINITIONS def1 def2      : library definitions
#                     LIBRARIES lib1 lib2        : linked libraries
#                     INCLUDE_DIRS dir1 dir2     : extra include directories
#                     TARGET_DIR dir             : target directory
#                     COPY file1 file2           : copy files to destination directory
#)
#---------------------------------------------------------------------------------------------------
function(GO4_USER_ANALYSIS)
  cmake_parse_arguments(ARG "" "NAME;LINKDEF;TARGET_DIR" "HEADERS;SOURCES;DEFINITIONS;LIBRARIES;INCLUDE_DIRS;COPY" ${ARGN})

  set(libname Go4UserAnalysis)
  if(ARG_NAME)
    set(libname ${ARG_NAME})
  endif()

  if(ARG_TARGET_DIR)
     set(tgtdir ${ARG_TARGET_DIR})
     set(tgt)
  else()
     set(tgtdir ${CMAKE_CURRENT_BINARY_DIR})
     GO4_TARGETNAME_FROM_FILE(tgt)
  endif()

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${tgtdir})

  if(CMAKE_PROJECT_NAME STREQUAL Go4)
     set(go4_libs Go4Fit Go4Base Go4ThreadManager Go4TaskHandler Go4AnalBase Go4Analysis)
     set(dict_depend ${go4_libs})
  else()
     set(go4_libs ${Go4Fit_LIBRARY} ${Go4Base_LIBRARY} ${Go4ThreadManager_LIBRARY} ${Go4TaskHandler_LIBRARY} ${Go4AnalBase_LIBRARY} ${Go4Analysis_LIBRARY})

  endif()

  GO4_LINK_LIBRARY(${libname}${tgt}
                   SOURCES ${ARG_SOURCES}
                   LIBRARIES ${LIBS_BASESET} ${go4_libs} ${ARG_LIBRARIES}
                   DEFINITIONS -D${GO4_PLATFORM} ${GO4_DEFINITIONS} ${ARG_DEFINITIONS})

  set_target_properties(${libname}${tgt} PROPERTIES LIBRARY_OUTPUT_NAME ${libname})

  if(CMAKE_PROJECT_NAME STREQUAL Go4)
     target_include_directories(${libname}${tgt} PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR} ${ARG_INCLUDE_DIRS})
  else()
     target_include_directories(${libname}${tgt} PRIVATE ${GO4_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${ARG_INCLUDE_DIRS})
  endif()

  ROOT_GENERATE_DICTIONARY(G__${libname}${tgt} ${ARG_HEADERS}
                          MODULE ${libname}${tgt}
                          LINKDEF ${ARG_LINKDEF}
                          DEPENDENCIES ${dict_depend}
                          NOINSTALL)

  if(ARG_COPY)
     foreach(f ${ARG_COPY})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${f} DESTINATION ${tgtdir})
     endforeach()
  endif()

endfunction()
