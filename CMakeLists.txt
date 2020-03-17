cmake_minimum_required (VERSION 3.10)
project (
    GGPOPLUSR
    VERSION 0.0.0.0
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
find_package(Json 1 REQUIRED)
find_program(VC_REDIST_X86_EXE 1 NAMES "VC_redist.x86.exe" PATHS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Redist/MSVC/14.24.28127")

# build time in UTC ISO 8601. https://stackoverflow.com/a/31401540
configure_file("${PROJECT_SOURCE_DIR}/cmake/build_datetime.cmake" ${CMAKE_BINARY_DIR}/build_datetime.cmake COPYONLY)
ADD_CUSTOM_TARGET (
    build_datetime
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/build_datetime.cmake
    ADD_DEPENDENCIES ${CMAKE_BINARY_DIR}/build_datetime.cmake)
    
add_library(Game STATIC src/game/game.h src/game/game.cxx src/game/scripting.h src/game/syncthread.cxx)
target_link_libraries(Game PUBLIC Detours GGPO ValveFileVDF "ws2_32" "Mswsock" "AdvApi32")

add_executable(Launcher WIN32
    src/launcher/discovery.cxx
    src/launcher/discovery.hxx
    src/launcher/utils.cxx
    src/launcher/utils.hxx
    src/launcher/launcher.cxx
    ${CMAKE_BINARY_DIR}/resources/rc/Launcher.rc # This .rc binds the icon and the versioninfo to the executable
)
target_link_libraries(Launcher
	PRIVATE Detours
	PRIVATE ValveFileVDF
	PRIVATE Json
	PRIVATE "shlwapi"
	PRIVATE "winhttp"
	PUBLIC Sidecar
)

add_library(Overlay STATIC src/overlay/overlay.cxx src/overlay/overlay.h)
target_link_libraries(Overlay PUBLIC DearImgui DearImgui_Win32 DearImgui_DX9 Game GGPO)

add_library(Sidecar SHARED src/sidecar/sidecar.cxx src/sidecar/ID3D9Wrapper.cxx src/sidecar/ID3D9Wrapper.h src/sidecar/ID3D9Wrapper_Device.cxx src/sidecar/ID3D9Wrapper_Device.h ${CMAKE_BINARY_DIR}/resources/rc/Sidecar.rc)
target_link_libraries(Sidecar PRIVATE Detours Game GGPO Overlay ValveFileVDF)

target_compile_definitions(Launcher PRIVATE -D_UNICODE -DUNICODE)

configure_file("default_config.vdf" "config.vdf" COPYONLY)

add_dependencies(Launcher build_datetime)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# Install targets for INSTALL project, CPack use this to know which files must be packaged
install(
  TARGETS Launcher Game Overlay Sidecar DearImgui DearImgui_Win32 DearImgui_DX9
  CONFIGURATIONS Debug
  RUNTIME DESTINATION "."
)
install(
  TARGETS Launcher Game Overlay Sidecar DearImgui DearImgui_Win32 DearImgui_DX9
  CONFIGURATIONS Release
  RUNTIME DESTINATION "."
)
install(
  FILES "default_config.vdf"
  CONFIGURATIONS Debug Release
  RUNTIME DESTINATION "."
  RENAME "config.vdf"
)
install(
  FILES "${GGPO_DIR}/build/bin/x86/Debug/GGPO.dll"
  CONFIGURATIONS Debug
  RUNTIME DESTINATION "."
)
install(
  FILES "${GGPO_DIR}/build/bin/x86/Release/GGPO.dll"
  CONFIGURATIONS Release
  RUNTIME DESTINATION "."
)
install(PROGRAMS ${VC_REDIST_X86_EXE} DESTINATION redist)

# At configure step CMake defaults OUTPUT_NAME to var-NOTFOUND, so we have to set it here
set_target_properties(Launcher PROPERTIES OUTPUT_NAME Launcher)
get_target_property(EXE_TARGET_NAME Launcher OUTPUT_NAME)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "The ${PROJECT_NAME} Dev Team")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} ${PROJECT_DESCRIPTION}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/resources/rc/${EXE_TARGET_NAME}.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${EXE_TARGET_NAME}.exe") #This will make the icon appear in "control panel/programs"
set(CPACK_NSIS_HELP_LINK "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(CPACK_NSIS_URL_INFO_ABOUT "${CMAKE_PROJECT_HOMEPAGE_URL}")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MENU_LINKS "${EXE_TARGET_NAME}.exe" "Start ${PROJECT_NAME}")
set(CPACK_EXE_TARGET_NAME "${EXE_TARGET_NAME}")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  !include LogicLib.nsh
  ExecWait '$INSTDIR\\\\redist\\\\vc_redist.x86.exe /install /passive /norestart'

  Var /GLOBAL ggxxacprdir
  ReadRegStr $ggxxacprdir HKLM \\\"SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\Steam App 348550\\\" \\\"InstallLocation\\\"

  \\\${If} $ggxxacprdir == ''
    SetRegView 64
    ReadRegStr $ggxxacprdir HKLM \\\"SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\Steam App 348550\\\" \\\"InstallLocation\\\"
    \\\${If} $ggxxacprdir == ''
      MessageBox MB_OK 'Could not find GGXXACPR installation directory. Please make sure GGXXACPR is installed, and move $INSTDIR\\\\GGPO.dll to your GGXXACPR install folder.'
    \\\${EndIf}
  \\\${EndIf}

  \\\${If} $ggxxacprdir != ''
    CopyFiles $INSTDIR\\\\GGPO.dll $ggxxacprdir\\\\GGPO.dll
    Delete $INSTDIR\\\\GGPO.dll
  \\\${EndIf}

  IfFileExists $INSTDIR\\\\config.vdf.bak exists notexists
  exists:
    DetailPrint \\\"A backup of the configuration file has been found and will be restored.\\\"
    Delete $INSTDIR\\\\config.vdf
    Rename $INSTDIR\\\\config.vdf.bak $INSTDIR\\\\config.vdf
    Goto next
  notexists:
  next:
")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  MessageBox MB_YESNO \\\"Do you want to delete your ${PROJECT_NAME} configuration file?\\\" /SD IDNO IDYES del IDNO nodel
  del:
    DetailPrint \\\"The ${PROJECT_NAME} configuration file will be deleted.\\\"
    Goto afterconfdelete
  nodel:
    DetailPrint \\\"The ${PROJECT_NAME} configuration file will be backed up.\\\"
    Delete $INSTDIR\\\\config.vdf.bak
    Rename $INSTDIR\\\\config.vdf $INSTDIR\\\\config.vdf.bak
  afterconfdelete:

  Delete \\\"$DESKTOP\\\\${PROJECT_NAME} ${EXE_TARGET_NAME}.lnk\\\"
")

configure_file(
  ${PROJECT_SOURCE_DIR}/resources/rc/Launcher.rc.in
  ${CMAKE_BINARY_DIR}/resources/rc/Launcher.rc
  @ONLY)
configure_file(
  ${PROJECT_SOURCE_DIR}/resources/rc/Launcher_rc_resource.h
  ${CMAKE_BINARY_DIR}/resources/rc/Launcher_rc_resource.h
  COPYONLY)
configure_file(
  ${PROJECT_SOURCE_DIR}/resources/rc/Launcher.ico
  ${CMAKE_BINARY_DIR}/resources/rc/Launcher.ico
  COPYONLY)

configure_file(
  ${PROJECT_SOURCE_DIR}/resources/rc/Sidecar.rc.in
  ${CMAKE_BINARY_DIR}/resources/rc/Sidecar.rc
  @ONLY)
configure_file(
  ${PROJECT_SOURCE_DIR}/resources/rc/Sidecar_rc_resource.h
  ${CMAKE_BINARY_DIR}/resources/rc/Sidecar_rc_resource.h
  COPYONLY)

# Load packaging facilities, always after all set(CPACK*) options
include(CPack)
