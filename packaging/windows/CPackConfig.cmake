# CPack / WiX configuration for the Windows MSI installer.
# Included from the root CMakeLists.txt (WIN32-only) before include(CPack).

set(CPACK_PACKAGE_NAME "ShotPlot")
set(CPACK_PACKAGE_VENDOR "ShotPlot")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ShotPlot - Shot group analysis tool")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ShotPlot")

# Version is inherited from project() VERSION
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Default generators on Windows - both run when invoking cpack with no -G flag
set(CPACK_GENERATOR "WIX;ZIP")

# Stable GUID - identifies this product across upgrades. NEVER change this.
set(CPACK_WIX_UPGRADE_GUID "6F63538C-4E5F-47DA-98FE-E4BB574E8B28")

# Start Menu shortcut
set(CPACK_PACKAGE_EXECUTABLES "ShotPlot" "ShotPlot")

# Output filename: ShotPlot-0.0.8-win64.msi
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-win64")

set(CPACK_WIX_LICENSE_RTF "${CMAKE_SOURCE_DIR}/packaging/windows/LICENSE.rtf")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# TODO: When app icon is available, uncomment:
# set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/windows/app.ico")
