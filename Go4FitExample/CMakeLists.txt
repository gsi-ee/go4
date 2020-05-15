cmake_minimum_required(VERSION 3.9)
project(Go4FitExample)
find_package(Go4)
include(${GO4_USE_FILE})

set(plain Example1 Example5 Example6 Example7 Example8 Example9 Example10 Example11 Example12)

foreach(ex ${plain})
  add_executable(${ex} ${ex}.cxx)
  target_link_libraries(${ex} ${ROOT_LIBRARIES} ${ROOT_Minuit_LIBRARY} ${ROOT_Spectrum_LIBRARY} ${Go4Fit_LIBRARY})
endforeach()

add_executable(Example2 Example2.cxx Example2Func.cxx)
target_link_libraries(Example2 ${ROOT_LIBRARIES} ${ROOT_Minuit_LIBRARY} ${ROOT_Spectrum_LIBRARY} ${Go4Fit_LIBRARY})

# add_executable(Example3 Example3.cxx Example3Func.f)
# target_link_libraries(Example3 ${ROOT_LIBRARIES} ${ROOT_Minuit_LIBRARY} ${ROOT_Spectrum_LIBRARY} ${Go4Fit_LIBRARY})

GO4_STANDARD_LIBRARY(Example4Model
                     LINKDEF Example4LinkDef.h 
                     HEADERS TModelTemplate.h 
                     SOURCES TModelTemplate.cxx
                     LIBRARIES ${ROOT_Minuit_LIBRARY} ${ROOT_Spectrum_LIBRARY} ${Go4Fit_LIBRARY})

add_executable(Example4 Example4.cxx)
target_link_libraries(Example4 ${ROOT_LIBRARIES} ${ROOT_Minuit_LIBRARY} ${ROOT_Spectrum_LIBRARY} ${Go4Fit_LIBRARY} Example4Model)
