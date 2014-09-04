#edit the following line to add the librarie's header files
FIND_PATH(check_scarf_INCLUDE_DIR check_scarf.h /usr/include/iridrivers /usr/local/include/iridrivers)

FIND_LIBRARY(check_scarf_LIBRARY
    NAMES check_scarf
    PATHS /usr/lib /usr/local/lib /usr/local/lib/iridrivers) 

IF (check_scarf_INCLUDE_DIR AND check_scarf_LIBRARY)
   SET(check_scarf_FOUND TRUE)
ENDIF (check_scarf_INCLUDE_DIR AND check_scarf_LIBRARY)

IF (check_scarf_FOUND)
   IF (NOT check_scarf_FIND_QUIETLY)
      MESSAGE(STATUS "Found check_scarf: ${check_scarf_LIBRARY}")
   ENDIF (NOT check_scarf_FIND_QUIETLY)
ELSE (check_scarf_FOUND)
   IF (check_scarf_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find check_scarf")
   ENDIF (check_scarf_FIND_REQUIRED)
ENDIF (check_scarf_FOUND)

