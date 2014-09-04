# driver source files
SET(sources check_scarf.cpp)
# application header files
SET(headers check_scarf.h)
# locate the necessary dependencies
find_package( OpenCV REQUIRED )
# add the necessary include directories
INCLUDE_DIRECTORIES(.)
include_directories( ${OpenCV_INCLUDE_DIRS} )
# create the shared library
ADD_LIBRARY(check_scarf SHARED ${sources})
# link necessary libraries
TARGET_LINK_LIBRARIES(check_scarf ${OpenCV_LIBS})
INSTALL(TARGETS check_scarf
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib/iridrivers
        ARCHIVE DESTINATION lib/iridrivers)
INSTALL(FILES ${headers} DESTINATION include/iridrivers)
INSTALL(FILES ../Findcheck_scarf.cmake DESTINATION ${CMAKE_ROOT}/Modules/)
ADD_SUBDIRECTORY(examples)
