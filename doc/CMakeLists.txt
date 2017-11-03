SET(DOC_FILES
	index.html
)

SET(GENERATED_DOC_FILES
	stead3.pdf
	modules3.pdf
	stead2.pdf
	stead2-modules.pdf
	stead2-en.html
	stead2.html
	instead.txt
)

FOREACH(FILE ${GENERATED_DOC_FILES})
	IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
		SET(DOC_FILES ${DOC_FILES} ${FILE})
	ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
		SET(MISSING_FILES ${MISSING_FILES} ${FILE})
	ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
ENDFOREACH(FILE ${GENERATED_DOC_FILES})

IF(NOT "${MISSING_FILES}" STREQUAL "")
	MESSAGE(WARNING "This distribution lacks some documentation files: ${MISSING_FILES}. These may be generated (network access and additional tools required, see doc/makepdfs), or you may use instead source tarball from sourceforge which has these files included")
ENDIF(NOT "${MISSING_FILES}" STREQUAL "")

IF(SYSTEMWIDE)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${DOCDIR})
	INSTALL(FILES instead.6 DESTINATION ${MANDIR}/man6)
ELSEIF(STANDALONE)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${STANDALONEDIR}/doc)
ENDIF(SYSTEMWIDE)
