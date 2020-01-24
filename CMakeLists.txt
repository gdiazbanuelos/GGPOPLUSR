cmake_minimum_required (VERSION 3.10)
project (
    GGPOPLUSR
    VERSION 0.0.0
    DESCRIPTION "A netplay implementation and quality-of-life tool bundle for the Steam release of Guilty Gear XX Accent Core +R."
    HOMEPAGE_URL "https://github.com/adanducci/GGPOPLUSR"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(DearImgui 1 REQUIRED)
find_package(Detours 4 REQUIRED)
find_package(GGPO 1 REQUIRED)
find_package(ValveFileVDF 1 REQUIRED)

add_library(Game STATIC src/game/game.h src/game/game.cxx)
target_link_libraries(Game PRIVATE Detours GGPO)

add_executable(Launcher WIN32
    src/launcher/discovery.cxx
    src/launcher/discovery.hxx
    src/launcher/launcher.cxx
)
target_link_libraries(Launcher
	PRIVATE Detours
	PRIVATE ValveFileVDF
	PRIVATE "shlwapi"
)

add_library(Overlay STATIC src/overlay/overlay.cxx src/overlay/overlay.h)
target_link_libraries(Overlay PRIVATE DearImgui DearImgui_Win32 DearImgui_DX9 Game GGPO)

add_library(Sidecar SHARED src/sidecar/sidecar.cxx)
target_link_libraries(Sidecar PRIVATE Detours Game GGPO Overlay)
