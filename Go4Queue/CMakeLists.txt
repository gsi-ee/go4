set(Go4Queue_headers
   TGo4BufferQueue.h
   TGo4ObjectQueue.h
   TGo4Queue.h
)

set(Go4Queue_sources
   TGo4BufferQueue.cxx
   TGo4ObjectQueue.cxx
   TGo4Queue.cxx
)

GO4_INSTALL_HEADERS(${Go4Queue_headers})

GO4_SOURCES(Go4TaskHandler HEADERS ${Go4Queue_headers} SOURCES ${Go4Queue_sources})

add_executable(MainGo4Queue MainGo4Queue.cxx)
target_link_libraries(MainGo4Queue Go4Base Go4ThreadManager Go4TaskHandler)
