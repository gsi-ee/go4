cmake_minimum_required(VERSION 3.10)
project(Go4ThreadManagerExample)
find_package(Go4)
include(${GO4_USE_FILE})

GO4_STANDARD_LIBRARY(Go4ThreadManagerExampleLib
                     NOINSTALL
                     LINKDEF Go4ThreadManagerExampleLinkDef.h
                     HEADERS TGo4TestRunnable.h TGo4TestThreadManager.h
                     SOURCES TGo4TestRunnable.cxx TGo4TestThreadManager.cxx
                     LIBRARIES ${Go4Base_LIBRARY} ${Go4ThreadManager_LIBRARY})

add_executable(Go4ThreadManagerExample MainGo4ThreadManagerExample.cxx)
target_link_libraries(Go4ThreadManagerExample ${ROOT_LIBRARIES} Go4ThreadManagerExampleLib)
