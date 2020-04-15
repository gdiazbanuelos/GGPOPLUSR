#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include "json.hpp"

#include <windows.h>
#include <shellapi.h>

#include <shlwapi.h>
#include <strsafe.h>
#include <winuser.h>
#include <thread>
#include <future>
#include <chrono>

#include <detours.h>

#include "discovery.hxx"
#include "utils.hxx"

#include "../sidecar/sidecar.h"
#include "build_datetime.h"

const std::wstring GITHUB_API_BASE_URI = L"api.github.com";
const std::wstring GITHUB_API_LATEST_RELEASE_ENDPOINT = L"repos/adanducci/GGPOPLUSR/releases/latest";
const std::wstring HTTP_USER_AGENT = L"GGPOPLUSR";
const std::wstring NEW_VERSION_AVAILABLE_TITLE = L"New version available";
const std::wstring NEW_VERSION_AVAILABLE_MESSAGE = L"A new version of GGPOPLUSR is available.\nDo you want to download it?";
const std::string LATEST_RELEASE_URI = "https://github.com/adanducci/GGPOPLUSR/releases/latest";
const DWORD REQUEST_TIMEOUT = 500;

using json = nlohmann::json;

void FindConfigFile(LPSTR szConfigPathA) {
	char szCwdBuffer[1024] = { 0 };
	GetCurrentDirectoryA(1024, szCwdBuffer);
	PathCombineA(szConfigPathA, szCwdBuffer, "config.vdf");
}

void FindSidecar(LPSTR szDllPathA) {
	char szCwdBuffer[1024] = { 0 };
	GetCurrentDirectoryA(1024, szCwdBuffer);
	PathCombineA(szDllPathA, szCwdBuffer, "Sidecar.dll");
}

void CreateAppIDFile(LPWSTR szGuiltyDirectory) {
	wchar_t szAppIDPath[1024] = { 0 };
	DWORD nBytesWritten = 0;

	PathCombine(szAppIDPath, szGuiltyDirectory, L"steam_appid.txt");

	HANDLE hAppIDHandle = CreateFile(
		szAppIDPath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);


	if (hAppIDHandle != INVALID_HANDLE_VALUE) {
		// Didn't exist, auto-created the file.
		WriteFile(hAppIDHandle, "348550", 6, &nBytesWritten, NULL);
		if (nBytesWritten != 6) {
			// Error!
		}
		CloseHandle(hAppIDHandle);
	}
}

void CreateGuiltyGearProcess(
	LPWSTR szGuiltyDirectory,
	LPWSTR szGuiltyExePath,
	LPSTR szSidecarDllPathA,
	LPSTR szSidecarConfigPathA
) {
	wchar_t szErrorString[1024] = { 0 };
	DWORD dwError;
	LPCSTR dllsToLoad[1] = { szSidecarDllPathA };
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	SetLastError(0);

	if (
		!DetourCreateProcessWithDllsW(
			szGuiltyExePath,
			NULL,
			NULL,
			NULL,
			TRUE,
			CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
			NULL,
			szGuiltyDirectory,
			&si,
			&pi,
			1,
			dllsToLoad,
			NULL
		)) {
		dwError = GetLastError();
		StringCchPrintf(szErrorString, 1024, L"DetourCreateProcessWithDllEx failed: %d", dwError);
		MessageBox(NULL, szErrorString, NULL, MB_OK);
		MessageBox(NULL, szGuiltyDirectory, NULL, MB_OK);
		MessageBox(NULL, szGuiltyExePath, NULL, MB_OK);
		MessageBoxA(NULL, szSidecarDllPathA, NULL, MB_OK);
		if (dwError == ERROR_INVALID_HANDLE) {
			MessageBox(NULL, L"Can't detour a 64-bit target process from a 32-bit parent process or vice versa.", NULL, MB_OK);
		}
		ExitProcess(9009);
	}

	if (!DetourCopyPayloadToProcess(pi.hProcess, s_guidSidecarPayload,
		szSidecarConfigPathA, sizeof(char) * strlen(szSidecarConfigPathA))) {
		StringCchPrintf(szErrorString, 1024, L"DetourCopyPayloadToProcess failed: %d", GetLastError());
		MessageBox(NULL, szErrorString, NULL, MB_OK);
		ExitProcess(9008);
	}

	ResumeThread(pi.hThread);
}

std::string GetLatestReleasePublicationDate() {
	std::wstring answer = GetHttpsRequest(GITHUB_API_BASE_URI, GITHUB_API_LATEST_RELEASE_ENDPOINT, HTTP_USER_AGENT);

	json resJson = json::parse(answer, NULL, false);
	if (resJson.contains("published_at")) { 
		return resJson["published_at"];
	}

	return "";
}

bool Update() {
	std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(REQUEST_TIMEOUT);
	
	std::string latestReleasePublicationDate = "";

	std::promise<std::string> prom;
	std::future<std::string> f_completes = prom.get_future();
	std::thread([](std::promise<std::string> prom)
		{
			prom.set_value(GetLatestReleasePublicationDate());
		},
		std::move(prom)
	).detach();

	if (std::future_status::ready == f_completes.wait_until(timeout)) {
		latestReleasePublicationDate = f_completes.get();
	}

	if (latestReleasePublicationDate == "") {
		return false;
	}

	if (ParseISO8601(_BUILD_DATETIME_ISO8601_) < ParseISO8601(latestReleasePublicationDate))
	{
		const int result = MessageBox(NULL, NEW_VERSION_AVAILABLE_MESSAGE.c_str(), NEW_VERSION_AVAILABLE_TITLE.c_str(), MB_YESNO);

		switch (result)
		{
		case IDYES:
			ShellExecuteA(NULL, "open", LATEST_RELEASE_URI.c_str(), NULL, NULL, SW_SHOWNORMAL);
			return true;
			break;
		case IDNO:
			return false;
			break;
		}
	}

	return false;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	wchar_t szGuiltyDirectory[1024] = { 0 };
	wchar_t szGuiltyExePath[1024] = { 0 };
	char szSidecarConfigPathA[1024] = { 0 };
	char szSidecarDllPathA[1024] = { 0 };

	DWORD env_size = 0;

	if (Update()) {
		return 0;
	}

	env_size = GetEnvironmentVariableW(L"GGPOPLUSR_LINUX_DIR", szGuiltyDirectory, 1024);

	if (env_size > 0) {
		PathCombineW(szGuiltyExePath, szGuiltyDirectory, L"GGXXACPR_Win.exe");
	}
	else {
		FindGuilty(szGuiltyDirectory, szGuiltyExePath);
	}
	FindSidecar(szSidecarDllPathA);
	FindConfigFile(szSidecarConfigPathA);
	CreateAppIDFile(szGuiltyDirectory);
	CreateGuiltyGearProcess(szGuiltyDirectory, szGuiltyExePath, szSidecarDllPathA, szSidecarConfigPathA);
	return 0;
}
