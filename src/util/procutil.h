#pragma once

#include <cstdint>

namespace util {

	//Gets the start and end of a module's memory address (needed for sigscan)
	bool get_module_bounds(const char* name, uintptr_t* start, uintptr_t* end);

	//Scans a byte pattern with a mask (do we need this?), returns the address of the begenning of the first result.
	uintptr_t sigscan(const char* name, const char* pattern, const char* mask);

}