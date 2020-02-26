include(FindPackageHandleStandardArgs)

set(JSON_DIR CACHE PATH "Location of a checkout of nlohmann/json.")

if(NOT JSON_DIR)
	set(JSON_DIR "${PROJECT_SOURCE_DIR}/../json/")
endif()

if(EXISTS "${JSON_DIR}/single_include/nlohmann/json.hpp")
	set(JSON_FOUND True)
	add_library(Json INTERFACE IMPORTED)
	target_include_directories(Json INTERFACE "${JSON_DIR}/single_include/nlohmann/")
else()
	set(JSON_FOUND False)
endif()