set(Go4ObjectManager_headers
   TGo4AccessWrapper.h
   TGo4CanvasProxy.h
   TGo4DirProxy.h
   TGo4FolderProxy.h
   TGo4HStackProxy.h
   TGo4Iter.h
   TGo4LinkProxy.h
   TGo4ObjectManager.h
   TGo4ObjectProxy.h
   TGo4Proxy.h
   TGo4Slot.h
   TGo4TreeProxy.h
)

set(Go4ObjectManager_sources
   TGo4CanvasProxy.cxx
   TGo4DirProxy.cxx
   TGo4FolderProxy.cxx
   TGo4HStackProxy.cxx
   TGo4Iter.cxx
   TGo4LinkProxy.cxx
   TGo4ObjectManager.cxx
   TGo4ObjectProxy.cxx
   TGo4Proxy.cxx
   TGo4Slot.cxx
   TGo4TreeProxy.cxx
)

GO4_INSTALL_HEADERS(${Go4ObjectManager_headers})

GO4_SOURCES(Go4Gui HEADERS ${Go4ObjectManager_headers} SOURCES ${Go4ObjectManager_sources})
