#ifndef UNICODE
#define UNICODE
#endif

#define DIRECTINPUT_VERSION 0x0800

#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include <detours.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LPCWSTR DETOUR_FAILED_MESSAGE = L"Could not detour targets!!!!!";
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static HWND g_hwnd;

static BOOL(WINAPI* RealIsDebuggerPresent)() = IsDebuggerPresent;

static byte* g_lpPEHeaderRoot;
static void(WINAPI* RealGenerateAndShadePrimitives)();
static DWORD(WINAPI* RealInitializeLibraries)();
static int(WINAPI* RealSetupD3D9)();
static int(WINAPI* RealInitProcess)(int p1, int p2);
static bool(__cdecl* RealSteamAPI_Init)();
static LRESULT(WINAPI* RealWindowFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ImGuiIO g_io;

DWORD FakeInitializeLibraries() {
	g_hwnd = *(HWND*)(g_lpPEHeaderRoot + (0xeb6554 - 0x9b0000));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	g_io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hwnd);

	return RealInitializeLibraries();
}

LRESULT WINAPI FakeWindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return RealWindowFunc(hWnd, msg, wParam, lParam);
}

int FakeSetupD3D9() {
	int out = RealSetupD3D9();
	g_pd3dDevice = *(LPDIRECT3DDEVICE9*)(g_lpPEHeaderRoot + (0xf05b94 - 0x9b0000));
	ImGui_ImplDX9_Init(g_pd3dDevice);
	return out;
}

void FakeGenerateAndShadePrimitives() {
	// Guilty should've started the scene already.
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Log", NULL, ImGuiWindowFlags_None);
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(320, 150), ImGuiCond_Always);
	for (int n = 0; n < 3; n++) {
		ImGui::Text("%04d: Some text", n);
	}
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	RealGenerateAndShadePrimitives();
	// Guilty will end the scene after this call finishes.
}

void FakeSteamAPI_Init() {
	DWORD error;

	RealGenerateAndShadePrimitives = (void(WINAPI*)())(g_lpPEHeaderRoot + (0x00af9d20 - 0x9b0000));
	RealInitializeLibraries = (DWORD(WINAPI*)())(g_lpPEHeaderRoot + (0xaf6c90 - 0x9b0000));
	RealSetupD3D9 = (int(WINAPI*)())(g_lpPEHeaderRoot + (0xaf8b60 - 0x9b0000));
	RealWindowFunc = (LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(g_lpPEHeaderRoot + (0xaf6a00 - 0x9b0000));

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)RealGenerateAndShadePrimitives, FakeGenerateAndShadePrimitives);
	DetourAttach(&(PVOID&)RealInitializeLibraries, FakeInitializeLibraries);
	DetourAttach(&(PVOID&)RealSetupD3D9, FakeSetupD3D9);
	DetourAttach(&(PVOID&)RealWindowFunc, FakeWindowFunc);
	error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		MessageBox(NULL, TEXT("Detour failure!"), NULL, 0);
	}

	RealSteamAPI_Init();
}

BOOL FakeIsDebuggerPresent() {
	// This in theory can be disabled for a production release
	return FALSE;
}

__declspec(dllexport) BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD dwReason,
	LPVOID reserved
) {
	LONG error;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DetourRestoreAfterWith();

		g_lpPEHeaderRoot = (byte*)DetourGetContainingModule(
			DetourGetEntryPoint(NULL)
		);
		RealSteamAPI_Init = (bool(__cdecl*)())DetourFindFunction("steam_api", "SteamAPI_Init");

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)RealIsDebuggerPresent, FakeIsDebuggerPresent);
		DetourAttach(&(PVOID&)RealSteamAPI_Init, FakeSteamAPI_Init);
		error = DetourTransactionCommit();

		if (error != NO_ERROR) {
			MessageBox(NULL, DETOUR_FAILED_MESSAGE, NULL, MB_OK);
		}

		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		break;
	}
	return TRUE;
}
