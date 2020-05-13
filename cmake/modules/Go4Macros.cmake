
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
#                           DEFINITIONS def1 def2      : library definitions 
#)
#---------------------------------------------------------------------------------------------------
function(GO4_STANDARD_LIBRARY libname)
  cmake_parse_arguments(ARG "" "LINKDEF" "HEADERS;SOURCES;DEPENDENCIES;DEFINITIONS" ${ARGN})

  ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS}
                          MODULE ${libname}
                          LINKDEF ${ARG_LINKDEF}
                          DEPENDENCIES ${ARG_DEPENDENCIES})

  add_library(${libname} SHARED ${ARG_SOURCES} G__${libname}.cxx)

  add_dependencies(${libname} G__${libname} move_headers)

#  target_link_libraries(${libname} ROOT::Core)

  target_link_libraries(${libname} ${LIBS_BASESET})
  
  target_include_directories(${libname} PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR})
  
  if (ARG_DEPENDENCIES)
    target_link_libraries(${libname} ${ARG_DEPENDENCIES})
  endif()
  
  if (ARG_DEFINITIONS)
    foreach(def ${ARG_DEFINITIONS})
      target_compile_definitions(${libname} PRIVATE ${def})
    endforeach()
  endif()

endfunction()


#---------------------------------------------------------------------------------------------------
#---GO4_USER_ANALYSIS(
#                     LINKDEF linkdef            : 
#                     HEADERS header1 header2    : 
#                     SOURCES src1 src2          : 
#                     DEPENDENCIES lib1 lib2     : dependend go4 libraries
#                     DEFINITIONS def1 def2      : library definitions 
#)
#---------------------------------------------------------------------------------------------------
function(GO4_USER_ANALYSIS)
  cmake_parse_arguments(ARG "" "LINKDEF" "HEADERS;SOURCES;DEPENDENCIES;DEFINITIONS" ${ARGN})
  
  set(libname Go4UserAnalysis)
  
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  set(depend Go4Analysis ${ARG_DEPENDENCIES})

  add_definitions(-DLinux ${ARG_DEFINITIONS})

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS}
                          MODULE ${libname}
                          LINKDEF ${ARG_LINKDEF}
                          DEPENDENCIES ${depend})

  add_library(${libname} SHARED ${ARG_SOURCES} G__${libname}.cxx)
  
  add_dependencies(${libname} G__${libname} move_headers)

  target_link_libraries(${libname} ${depend})
  
  target_include_directories(${libname} PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()
