
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
#---GO4_STANDARD_LIBRARY(libname
#                           LINKDEF linkdef            : 
#                           HEADERS header1 header2    : 
#                           SOURCES src1 src2          : 
#)
#---------------------------------------------------------------------------------------------------
function(GO4_STANDARD_LIBRARY libname)
  cmake_parse_arguments(ARG "" "LINKDEF;DEPENDENCIES" "HEADERS;SOURCES" ${ARGN})

  ROOT_GENERATE_DICTIONARY(G__${libname} ${ARG_HEADERS}
                          MODULE ${libname}
                          LINKDEF ${ARG_LINKDEF}
                          DEPENDENCIES ${ARG_DEPENDENCIES})

  add_library(${libname} SHARED ${ARG_SOURCES} G__${libname}.cxx)

  add_dependencies(${libname} G__${libname} move_headers)

  target_link_libraries(${libname} ROOT::Core)
  
  target_include_directories(${libname} PRIVATE ${CMAKE_BINARY_DIR}/include)


endfunction()
