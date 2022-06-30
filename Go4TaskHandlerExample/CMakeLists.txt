cmake_minimum_required(VERSION 3.9)
project(Go4TaskHandlerExample)
find_package(Go4)
include(${GO4_USE_FILE})

set(lib_headers
   TGo4ComAction1.h
   TGo4ComAction2.h
   TGo4ControllerRunnable.h
   TGo4ExampleApplication.h
   TGo4ExampleClient.h
   TGo4ExampleClientStatus.h
   TGo4ExampleCommand.h
   TGo4ExampleCommandList.h
   TGo4ExampleController.h
   TGo4ExampleServer.h
   TGo4LoggingRunnable.h
   TGo4MainRunnable.h
   TGo4WatchRunnable.h
)

set(lib_sources
   TGo4ComAction1.cxx
   TGo4ComAction2.cxx
   TGo4ControllerRunnable.cxx
   TGo4ExampleApplication.cxx
   TGo4ExampleClient.cxx
   TGo4ExampleClientStatus.cxx
   TGo4ExampleCommand.cxx
   TGo4ExampleCommandList.cxx
   TGo4ExampleController.cxx
   TGo4ExampleServer.cxx
   TGo4LoggingRunnable.cxx
   TGo4MainRunnable.cxx
   TGo4WatchRunnable.cxx
)

GO4_STANDARD_LIBRARY(Go4TaskHandlerExampleLib
                     NOINSTALL
                     LINKDEF Go4TaskHandlerExampleLinkDef.h
                     HEADERS ${lib_headers}
                     SOURCES ${lib_sources}
                     LIBRARIES ${Go4Base_LIBRARY} ${Go4ThreadManager_LIBRARY} ${Go4TaskHandler_LIBRARY} ROOT::Core)

add_executable(MainGo4TaskHandlerClient MainGo4TaskHandlerClient.cxx)
target_link_libraries(MainGo4TaskHandlerClient ${ROOT_LIBRARIES} Go4TaskHandlerExampleLib)

add_executable(MainGo4TaskHandlerServer MainGo4TaskHandlerServer.cxx)
target_link_libraries(MainGo4TaskHandlerServer ${ROOT_LIBRARIES} Go4TaskHandlerExampleLib)