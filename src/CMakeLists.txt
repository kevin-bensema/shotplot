# Generate version header from project() VERSION
configure_file(ShotPlotVersion.h.in ShotPlotVersion.h @ONLY)

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
    Widgets/ImageCropRotateDialog.cpp
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
    Graphics/CroppingToolItem.cpp
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
    ${CMAKE_CURRENT_BINARY_DIR}

)

# Link libraries
target_link_libraries(ShotPlot PRIVATE
    QX
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    libzip::zip
)

# Platform-specific settings
if(WIN32)
    set_target_properties(ShotPlot PROPERTIES WIN32_EXECUTABLE TRUE)

    find_program(WINDEPLOYQT windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT)
        add_custom_command(TARGET ShotPlot POST_BUILD
            COMMAND "${WINDEPLOYQT}"
                --no-compiler-runtime
                "$<TARGET_FILE:ShotPlot>"
            COMMENT "Running windeployqt..."
        )

        # Install rules for CPack packaging
        install(TARGETS ShotPlot QX DESTINATION .)

        # Run windeployqt first so Qt DLLs/plugins are in the install tree.
        # --compiler-runtime bundles the MSVC redistributable for end users.
        install(CODE "
            execute_process(
                COMMAND \"${WINDEPLOYQT}\"
                    --compiler-runtime
                    \"\${CMAKE_INSTALL_PREFIX}/ShotPlot.exe\"
            )
        ")

        # Install all vcpkg runtime DLLs (e.g. zip.dll, bz2.dll, zlib1.dll).
        # Grabbing the whole bin/ directory is safe - vcpkg never puts system DLLs there.
        file(GLOB VCPKG_RUNTIME_DLLS
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
        install(FILES ${VCPKG_RUNTIME_DLLS} DESTINATION .)
        # TODO: When app icon is available, add:
        #   set_target_properties(ShotPlot PROPERTIES RC_ICONS resources/windows/app.ico)
        #   (and create resources/windows/app.rc referencing the .ico)
    else()
        message(WARNING "windeployqt not found - Qt DLLs will not be deployed to the build directory")
    endif()
elseif(APPLE)
    set_target_properties(ShotPlot PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist
    )
endif()

if(NOT WIN32)
    install(TARGETS ShotPlot QX)
endif()
