cmake_minimum_required (VERSION 3.10)
project (
    GGPOPLUSR
    VERSION 0.0.1
    DESCRIPTION "GGAC+R GGPO-BASED CASTER"
    HOMEPAGE_URL "https://github.com/Labreezy/GGPOPLUSR"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Detours 4 REQUIRED)

add_executable(Launcher WIN32 src/launcher/launcher.cxx)
target_link_libraries(Launcher
	PRIVATE Detours
	PRIVATE "shlwapi"
)

add_library(Sidecar SHARED src/sidecar/sidecar.cxx)
target_link_libraries(Sidecar PRIVATE Detours)
