# Collect source files by subdirectory
set(APP_SOURCES
    App/MainWindow.cpp
)

set(CORE_SOURCES
    Core/ShotGroupDocument.cpp
    Core/StatisticsCalculator.cpp
    Core/MinimumEnclosingCircle.cpp
    Core/UnitConverter.cpp
)

set(STATES_SOURCES
    States/WorkflowState.cpp
    States/SetCaliberState.cpp
    States/ScaleFactorState.cpp
    States/POAState.cpp
    States/MarkImpactsState.cpp
    States/VisualizationState.cpp
)

set(WIDGETS_SOURCES
    Widgets/WorkflowToolbar.cpp
    Widgets/PerStateToolbar.cpp
    Widgets/TargetView.cpp
    Widgets/StatisticsPanel.cpp
    Widgets/CaliberDialog.cpp
    Widgets/GraphicsSettingsDialog.cpp
    Widgets/PlaqueSettingsWidget.cpp
)

set(SERVICES_SOURCES
    Services/GraphicsSettingsService.cpp
)

set(GRAPHICS_SOURCES
    Graphics/TargetScene.cpp
    Graphics/ImpactGlyphItem.cpp
    Graphics/POAGlyphItem.cpp
    Graphics/CentroidGlyphItem.cpp
    Graphics/GroupCircleItem.cpp
    Graphics/ScaleLineItem.cpp
    Graphics/StatisticsPlaqueItem.cpp
)

set(IO_SOURCES
    IO/DocumentSerializer.cpp
    IO/ImageExporter.cpp
)

set(COMMANDS_SOURCES
    Commands/AddImpactCommand.cpp
    Commands/RemoveImpactCommand.cpp
    Commands/ClearImpactsCommand.cpp
)

# Combine all sources
set(SHOTPLOT_SOURCES
    main.cpp
    ${APP_SOURCES}
    ${CORE_SOURCES}
    ${STATES_SOURCES}
    ${WIDGETS_SOURCES}
    ${SERVICES_SOURCES}
    ${GRAPHICS_SOURCES}
    ${IO_SOURCES}
    ${COMMANDS_SOURCES}
)

# Qt resources
set(SHOTPLOT_RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

# Create executable
add_executable(ShotPlot
    ${SHOTPLOT_SOURCES}
    ${SHOTPLOT_RESOURCES}
)

# Include directories
target_include_directories(ShotPlot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBZIP_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ShotPlot PRIVATE
    QX
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${LIBZIP_LIBRARIES}
)

# Platform-specific settings
if(WIN32)
    set_target_properties(ShotPlot PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(APPLE)
    set_target_properties(ShotPlot PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist
    )
endif()
