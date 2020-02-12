cmake_minimum_required (VERSION 3.10)
project (
    GGPOPLUSR
    VERSION 0.0.0
    DESCRIPTION "A netplay implementation and quality-of-life tool bundle for the Steam release of Guilty Gear XX Accent Core +R."
    HOMEPAGE_URL "https://github.com/adanducci/GGPOPLUSR"
)

set(CPACK_BINARY_NSIS ON CACHE BOOL "CPACK_BINARY_NSIS")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(DearImgui 1 REQUIRED)
find_package(Detours 4 REQUIRED)
find_package(GGPO 1 REQUIRED)
find_package(ValveFileVDF 1 REQUIRED)

add_library(Game STATIC src/game/game.h src/game/game.cxx)
target_link_libraries(Game PUBLIC Detours GGPO ValveFileVDF)

add_executable(Launcher WIN32
    src/launcher/discovery.cxx
    src/launcher/discovery.hxx
    src/launcher/launcher.cxx
    ${PROJECT_SOURCE_DIR}/resources/graphics/Launcher.rc # This .rc binds the .ico to the executable
)
target_link_libraries(Launcher
	PRIVATE Detours
	PRIVATE ValveFileVDF
	PRIVATE "shlwapi"
	PUBLIC Sidecar
)

add_library(Overlay STATIC src/overlay/overlay.cxx src/overlay/overlay.h)
target_link_libraries(Overlay PUBLIC DearImgui DearImgui_Win32 DearImgui_DX9 Game GGPO)

add_library(Sidecar SHARED src/sidecar/sidecar.cxx)
target_link_libraries(Sidecar PRIVATE Detours Game GGPO Overlay ValveFileVDF)

configure_file("default_config.vdf" "config.vdf" COPYONLY)

# Install targets for INSTALL project, CPack use this to know which files must be packaged
install(
  TARGETS Launcher Game Overlay Sidecar DearImgui DearImgui_Win32 DearImgui_DX9
  CONFIGURATIONS Debug
  RUNTIME DESTINATION "."
)
install(
  TARGETS Launcher Game Overlay Sidecar DearImgui DearImgui_Win32 DearImgui_DX9
  CONFIGURATIONS Release
  DESTINATION "."
)

# At configure step CMake defaults OUTPUT_NAME to var-NOTFOUND, so we have to set it here
set_target_properties(Launcher PROPERTIES OUTPUT_NAME Launcher)
get_target_property(EXE_TARGET_NAME Launcher OUTPUT_NAME)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_NAME} Dev Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} ${PROJECT_DESCRIPTION}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/resources/graphics/${EXE_TARGET_NAME}.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_SOURCE_DIR}/resources/graphics/${EXE_TARGET_NAME}.ico")
set(CPACK_NSIS_HELP_LINK "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(CPACK_NSIS_URL_INFO_ABOUT "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_NSIS_MENU_LINKS "${EXE_TARGET_NAME}.exe" "Start ${PROJECT_NAME}")

# Load packaging facilities, always after all set(CPACK*) options
include(CPack)

