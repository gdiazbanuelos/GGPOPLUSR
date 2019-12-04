#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>
#include <windows.h>
#include <detours.h>

LPCWSTR DETOUR_SUCCESSFUL_MESSAGE = L"Detouring successful!";
LPCWSTR DETOUR_FAILED_MESSAGE = L"Could not detour targets!!!!!";

static BOOL(WINAPI* RealIsDebuggerPresent)() = IsDebuggerPresent;
static LPCWSTR szMessage;

BOOL FakeIsDebuggerPresent() {
	// This in theory can be disabled for a production release
	return FALSE;
}

static HWND hwndMain;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

__declspec(dllexport) BOOL WINAPI DllMain(
	HINSTANCE hinst,
	DWORD dwReason,
	LPVOID reserved
) {
	LONG error;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)RealIsDebuggerPresent, FakeIsDebuggerPresent);
		error = DetourTransactionCommit();

		if (error == NO_ERROR) {
			szMessage = DETOUR_SUCCESSFUL_MESSAGE;
		}
		else {
			szMessage = DETOUR_FAILED_MESSAGE;
		}

		MessageBox(NULL, szMessage, NULL, MB_OK);
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
