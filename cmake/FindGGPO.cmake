include(FindPackageHandleStandardArgs)

set(GGPO_DIR CACHE PATH "Location of a checkout of GGPO.")

if(NOT GGPO_DIR)
	set(GGPO_DIR "${PROJECT_SOURCE_DIR}/../ggpo/")
endif()

if(EXISTS "${GGPO_DIR}/build/bin/x86/Debug/GGPO.dll")
	set(GGPO_FOUND True)
	add_library(GGPO STATIC IMPORTED)
	set_target_properties(GGPO PROPERTIES IMPORTED_LOCATION "${GGPO_DIR}/build/lib/x86/Debug/GGPO.lib")
	target_include_directories(GGPO INTERFACE "${GGPO_DIR}/src/include/")
else()
	message("GGPO not exists")
	set(GGPO_FOUND False)
endif()