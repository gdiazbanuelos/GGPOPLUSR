#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_demo.cpp>

#include "../game/game.h"

#define DEFAULT_ALPHA 0.87f

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 

void DrawPlayerStateWindow(GameObjectData* lpGameObject) {
	ImGui::Begin(
		lpGameObject->playerIndex == 0 ? "Player 1 State" : "Player 2 State",
		NULL,
		ImGuiWindowFlags_None
	);

	ImGui::Columns(2, NULL, false);

	ImGui::Text("Object address:");
	ImGui::Text("Object ID:");
	ImGui::Text("Action ID:");
	ImGui::Text("Current Health:");
	ImGui::Text("Current Tension:");
	ImGui::Text("Character Facing:");
	ImGui::Text("Character Side:");
	ImGui::Text("Guard Balance:");
	ImGui::Text("Dizzy Value:");
	ImGui::Text("Character X Position:");
	ImGui::Text("Character Y Position:");
	ImGui::Text("Character X Velocity:");
	ImGui::Text("Character Y Velocity:");

	ImGui::NextColumn();

	ImGui::Text("%p", lpGameObject);
	ImGui::Text("%X", lpGameObject->objectID);
	ImGui::Text("%X", lpGameObject->actNo);
	ImGui::Text("%i", lpGameObject->field_0x1e);
	ImGui::Text("%hi", lpGameObject->playerData->currentTension);
	ImGui::Text("%X", lpGameObject->field_0x2);
	ImGui::Text("%X", lpGameObject->field_0x3);
	ImGui::Text("%hi", lpGameObject->playerData->guardBalance);
	ImGui::Text("%hi", lpGameObject->playerData->currentFaint);
	ImGui::Text("%i", lpGameObject->xPos);
	ImGui::Text("%i", lpGameObject->ypos);
	ImGui::Text("%i", lpGameObject->xvel);
	ImGui::Text("%i", lpGameObject->yvel);

	ImGui::End();
}

void DrawActionLogWindow(GameObjectData* lpGameObject) {
	static int lastAction[2] = { 0, 0 };
	static ExampleAppLog actionLogs[2];

	if (lpGameObject->actNo != lastAction[lpGameObject->playerIndex]) {
		actionLogs[lpGameObject->playerIndex].AddLog("Action ID: %04X\n", lpGameObject->actNo);
		lastAction[lpGameObject->playerIndex] = lpGameObject->actNo;
	}

	actionLogs[lpGameObject->playerIndex].Draw(
		lpGameObject->playerIndex == 0 ? "Player 1 Action Log" : "Player 2 Action Log"
	);
}

void InitializeOverlay(GameState* lpGameState) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = DEFAULT_ALPHA;
	ImGui_ImplWin32_Init(*lpGameState->hWnd);
	ImGui_ImplDX9_Init(*lpGameState->d3dDevice);
}

void DrawOverlay(GameMethods* lpGameMethods, GameState* lpGameState) {
	static bool show_p1_state = false;
	static bool show_p1_log = false;
	static bool show_p2_state = false;
	static bool show_p2_log = false;
	static bool show_hitboxes = false;
	static bool show_cheattable = false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::IsMousePosValid() && ImGui::GetIO().MousePos.y < 200) {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Windows")) {
				if (ImGui::BeginMenu("Player State")) {
					ImGui::MenuItem("Player 1 State", NULL, &show_p1_state, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 1 Action Log", NULL, &show_p1_log, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 2 State", NULL, &show_p2_state, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 2 Action Log", NULL, &show_p2_log, *lpGameState->arrCharacters != 0);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			ImGui::MenuItem(
				show_hitboxes ? "Hide Hitboxes" : "Show Hitboxes",
				NULL,
				&show_hitboxes
			);
			ImGui::EndMainMenuBar();
		}
	}

	if (show_p1_state) {
		DrawPlayerStateWindow(&(*lpGameState->arrCharacters)[0]);
	}
	if (show_p1_log) {
		DrawActionLogWindow(&(*lpGameState->arrCharacters)[0]);
	}
	if (show_p2_state) {
		DrawPlayerStateWindow(&(*lpGameState->arrCharacters)[1]);
	}
	if (show_p2_log) {
		DrawActionLogWindow(&(*lpGameState->arrCharacters)[1]);
	}
	if (show_hitboxes) {
		if (*lpGameState->bHitboxDisplayEnabled == 0) {
			*lpGameState->bHitboxDisplayEnabled = 1;
		}
	}
	else {
		if (*lpGameState->bHitboxDisplayEnabled != 0) {
			*lpGameState->bHitboxDisplayEnabled = 0;
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void FreeOverlay() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

LRESULT WINAPI OverlayWindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}