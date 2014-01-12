# - Try to locate SDL2_image
# This module defines:
#
#  SDL2IMAGE_INCLUDE_DIR
#  SDL2IMAGE_LIBRARY
#  SDL2IMAGE_FOUND
#

FIND_PATH(SDL2IMAGE_INCLUDE_DIR NAMES SDL_image.h PATH_SUFFIXES SDL2)

FIND_LIBRARY(SDL2IMAGE_LIBRARY NAMES SDL2_image)

IF(SDL2IMAGE_INCLUDE_DIR AND SDL2IMAGE_LIBRARY)
	SET(SDL2IMAGE_FOUND TRUE)
ENDIF(SDL2IMAGE_INCLUDE_DIR AND SDL2IMAGE_LIBRARY)

IF(SDL2IMAGE_FOUND)
	IF(NOT SDL2_image_FIND_QUIETLY)
		MESSAGE(STATUS "Found SDL2_image: -I${SDL2IMAGE_INCLUDE_DIR}, ${SDL2IMAGE_LIBRARY}")
	ENDIF(NOT SDL2_image_FIND_QUIETLY)
ELSE(SDL2IMAGE_FOUND)
	IF(SDL2_image_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find SDL2_image")
	ENDIF(SDL2_image_FIND_REQUIRED)
ENDIF(SDL2IMAGE_FOUND)

MARK_AS_ADVANCED(SDL2IMAGE_INCLUDE_DIR SDL2IMAGE_LIBRARY SDL2IMAGE_FOUND)
