set(SOURCES
    micro-controller-io/atmelsysctrl.cpp
    scpi-interfaces/clampinterface.cpp
    scpi-interfaces/senseinterface.cpp
    scpi-interfaces/systeminterface.cpp
    scpi-interfaces/accumulatorinterface.cpp
    settings/ctrlsettings.cpp
    settings/accumulatorsettings.cpp
    senserange.cpp
    sensechannel.cpp
    adjustment.cpp
    mt310s2systeminfo.cpp
    clampjustdata.cpp
    adjflash.cpp
    clamp.cpp
    adjxml.cpp
    mt310s2d.cpp
    mt310s2justdata.cpp
    )

set(HEADER
    micro-controller-io/atmelsysctrl.h
    scpi-interfaces/clampinterface.h
    scpi-interfaces/senseinterface.h
    scpi-interfaces/systeminterface.h
    scpi-interfaces/accumulatorinterface.h
    settings/ctrlsettings.h
    settings/accumulatorsettings.h
    senserange.h
    sensechannel.h
    adjflash.h
    adjxml.h
    adjustment.h
    mt310s2systeminfo.h
    clampjustdata.h
    clamp.h
    mt310s2d.h
    mt310s2dglobal.h
    mt310s2justdata.h
    )

add_library(mt310s2d-lib STATIC
    ${HEADER}
    ${SOURCES}
    )

target_include_directories(mt310s2d-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/micro-controller-io>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
    )

if(${Systemd_FOUND})
    target_link_libraries(mt310s2d-lib
        PRIVATE
        systemd
    )
endif()

target_link_libraries(mt310s2d-lib
    PUBLIC
    Qt5::Core
    Qt5::Xml
    Qt5::Network
    Zera::zera-xmlconfig
    Zera::zera-i2c-devices
    Zera::zeraprotobuf
    zenuxservicecommon
    )

