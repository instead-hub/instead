FILE(GLOB DOC_FILES 
	instead.txt
	index.html
	instead-manual-en.html 
	instead-manual.html
	instead-manual.pdf
	instead-modules.pdf
)

IF(SYSTEMWIDE)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${DOCDIR})
	INSTALL(FILES instead.6 DESTINATION ${MANDIR}/man6)
ELSEIF(STANDALONE)
	INSTALL(FILES ${DOC_FILES} DESTINATION ${STANDALONEDIR}/doc)
ENDIF(SYSTEMWIDE)
