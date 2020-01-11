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
//#include <imgui_memory_editor.h>
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
static void(WINAPI* RealGenerateAndShadePrimitives)();
static DWORD(WINAPI* RealInitializeLibraries)();
static int(WINAPI* RealSetupD3D9)();
static int(WINAPI* RealInitProcess)(int p1, int p2);
static bool(__cdecl* RealSteamAPI_Init)();
static LRESULT(WINAPI* RealWindowFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void	createImguiWindows();

static ExampleAppLog p1state_log;
static ExampleAppLog p2state_log;
short prev_p1action;
short prev_p2action;

//static MemoryEditor mem_edit_1;



DWORD FakeInitializeLibraries() {
	g_hwnd = *(HWND*)(g_lpPEHeaderRoot + (0xeb6554 - 0x9b0000));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO &g_io = ImGui::GetIO();
	//g_io.MouseDrawCursor = TRUE;
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

	createImguiWindows();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	// Preserve the original method's invariant by setting the render target
	// back to the "game" surface.
	d3dDevice->SetRenderTarget(0, GetGameRenderTarget());

	// Guilty will end the scene after this call finishes.
}

void createImguiWindows() {

	DWORD** pstate = (DWORD**)(g_lpPEHeaderRoot + 0x516778);
	DWORD*	hstate = (DWORD*)(g_lpPEHeaderRoot + 0x520B3C);
	DWORD* p1state = *pstate;
	DWORD* p2state = (DWORD*)((int)*pstate + 0x130);
	short* p1action = (short*)((int)p1state + 0x18);
	short* p2action = (short*)((int)p2state + 0x18);
	DWORD* ctdiff = (DWORD*)(g_lpPEHeaderRoot + 0x4E6164);
	DWORD* cttime = (DWORD*)(g_lpPEHeaderRoot + 0x4E6164 + 0x18);
	DWORD* ctrand = (DWORD*)(g_lpPEHeaderRoot + 0x565F20);
	char p1binds[0x35];
	char p2binds[0x72];
	memcpy(p1binds, ((DWORD*)(g_lpPEHeaderRoot + 0x51A081)), 0x35 - 0x0);
	memcpy(p2binds, ((DWORD*)(g_lpPEHeaderRoot + 0x513529)), 0x72 - 0x0);




	static bool show_pstates = false;
	static bool show_hboxes = false;
	static bool show_ctable = false;
	ImGuiWindowFlags window_flags = 1080;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320, 150), ImGuiCond_FirstUseEver);

	ImGui::Begin("GGPO Plus R", NULL, window_flags);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::Text("Placeholder");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::Checkbox("Player States", &show_pstates);
				ImGui::Checkbox("Show Hitboxes", &show_hboxes);
				ImGui::Checkbox("Cheat Table", &show_ctable);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	ImGui::End();

		if(show_pstates){
		ImGui::Begin("Player 1 State", NULL, ImGuiWindowFlags_None);


		if (*pstate != 0) {
			ImGui::Columns(2, NULL, false);
			ImGui::Text("Base Address:");
			ImGui::Text("Player Array Address Location:");
			ImGui::Text("Player 1 Array Address:");
			ImGui::Text("Player 1 Character ID:");
			ImGui::Text("Player 1 Action ID Address:");
			ImGui::Text("Player 1 Action ID Value:");
			ImGui::NextColumn();
			ImGui::Text("%p", g_lpPEHeaderRoot);
			ImGui::Text("%p", pstate);
			ImGui::Text("%p", p1state);
			ImGui::Text("%02X", *((byte*)(int)p1state));
			ImGui::Text("%X", p1action);
			ImGui::Text("%04X", *p1action);

			if (*p1action != prev_p1action) {
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

		if (*pstate != 0) {
			ImGui::Columns(2, NULL, false);
			ImGui::Text("Base Address:");
			ImGui::Text("Player Array Address Location:");
			ImGui::Text("Player 2 Array Address:");
			ImGui::Text("Player 2 Character ID:");
			ImGui::Text("Player 2 Action ID Address:");
			ImGui::Text("Player 2 Action ID Value:");
			ImGui::NextColumn();
			ImGui::Text("%p", g_lpPEHeaderRoot);
			ImGui::Text("%p", pstate);
			ImGui::Text("%p", p2state);
			ImGui::Text("%02X", *((byte*)(int)p2state));
			ImGui::Text("%X", p2action);
			ImGui::Text("%04X", *p2action);

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
		}

		if(show_hboxes){
			ImGui::Begin("Hitbox Address");
			if (*pstate != 0) {
				*hstate = 1;
				ImGui::Text("Hitbox View Address:\t%p", hstate);
				ImGui::Text("Hitboxes enabled?:\t%s", "True");
			}
			else {
				*hstate = 0;
				ImGui::Text("Hitbox View Address:\t%p", *hstate);
				ImGui::Text("Characters are not Initialized yet.");
			}
			ImGui::End();
		}
		else {
			*hstate = 0;
		}

		if (show_ctable) {
			ImGui::Begin("Cheat Table", &show_ctable);
			if (ImGui::CollapsingHeader("Settings")) {
				ImGui::Columns(2, NULL, false);
				ImGui::Text("Difficulty:");
				ImGui::Text("Time:");
				ImGui::Text("Rounds:");
				ImGui::Text("Victory BGM:");
				ImGui::Text("Sol VA:");
				ImGui::Text("HOS VA:");
				ImGui::Text("Language:");
				ImGui::Text("+R on/off:");
				ImGui::Text("P1 Controller Binds:");
				ImGui::Text("P2 Controller Binds:");
				ImGui::NextColumn();
				ImGui::Text("%4X", *ctdiff);
				ImGui::Text("%4X", *cttime);
				ImGui::Text("%4X", *(ctdiff+0x30));
				ImGui::Text("%4X", *(ctdiff + 0x48));
				ImGui::Text("%4X", *(ctdiff + 0x60));
				ImGui::Text("%4X", *(ctdiff + 0x78));
				ImGui::Text("%4X", *(ctdiff + 0x90));
				ImGui::Text("%4X", *(ctdiff + 0xA8));
				ImGui::Text("%X", p1binds);
				ImGui::Text("%X", p2binds);
				ImGui::Columns(1, NULL, false);
			}
			if (ImGui::CollapsingHeader("In Game Values")) {
				ImGui::Columns(2, NULL, false);
				ImGui::Text("Rand:");
				ImGui::NextColumn();
				ImGui::Text("%02X", *((short*)ctrand));
				ImGui::Columns(1, NULL, false);
			}
			if (ImGui::CollapsingHeader("Training Mode Values")) {
			
			}
			if (ImGui::CollapsingHeader("Character Select Values")) {
			
			}
			if (ImGui::CollapsingHeader("Game State")) {
			
			}
			ImGui::End();
		}

	//if (*pstate != 0) {
	//	mem_edit_1.drawcontents(*pstate, 0x260, (size_t)*pstate);
	//}
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
