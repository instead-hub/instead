PROJECT(instead)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

IF("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	MESSAGE(FATAL_ERROR "In-source builds are not allowed. Please read ./INSTALL")
ENDIF("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# global constants
SET(INSTEAD_VERSION "3.0.2")

# options
OPTION(WITH_SDL2 "Use SDL2 instead of SDL 1" OFF)
OPTION(WITH_GTK2 "Use GTK2 file open dialog" ON)
OPTION(WITH_GTK3 "Use GTK3 file open dialog" OFF)
OPTION(WITH_ICONV "Build with iconv support" ON)

# Note: there are three build modes for instead:
# - SYSTEMWIDE
#     use it if you want to install it into your (unix) system
# - STANDALONE
#     used to generate standalone relocatable package which
#     may be run from anywhere. `install' target stages all
#     required files in ${STANDALONEDIR} for you
# - neither (both options disabled)
#     instead may be run from build directory, useful for
#     development
OPTION(SYSTEMWIDE "Build for systemwide install" ON)
OPTION(STANDALONE "Build for standalone package creation" OFF)

IF(WITH_GTK2 AND WITH_GTK3)
	MESSAGE(WARNING "WITH_GTK2 and WITH_GTK3 are mutually exclusive, disabling WITH_GTK2")
	SET(WITH_GTK2 OFF)
ENDIF(WITH_GTK2 AND WITH_GTK3)

IF(SYSTEMWIDE AND STANDALONE)
	MESSAGE(WARNING "Both STANDALONE and SYSTEMWIDE specified, disabling SYSTEMWIDE")
	SET(SYSTEMWIDE OFF)
ENDIF(SYSTEMWIDE AND STANDALONE)

SET(BINDIR "bin" CACHE STRING "Where to install binaries")
SET(DATADIR "share/instead" CACHE STRING "Where to install data files")
SET(STEADDIR "${DATADIR}/stead" CACHE STRING "Where to install stead files")
SET(THEMESDIR "${DATADIR}/themes" CACHE STRING "Where to install themes")
SET(GAMESDIR "${DATADIR}/games" CACHE STRING "Where to install games")
SET(ICONDIR "share/pixmaps" CACHE STRING "Where to install icons")
SET(DOCDIR "share/doc/instead" CACHE STRING "Where to install documentation")
SET(LANGDIR "${DATADIR}/lang" CACHE STRING "Where to install language files")
SET(MANDIR "share/man" CACHE STRING "Where to install man pages")
SET(DESKTOPDIR "share/applications" CACHE STRING "Where to install .desktop files")

SET(STANDALONEDIR "${PROJECT_BINARY_DIR}/standalone" CACHE STRING "Where to stage stanalone package")

# subdirectories
ADD_SUBDIRECTORY(desktop)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(games)
ADD_SUBDIRECTORY(icon)
ADD_SUBDIRECTORY(lang)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(stead)
ADD_SUBDIRECTORY(themes)
