include(CTest)
include(Catch)

add_executable(ShotPlotTests
    test_smoke.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/StatisticsCalculator.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/MinimumEnclosingCircle.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/UnitConverter.cpp
)

target_include_directories(ShotPlotTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ShotPlotTests PRIVATE
    Catch2::Catch2WithMain
    Qt6::Core
    Qt6::Gui
)

catch_discover_tests(ShotPlotTests)
