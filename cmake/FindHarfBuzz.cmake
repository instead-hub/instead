# - Try to locate HarfBuzz
# This module defines:
#
#  HARFBUZZ_INCLUDE_DIR
#  HARFBUZZ_LIBRARY
#  HARFBUZZ_FOUND
#

FIND_PATH(HARFBUZZ_INCLUDE_DIR NAMES hb.h PATH_SUFFIXES harfbuzz)

FIND_LIBRARY(HARFBUZZ_LIBRARY NAMES harfbuzz)

IF(HARFBUZZ_INCLUDE_DIR AND HARFBUZZ_LIBRARY)
	SET(HARFBUZZ_FOUND TRUE)
ENDIF(HARFBUZZ_INCLUDE_DIR AND HARFBUZZ_LIBRARY)

IF(HARFBUZZ_FOUND)
	IF(NOT HarfBuzz_FIND_QUIETLY)
		MESSAGE(STATUS "Found HarfBuzz: -I${HARFBUZZ_INCLUDE_DIR}, ${HARFBUZZ_LIBRARY}")
	ENDIF(NOT HarfBuzz_FIND_QUIETLY)
ELSE(HARFBUZZ_FOUND)
	IF(HarfBuzz_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find HarfBuzz")
	ENDIF(HarfBuzz_FIND_REQUIRED)
ENDIF(HARFBUZZ_FOUND)

MARK_AS_ADVANCED(HARFBUZZ_INCLUDE_DIR HARFBUZZ_LIBRARY HARFBUZZ_FOUND)
