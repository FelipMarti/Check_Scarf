# create an example application
ADD_EXECUTABLE(check_scarf_test check_scarf_test.cpp)
# link necessary libraries
TARGET_LINK_LIBRARIES(check_scarf_test check_scarf)
