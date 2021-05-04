set(Go4DProxies_headers
   TGo4AbstractInterface.h
   TGo4AnalysisProxy.h
   TGo4BrowserItem.h
   TGo4BrowserProxy.h
   TGo4DabcProxy.h
   TGo4HServProxy.h
   TGo4Interface.h
   TGo4RootBrowserProxy.h
   TGo4ServerProxy.h
   TGo4WidgetProxyBase.h
)

set(Go4DProxies_sources
   TGo4AbstractInterface.cxx
   TGo4AnalysisProxy.cxx
   TGo4BrowserItem.cxx
   TGo4BrowserProxy.cxx
   TGo4DabcProxy.cxx
   TGo4HServProxy.cxx
   TGo4Interface.cxx
   TGo4RootBrowserProxy.cxx
   TGo4ServerProxy.cxx
   TGo4WidgetProxyBase.cxx
)

GO4_INSTALL_HEADERS(MAKESUB ${Go4DProxies_headers})

GO4_SOURCES(Go4Gui HEADERS ${Go4DProxies_headers} SOURCES ${Go4DProxies_sources})
