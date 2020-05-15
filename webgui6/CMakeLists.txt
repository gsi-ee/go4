set(webgui6_headers
      TWebCanvasFull.h
      TWebGuiFactoryFull.h
      TWebMenuItem.h
      TWebPadOptions.h
)

set(webgui6_sources
      TWebCanvasFull.cxx
      TWebGuiFactoryFull.cxx
      TWebMenuItem.cxx
      TWebPadOptions.cxx
)

GO4_INSTALL_HEADERS(${webgui6_headers})

GO4_STANDARD_LIBRARY(WebGui6Full
                     LINKDEF LinkDef.h
                     HEADERS ${webgui6_headers}
                     SOURCES ${webgui6_sources}
                     LIBRARIES ${ROOT_Core_LIBRARY} ${ROOT_Gpad_LIBRARY} ${ROOT_RHTTP_LIBRARY} ${ROOT_WebGui6_LIBRARY})
