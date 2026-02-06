include(CTest)
include(Catch)

add_executable(ShotPlotTests
    test_smoke.cpp
    test_document_roundtrip.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/ShotGroupDocument.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/StatisticsCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/MinimumEnclosingCircle.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/UnitConverter.cpp
    ${CMAKE_SOURCE_DIR}/src/IO/DocumentSerializer.cpp
)

target_include_directories(ShotPlotTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${LIBZIP_INCLUDE_DIRS}
)

target_link_libraries(ShotPlotTests PRIVATE
    Catch2::Catch2WithMain
    Qt6::Core
    Qt6::Gui
    ${LIBZIP_LIBRARIES}
)

catch_discover_tests(ShotPlotTests)
