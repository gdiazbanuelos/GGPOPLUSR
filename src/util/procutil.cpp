#include <"procutil.h">
#include <stdexcept>
#include <Windows.h>
#include <Psapi.h>

namespace util {

	bool get_module_bounds(const char* name, uintptr_t* start, uintptr_t* end) {
		const auto module = GetModuleHandle(name);
		if (module == nullptr) {
			return false;
		}

		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(info));
		*start = (uintptr_t)info.lpBaseOfDll;
		*end = *start + info.SizeOfImage;
		return true;
	}

	uintptr_t sigscan(const char* name, const char* pattern, const char* mask) {
		
		uintptr_t start, end;
		if (!get_module_bounds(name, &start, &end))
			throw std::runtime_error("Module not Loaded (?)");

		const auto last_scan = end - strlen(mask) - 1;

		for (auto addr = start; addr < last_scan; addr++) {
			for (size_t i = 0;; i++) {
				if (mask[i] == '\0') {
					return addr;
				}
				if (mask[i] != '?' && sig[i] != *(char*)(addr + i))
					break;
			}
		}
		throw std::runtime_error("Signature not found");
	}
	
}