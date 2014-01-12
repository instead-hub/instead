IF(NOT STANDALONE)
	SET(DOC_FILES
		index.html
		instead-manual-en.html
		instead-manual.html
		instead-manual.pdf
		instead-modules.pdf
		instead.txt
	)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${DOCDIR})
	INSTALL(FILES instead.6 DESTINATION ${MANDIR})
ENDIF(NOT STANDALONE)
