include(GenerateExportHeader)

# QX library version (independent of the parent project)
set(QX_VERSION 0.1.0)
set(QX_SOVERSION 0)

# Collect sources
set(QX_SOURCES
    Src/QService.cpp
    Src/QServiceLocator.cpp
    Src/Services.cpp
    Src/QStylingService.cpp
    Src/SvgIconEngine.cpp
)

set(QX_HEADERS
    Inc/QX/QService.h
    Inc/QX/Services.h
    Inc/QX/QStylingService.h
)

set(QX_INTERNAL_HEADERS
    Src/QServiceLocator.h
    Src/SvgIconEngine.h
)

add_library(QX SHARED
    ${QX_SOURCES}
    ${QX_HEADERS}
    ${QX_INTERNAL_HEADERS}
)

set_target_properties(QX PROPERTIES
    VERSION   ${QX_VERSION}
    SOVERSION ${QX_SOVERSION}
)

# Generate export header into Inc/QX/Export.h
generate_export_header(QX
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/Inc/QX/Export.h
)

# Include directories
target_include_directories(QX
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
)

# Link Qt modules
target_link_libraries(QX
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::Xml
)

