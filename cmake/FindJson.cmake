include(FindPackageHandleStandardArgs)

set(JSON_DIR CACHE PATH "Location of a checkout of nlohmann/json.")

if(NOT JSON_DIR)
	set(JSON_DIR "${PROJECT_SOURCE_DIR}/../json/single_include/nlohmann")
endif()

if(EXISTS "${JSON_DIR}/json.hpp")
	set(JSON_FOUND True)
	add_library(Json INTERFACE IMPORTED)
	target_include_directories(Json INTERFACE "${JSON_DIR}/")
else()
	set(JSON_FOUND False)
endif()