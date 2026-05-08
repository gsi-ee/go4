set(Go4Http_headers
   TGo4MsgList.h
   TGo4Sniffer.h
)

set(Go4Http_sources
   TGo4MsgList.cxx
   TGo4Sniffer.cxx
)

GO4_INSTALL_HEADERS(${Go4Http_headers})


GO4_STANDARD_LIBRARY(Go4Http
                     LINKDEF LinkDef.h
                     HEADERS ${Go4Http_headers}
                     SOURCES ${Go4Http_sources}
                     DEPENDENCIES Go4Base Go4ThreadManager Go4TaskHandler Go4AnalBase Go4Analysis
                     DEFINITIONS ${def_have_allowed}
                     LIBRARIES ${ROOT_RHTTP_LIBRARY} ${ROOT_RHTTPSniff_LIBRARY})

get_target_property(TARGET_INCS Go4Http INCLUDE_DIRECTORIES)

find_path(ROOT_SNIFFER_PATH
  NAMES TRootSniffer.h
  PATHS ${TARGET_INCS}
  NO_DEFAULT_PATH  # Forces search ONLY in the provided PATHS
)

if(ROOT_SNIFFER_PATH)
  file(READ "${ROOT_SNIFFER_PATH}/TRootSniffer.h" SNIFFER_CONTENT)
  if(SNIFFER_CONTENT MATCHES "SetAllowPostObject")
    target_compile_definitions(Go4Http PRIVATE __ROOT_HAVE_POST_ALLOWED__)
  endif()
endif()
