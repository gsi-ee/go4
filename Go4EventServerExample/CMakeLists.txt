cmake_minimum_required(VERSION 3.9)
project(Go4ExampleSimple)
find_package(Go4)
include(${GO4_USE_FILE})

GO4_USER_ANALYSIS(
   NAME
      Go4EventServerExampleLib
   LINKDEF
      Go4EventServerExampleLinkDef.h
   HEADERS
      TGo4SimpleEvent.h
      TGo4SimpleEventProcessor.h
      TGo4SimpleSubEvent.h
   SOURCES
      TGo4SimpleEvent.cxx
      TGo4SimpleEventProcessor.cxx
      TGo4SimpleSubEvent.cxx
   LIBRARIES
      ROOT::Core
)

add_executable(MainGo4EventServerExample MainGo4EventServerExample.cxx)
target_link_libraries(MainGo4EventServerExample ${ROOT_LIBRARIES} Go4EventServerExampleLib)