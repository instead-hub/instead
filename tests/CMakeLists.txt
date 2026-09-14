IF(WITH_LUAJIT)
	FIND_PACKAGE(LuaJIT REQUIRED)
ELSE(WITH_LUAJIT)
	FIND_PACKAGE(Lua51 REQUIRED)
ENDIF(WITH_LUAJIT)

ADD_EXECUTABLE(instead_unit_tests
	unit/main.c
	unit/test_util.c
	unit/test_list.c
	unit/test_xoshiro.c
	unit/test_cache.c
	unit/test_utils.c
	unit/test_bits.c
	${CMAKE_SOURCE_DIR}/src/instead/util.c
	${CMAKE_SOURCE_DIR}/src/instead/list.c
	${CMAKE_SOURCE_DIR}/src/instead/xoshiro128.c
	${CMAKE_SOURCE_DIR}/src/instead/cache.c
	${CMAKE_SOURCE_DIR}/src/utils.c
	${CMAKE_SOURCE_DIR}/src/instead_bits.c
)
TARGET_INCLUDE_DIRECTORIES(instead_unit_tests PRIVATE
	${CMAKE_SOURCE_DIR}/src/instead
	${CMAKE_SOURCE_DIR}/src
	${LUA_INCLUDE_DIR}
)
TARGET_LINK_LIBRARIES(instead_unit_tests ${LUA_LIBRARIES})
IF(UNIX)
	TARGET_LINK_LIBRARIES(instead_unit_tests m)
ENDIF(UNIX)
ADD_TEST(NAME instead-unit COMMAND instead_unit_tests)

IF(STANDALONE)
	ADD_TEST(NAME instead-stage COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR})
	# run the staged binary: on macOS macosx_init() chdirs to the executable
	# directory, so the data dirs must be next to it
	ADD_TEST(NAME instead-smoke
		COMMAND ${CMAKE_SOURCE_DIR}/tests/smoke.sh ${STANDALONEDIR}/sdl-instead${CMAKE_EXECUTABLE_SUFFIX})
	SET_TESTS_PROPERTIES(instead-smoke PROPERTIES
		DEPENDS instead-stage
		WORKING_DIRECTORY ${STANDALONEDIR}
		TIMEOUT 60
	)
	ADD_TEST(NAME instead-integration
		COMMAND ${CMAKE_SOURCE_DIR}/tests/integration.sh ${STANDALONEDIR}/sdl-instead${CMAKE_EXECUTABLE_SUFFIX})
	SET_TESTS_PROPERTIES(instead-integration PROPERTIES
		DEPENDS instead-stage
		WORKING_DIRECTORY ${STANDALONEDIR}
		TIMEOUT 60
	)
ENDIF(STANDALONE)
