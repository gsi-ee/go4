
#---------------------------------------------------------------------------------------------------
#---GO4_INSTALL_HEADERS([hdr1 hdr2 ...])
#---------------------------------------------------------------------------------------------------
function(GO4_INSTALL_HEADERS)
  cmake_parse_arguments(ARG "" "" "" ${ARGN})
  string(REPLACE ${CMAKE_SOURCE_DIR} "" tgt ${CMAKE_CURRENT_SOURCE_DIR})
  string(MAKE_C_IDENTIFIER move_header${tgt} tgt)
  foreach (include_file ${ARG_UNPARSED_ARGUMENTS})
    set (src ${CMAKE_CURRENT_SOURCE_DIR}/${include_file})
    set (dst ${CMAKE_BINARY_DIR}/include/${include_file})
    add_custom_command(
      OUTPUT ${dst}
      COMMAND ${CMAKE_COMMAND} -E copy ${src} ${dst}
      COMMENT "Copying header ${src} to ${CMAKE_BINARY_DIR}/include"
      DEPENDS ${src})
    list(APPEND dst_list ${dst})
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
  
  if(ARG_HEADERS)
    foreach(hdr ${ARG_HEADERS})
      set_property(GLOBAL APPEND PROPERTY ${libname}_headers ${dir}/${hdr})
    endforeach()
  endif()

  if(ARG_SOURCES)
    foreach(src ${ARG_SOURCES})
      set_property(GLOBAL APPEND PROPERTY ${libname}_sources ${dir}/${src})
    endforeach()
  endif()
endfunction()


#---------------------------------------------------------------------------------------------------
#---GO4_STANDARD_LIBRARY(libname
#                           LINKDEF linkdef            : 
#                           HEADERS header1 header2    : 
#                           SOURCES src1 src2          : 
#                           DEPENDENCIES lib1 lib2     : dependend go4 libraries
#                           LIBRARIES lib1 lib2        : direct linked libraries
#                           DEFINITIONS def1 def2      : library definitions
#)
#---------------------------------------------------------------------------------------------------
function(GO4_STANDARD_LIBRARY libname)
  cmake_parse_arguments(ARG "" "LINKDEF" "HEADERS;SOURCES;DEPENDENCIES;LIBRARIES;DEFINITIONS" ${ARGN})

  add_library(${libname} SHARED ${ARG_SOURCES})

  add_dependencies(${libname} move_headers ${ARG_DEPENDENCIES})

  if (ARG_DEFINITIONS)
     target_compile_definitions(${libname} PUBLIC ${ARG_DEFINITIONS})
  endif()

  target_link_libraries(${libname} ${LIBS_BASESET})
  
  target_include_directories(${libname} PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  
  if (ARG_DEPENDENCIES)
    target_link_libraries(${libname} ${ARG_DEPENDENCIES})
  endif()

  if (ARG_LIBRARIES)
    target_link_libraries(${libname} ${ARG_LIBRARIES})
  endif()

  if(ARG_LINKDEF)
     ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS}
                              MODULE ${libname}
                              LINKDEF ${ARG_LINKDEF}
                              DEPENDENCIES ${ARG_DEPENDENCIES})
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
#---GO4_USER_ANALYSIS(
#                     LINKDEF linkdef            : 
#                     HEADERS header1 header2    : 
#                     SOURCES src1 src2          : 
#                     DEPENDENCIES lib1 lib2     : dependend go4 libraries
#                     DEFINITIONS def1 def2      : library definitions
#                     LIBRARIES lib1 lib2        : linked libraries
#                     INCLUDE_DIRS dir1 dir2     : extra include directories
#)
#---------------------------------------------------------------------------------------------------
function(GO4_USER_ANALYSIS)
  cmake_parse_arguments(ARG "" "LINKDEF" "HEADERS;SOURCES;DEPENDENCIES;DEFINITIONS;LIBRARIES;INCLUDE_DIRS" ${ARGN})
  
  set(libname Go4UserAnalysis)

  GO4_TARGETNAME_FROM_FILE(tgt)
  
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  set(depend Go4Analysis ${ARG_DEPENDENCIES})

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  
  if(CMAKE_PROJECT_NAME STREQUAL "Go4")
     set(dict_depend ${depend})
  endif()

  add_library(${libname}${tgt} SHARED ${ARG_SOURCES})

  set_target_properties(${libname}${tgt} PROPERTIES LIBRARY_OUTPUT_NAME ${libname})

  target_link_libraries(${libname}${tgt} ${depend} ${ARG_LIBRARIES})

  target_include_directories(${libname}${tgt} PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR} ${ARG_INCLUDE_DIRS})

  target_compile_definitions(${libname}${tgt} PUBLIC Linux ${ARG_DEFINITIONS})

  ROOT_GENERATE_DICTIONARY(G__${libname}${tgt} ${ARG_HEADERS}
                          MODULE ${libname}${tgt}
                          LINKDEF ${ARG_LINKDEF}
                          DEPENDENCIES ${dict_depend}
                          NOINSTALL)

endfunction()
