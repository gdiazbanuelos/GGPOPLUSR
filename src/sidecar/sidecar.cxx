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
#include <imgui_demo.cpp>
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>

#define DEFAULT_ALPHA 0.87f

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LPCWSTR DETOUR_FAILED_MESSAGE = L"Could not detour targets!!!!!";
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static HWND g_hwnd;

static BOOL(WINAPI* RealIsDebuggerPresent)() = IsDebuggerPresent;

static byte* g_lpPEHeaderRoot;
static void(WINAPI* RealDrawStage)();
static void(__fastcall* RealDrawGameUI)(DWORD);
static void(WINAPI* RealD3DSceneSetting)(int);
static void(WINAPI* RealGenerateAndShadePrimitives)();
static DWORD(WINAPI* RealInitializeLibraries)();
static int(WINAPI* RealSetupD3D9)();
static int(WINAPI* RealInitProcess)(int p1, int p2);
static bool(__cdecl* RealSteamAPI_Init)();
static LRESULT(WINAPI* RealWindowFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ExampleAppLog p1state_log;
static ExampleAppLog p2state_log;
short prev_p1action;
short prev_p2action;

static BOOL bIsBackgroundEnabled = true;

DWORD FakeInitializeLibraries() {
	g_hwnd = *(HWND*)(g_lpPEHeaderRoot + (0xeb6554 - 0x9b0000));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();
	style.Alpha = DEFAULT_ALPHA;
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

void WINAPI FakeDrawStage() {
	if (bIsBackgroundEnabled) {
		RealDrawStage();
	}
}

void __fastcall FakeDrawGameUI(DWORD param1) {
	if (bIsBackgroundEnabled) {
		RealDrawGameUI(param1);
	}
}

void SetCleanBackgroundState() {
	long error;
	wchar_t szMessage[512] = { 0 };
	PVOID lpGuiltyEntryPoint;

	DWORD* lpnSceneClearColor = (DWORD*)((byte*)g_lpPEHeaderRoot + 0x5476E8);
	*lpnSceneClearColor = 0x00ff00ff;
	bIsBackgroundEnabled = false;
}

LPDIRECT3DSURFACE9 GetGameRenderTarget() {
	return *(LPDIRECT3DSURFACE9*)(g_lpPEHeaderRoot + (0xeb5ae0 - 0x9b0000));
}

LPDIRECT3DSURFACE9 GetUIRenderTarget() {
	return *(LPDIRECT3DSURFACE9*)(g_lpPEHeaderRoot + (0xf05b98 - 0x9b0000));
}

LPDIRECT3DDEVICE9 GetD3DDevice() {
	return *(LPDIRECT3DDEVICE9*)(g_lpPEHeaderRoot + (0xf05b94 - 0x9b0000));
}

void FakeGenerateAndShadePrimitives() {
	// Guilty should've started the scene already.
	LPDIRECT3DDEVICE9 d3dDevice = GetD3DDevice();

	// This method temporarily sets the render target to the UI render target
	// to draw the UI elements like the sidebars, then sets the render target
	// back to the game render target so that other methods can rely on the
	// invariant that the device's current render target is always the "game"
	// surface.
	RealGenerateAndShadePrimitives();

	// Since we want to draw Imgui as if it were a UI element (ex. with the
	// full, unscaled window as the viewport), set the render target while we
	// draw Imgui.
	d3dDevice->SetRenderTarget(0, GetUIRenderTarget());

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DWORD** pstate = (DWORD**)(g_lpPEHeaderRoot + 0x516778);
	DWORD* p1state = *pstate;
	DWORD* p2state = (DWORD*)((int)*pstate + 0x130);
	short* p1action = (short*)((int)p1state + 0x18);
	short* p2action = (short*)((int)p2state + 0x18);

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320, 150), ImGuiCond_FirstUseEver);

	ImGui::Begin("SetCleanBackgroundState", NULL, ImGuiWindowFlags_None);
	if (ImGui::Button("Set")) {
		SetCleanBackgroundState();
	}
	ImGui::End();

	ImGui::Begin("Player 1 State", NULL, ImGuiWindowFlags_None);
	ImGui::Text("Base Address:\t%p", g_lpPEHeaderRoot);

	if (*pstate != 0) {
		ImGui::Text("Player Array Address Location:\t%p", pstate);
		ImGui::Text("Player 1 Array Address:\t%p", p1state);
		ImGui::Text("Player 1 Character ID:\t%02X", *((byte*)(int)p1state));
		ImGui::Text("Player 1 Action ID Address:\t%X", p1action);
		ImGui::Text("Player 1 Action ID Value:\t%04X", *p1action);
		if(*p1action != prev_p1action){
		p1state_log.AddLog("Action ID: %04X\n", *p1action);
		prev_p1action = *p1action;
		}
	}
	else {
		ImGui::Text("Player 1 State not initialized");
	}
	p1state_log.Draw("P1 Action ID Log");
	ImGui::End();

	ImGui::Begin("Player 2 State", NULL, ImGuiWindowFlags_None);
	ImGui::Text("Base Address:\t%p", g_lpPEHeaderRoot);

	if (*pstate != 0) {
		ImGui::Text("Player Array Address Location:\t%p", pstate);
		ImGui::Text("Player 2 Array Address:\t%p", p2state);
		ImGui::Text("Player 2 Character ID:\t%02X", *((byte*)(int)p2state));
		ImGui::Text("Player 2 Action ID Address:\t%X", p2action);
		ImGui::Text("Player 2 Action ID Value:\t%04X", *p2action);
		if (*p2action != prev_p2action) {
			p2state_log.AddLog("Action ID:%04X\n", *p2action);
			prev_p2action = *p2action;
		}
	}
	else {
		ImGui::Text("Player 2 State not initialized");
	}
	p2state_log.Draw("P2 Action ID Log");
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	// Preserve the original method's invariant by setting the render target
	// back to the "game" surface.
	d3dDevice->SetRenderTarget(0, GetGameRenderTarget());

	// Guilty will end the scene after this call finishes.
}

void FakeSteamAPI_Init() {
	DWORD error;

	RealGenerateAndShadePrimitives = (void(WINAPI*)())(g_lpPEHeaderRoot + (0x00af9d20 - 0x9b0000));
	RealInitializeLibraries = (DWORD(WINAPI*)())(g_lpPEHeaderRoot + (0xaf6c90 - 0x9b0000));
	RealSetupD3D9 = (int(WINAPI*)())(g_lpPEHeaderRoot + (0xaf8b60 - 0x9b0000));
	RealWindowFunc = (LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(g_lpPEHeaderRoot + (0xaf6a00 - 0x9b0000));
	RealDrawStage = (void(WINAPI*)())(
		g_lpPEHeaderRoot + (0x00af5130 - 0x9b0000)
		);
	RealDrawGameUI = (void(__fastcall*)(DWORD))(
		g_lpPEHeaderRoot + (0x00ac4050 - 0x9b0000)
		);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)RealGenerateAndShadePrimitives, FakeGenerateAndShadePrimitives);
	DetourAttach(&(PVOID&)RealInitializeLibraries, FakeInitializeLibraries);
	DetourAttach(&(PVOID&)RealSetupD3D9, FakeSetupD3D9);
	DetourAttach(&(PVOID&)RealWindowFunc, FakeWindowFunc);
	DetourAttach(&(PVOID&)RealDrawStage, FakeDrawStage);
	DetourAttach(&(PVOID&)RealDrawGameUI, FakeDrawGameUI);
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
