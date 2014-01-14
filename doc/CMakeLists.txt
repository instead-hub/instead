SET(DOC_FILES
	index.html
	instead-manual-en.html
	instead-manual.html
	instead-manual.pdf
	instead-modules.pdf
	instead.txt
)

IF(SYSTEMWIDE)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${DOCDIR})
	INSTALL(FILES instead.6 DESTINATION ${MANDIR})
ELSEIF(STANDALONE)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${STANDALONEDIR}/doc)
ENDIF(SYSTEMWIDE)
