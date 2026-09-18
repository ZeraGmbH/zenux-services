set(SCRIPTS
    zenuxlog
    updatehtmls2llog
    all-zenux-services
    )

# just for IDE
add_custom_target(scripts SOURCES
    ${SCRIPTS}
    )

install(
    PROGRAMS ${SCRIPTS}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
