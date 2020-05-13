set(Go4Display_headers
   TGo4DisplayDrawerTimer.h
   TGo4Display.h
   TGo4DisplayLoggerTimer.h
)

set(Go4Display_sources
   TGo4Display.cxx
   TGo4DisplayDrawerTimer.cxx
   TGo4DisplayLoggerTimer.cxx
)

GO4_INSTALL_HEADERS(${Go4Display_headers})

GO4_SOURCES(Go4Gui SOURCES ${Go4Display_sources})
