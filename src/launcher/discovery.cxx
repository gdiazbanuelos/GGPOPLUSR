#include <windows.h>

#include <shlwapi.h>
#include <strsafe.h>
#include <winuser.h>

#include <vdf_parser.hpp>

int FindGuilty(LPWSTR szGuiltyDirectory, LPWSTR szGuiltyExePath) {
	DWORD dwDataRead = 1024;
	DWORD dwRegValueType;
	LSTATUS lQueryStatus;
	wchar_t szError[1024];
	const wchar_t* szGuiltySuffix = L"steamapps\\common\\Guilty Gear XX Accent Core Plus R";
	wchar_t szLibraries[8][1024];
	int nLibrariesUsed = 1;
	wchar_t szLibraryFolderVDFPath[1024];

	lQueryStatus = RegGetValueW(
		HKEY_CURRENT_USER,
		L"Software\\Valve\\Steam",
		L"SteamPath",
		RRF_RT_REG_SZ,
		&dwRegValueType,
		szLibraries[0],
		&dwDataRead
	);

	if (lQueryStatus != ERROR_SUCCESS) {
		StringCchPrintfW(szError, 1024, L"Query status: %d", lQueryStatus);
		MessageBoxW(NULL, szError, NULL, 0);
	}

	// Read additional libraries
	PathCombineW(szLibraryFolderVDFPath, szLibraries[0], L"steamapps\\libraryfolders.vdf");
	std::ifstream libraryFoldersFile(szLibraryFolderVDFPath);
	tyti::vdf::object libraryFoldersRoot = tyti::vdf::read(libraryFoldersFile);
	std::unordered_map<std::string, std::string>::iterator it;
	for (it = libraryFoldersRoot.attribs.begin(); it != libraryFoldersRoot.attribs.end(); ++it) {
		if (
			it->first.compare("TimeNextStatsReport") == 0 ||
			it->first.compare("ContentStatsID") == 0
		) {
			continue;
		}

		MultiByteToWideChar(
			CP_ACP,
			0,
			it->second.c_str(),
			-1,
			szLibraries[nLibrariesUsed],
			1024
		);
		nLibrariesUsed++;
	}

	for (int i = 0; i < nLibrariesUsed; i++) {
		PathCombineW(szGuiltyDirectory, szLibraries[i], szGuiltySuffix);
		if (PathIsDirectoryW(szGuiltyDirectory)) {
			PathCombineW(szGuiltyExePath, szGuiltyDirectory, L"GGXXACPR_Win.exe");
			return 0;
		}
	}

	return 1;
}
