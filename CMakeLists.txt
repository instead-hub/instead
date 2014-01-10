PROJECT(instead)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

SET(INSTEAD_VERSION "1.9.2")

# options
OPTION(WITH_GTK "Use GTK file open dialog" ON)
OPTION(WITH_ICONV "Build with iconv support" ON)
OPTION(STANDALONE "Build for standalone run" OFF)

SET(BINDIR "bin" CACHE STRING "Where to install binaries")
SET(DATADIR "share/instead" CACHE STRING "Where to install binaries")
SET(STEADDIR "${DATADIR}/stead" CACHE STRING "Where to install stead files")
SET(THEMESDIR "${DATADIR}/themes" CACHE STRING "Where to install themes")
SET(GAMESDIR "${DATADIR}/games" CACHE STRING "Where to install games")
SET(ICONDIR "share/pixmaps" CACHE STRING "Where to install icons")
SET(DOCDIR "share/doc/instead" CACHE STRING "Where to install documentation")
SET(LANGDIR "${DATADIR}/lang" CACHE STRING "Where to install language files")
SET(MANDIR "share/man/man6" CACHE STRING "Where to install man pages")
SET(DESKTOPDIR "share/applications" CACHE STRING "Where to install .desktop files")

# depends
FIND_PACKAGE(Lua51 REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_mixer REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(SDL_ttf REQUIRED)

IF(WITH_ICONV)
ADD_DEFINITIONS(-D_HAVE_ICONV)
FIND_PATH(ICONV_INCLUDE_DIR iconv.h)
FIND_LIBRARY(ICONV_LIBRARY NAMES iconv libiconv c)
ENDIF(WITH_ICONV)

IF(WITH_GTK)
	FIND_PACKAGE(GTK2 REQUIRED)
	ADD_DEFINITIONS(
		-D_USE_GTK -D_USE_BROWSE
	)
ENDIF(WITH_GTK)

# sources
SET(INSTEAD_SRCS
	src/sdl-instead/SDL_anigif.c
	src/sdl-instead/SDL_gfxBlitFunc.c
	src/sdl-instead/SDL_rotozoom.c
	src/sdl-instead/cache.c
	src/sdl-instead/config.c
	src/sdl-instead/game.c
	src/sdl-instead/graphics.c
	src/sdl-instead/idf.c
	src/sdl-instead/input.c
	src/sdl-instead/instead.c
	src/sdl-instead/ioapi.c
	src/sdl-instead/lfs.c
	src/sdl-instead/main.c
	src/sdl-instead/menu.c
	src/sdl-instead/sound.c
	src/sdl-instead/themes.c
	src/sdl-instead/unpack.c
	src/sdl-instead/unzip.c
	src/sdl-instead/util.c
)

# platform-specific stuff
IF(APPLE)
	SET(INSTEAD_SRCS ${INSTEAD_SRCS} src/sdl-instead/macos.c)
ELSEIF(WIN32 AND WINCE)
	SET(INSTEAD_SRCS ${INSTEAD_SRCS} src/sdl-instead/wince.c src/sdl-instead/iowin32.c)
ELSEIF(WIN32 AND NOT WINCE)
	SET(INSTEAD_SRCS ${INSTEAD_SRCS} src/sdl-instead/windows.c src/sdl-instead/iowin32.c)
ELSEIF(UNIX)
	SET(INSTEAD_SRCS ${INSTEAD_SRCS} src/sdl-instead/unix.c)
ELSE(APPLE)
	MESSAGE(FATAL_ERROR "Your platform is not supported yet, sorry")
ENDIF(APPLE)

# flags
ADD_DEFINITIONS(
	-DVERSION=\"${INSTEAD_VERSION}\"
	-D_HAVE_ICONV
	-D_USE_UNPACK
)

IF(NOT STANDALONE)
	ADD_DEFINITIONS(
		-DLANG_PATH=\"${CMAKE_INSTALL_PREFIX}/${LANGDIR}/\"
		-DSTEAD_PATH=\"${CMAKE_INSTALL_PREFIX}/${STEADDIR}/\"
		-DGAMES_PATH=\"${CMAKE_INSTALL_PREFIX}/${GAMESDIR}/\"
		-DTHEMES_PATH=\"${CMAKE_INSTALL_PREFIX}/${THEMESDIR}/\"
		-DICON_PATH=\"${CMAKE_INSTALL_PREFIX}/${ICONDIR}/\"
	)
ELSE(NOT STANDALONE)
	ADD_DEFINITIONS(
		-DLANG_PATH=\"${CMAKE_SOURCE_DIR}/lang/\"
		-DSTEAD_PATH=\"${CMAKE_SOURCE_DIR}/stead/\"
		-DGAMES_PATH=\"${CMAKE_SOURCE_DIR}/games/\"
		-DTHEMES_PATH=\"${CMAKE_SOURCE_DIR}/themes/\"
		-DICON_PATH=\"${CMAKE_SOURCE_DIR}/icon/\"
	)
ENDIF(NOT STANDALONE)

# targets
INCLUDE_DIRECTORIES(
	${SDL_INCLUDE_DIR}
	${SDL_MIXER_INCLUDE_DIR}
	${SDL_IMAGE_INCLUDE_DIR}
	${SDL_TTF_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${GTK2_DEFINITOIONS}
	${GTK2_INCLUDE_DIRS}
	${ICONV_INCLUDE_DIR}
)

ADD_EXECUTABLE(sdl-instead ${INSTEAD_SRCS})

TARGET_LINK_LIBRARIES(sdl-instead
	${SDL_LIBRARY}
	${SDL_MIXER_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${SDL_TTF_LIBRARY}
	${ZLIB_LIBRARY}
	${LUA_LIBRARIES}
	${GTK2_LIBRARIES}
	${ICONV_LIBRARY}
)

# install
IF(NOT STANDALONE)
	INSTALL(TARGETS sdl-instead DESTINATION ${BINDIR})
	INSTALL(DIRECTORY stead/ DESTINATION ${STEADDIR} FILES_MATCHING PATTERN "*.lua")
	INSTALL(DIRECTORY doc/ DESTINATION ${DOCDIR} FILES_MATCHING PATTERN "*.txt" PATTERN "*.pdf" PATTERN "*.html" PATTERN "examples" EXCLUDE)
	INSTALL(DIRECTORY lang/ DESTINATION ${LANGDIR} FILES_MATCHING PATTERN "*.ini")
	INSTALL(FILES doc/instead.6 DESTINATION ${MANDIR})
	INSTALL(FILES icon/sdl_instead.png DESTINATION ${ICONDIR})
	INSTALL(FILES desktop/instead.desktop.in DESTINATION ${DESKTOPDIR} RENAME instead.desktop)
	FOREACH(GAME tutorial3)
		INSTALL(DIRECTORY games/${GAME} DESTINATION ${GAMESDIR} PATTERN ".svn" EXCLUDE)
	ENDFOREACH(GAME)
	FOREACH(THEME arctic bluesteel book clearlooks default fantasy original wide)
		INSTALL(DIRECTORY themes/${THEME} DESTINATION ${THEMESDIR} PATTERN ".svn" EXCLUDE)
	ENDFOREACH(THEME)
ENDIF(NOT STANDALONE)
