# Find DABC library and includes

 set(DABC_FOUND 0)

if(NOT "$ENV{DABCSYS}" STREQUAL "")
   find_library(DABC_LIBRARY NAMES DabcBase PATHS
      $ENV{DABCSYS}/lib
      DOC "Searching DabcCore library"
   )
   
   if (DABC_LIBRARY)
      set(DABC_FOUND 1)
      set(DABC_INCLUDE_DIR $ENV{DABCSYS}/include)
      message(STATUS "Found DABC includes ${DABC_INCLUDE_DIR} and library ${DABC_LIBRARY}")
   endif()
endif()

mark_as_advanced(DABC_FOUND DABC_LIBRARY DABC_INCLUDE_DIR)

