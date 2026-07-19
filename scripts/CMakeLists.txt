set(SCRIPTS
    zenuxlog
    journal2html.py
    )

add_custom_target(scripts SOURCES
    ${SCRIPTS}
    )

install(
    PROGRAMS ${SCRIPTS}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
