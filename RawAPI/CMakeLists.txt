set(RawAPI_headers
   f_rawcli.h
   rawapigsi.h
   rawapin.h
   rawapitd-gsin.h
   rawapitd.h
   rawapplcli.h
   rawclin.h
   rawcommn.h
   rawdefn.h
   rawentn.h
   rawentserv.h
)

set(RawAPI_sources
   rawapin.c
   rawCliProcn.c
   rawProcn.c
   rawProcUn.c
   rconnect.c
)

GO4_INSTALL_HEADERS(${RawAPI_headers})

GO4_SOURCES(Go4Analysis SOURCES ${RawAPI_sources})

set_source_files_properties(${RawAPI_sources} PROPERTIES COMPILE_DEFINITIONS SYSTEM64)