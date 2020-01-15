#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_demo.cpp>

#include "../game/game.h"

#define DEFAULT_ALPHA 0.87f

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 

void DrawObjectStateWindow(GameObjectData* lpGameObject) {
	ImGui::Begin(
		lpGameObject->playerIndex == 0 ? "Player 1 Object State" : "Player 2 Object State",
		NULL,
		ImGuiWindowFlags_None
	);

	ImGui::Columns(2, NULL, false);

	ImGui::Text("Object address:");
	ImGui::Text("Object ID:");
	ImGui::Text("Action ID:");
	ImGui::Text("Object Facing:");
	ImGui::Text("Object Side:");
	ImGui::Text("X Position:");
	ImGui::Text("Y Position:");
	ImGui::Text("X Velocity:");
	ImGui::Text("Y Velocity:");

	ImGui::NextColumn();

	ImGui::Text("%p", lpGameObject);
	ImGui::Text("%X", lpGameObject->objectID);
	ImGui::Text("%X", lpGameObject->actNo);
	ImGui::Text("%X", lpGameObject->facing);
	ImGui::Text("%X", lpGameObject->side);
	ImGui::Text("%i", lpGameObject->xPos);
	ImGui::Text("%i", lpGameObject->ypos);
	ImGui::Text("%i", lpGameObject->xvel);
	ImGui::Text("%i", lpGameObject->yvel);

	ImGui::End();
}

void DrawPlayerStateWindow(GameObjectData* lpGameObject) {
	PlayerData* lpPlayerData = lpGameObject->playerData;

	// Current faint gets stored as a short and divided by 100 before compared
	// to the character's maxFaint, which is stored as a single byte. Be careful
	// with integer truncation- comparing currentFaint to maxFaint*100 is not the
	// same as comparing currentFaint / 100 to maxFaint!
	int currentFaint = (int)(lpPlayerData->currentFaint / 100);
	int maxFaint = (int)lpPlayerData->maxFaint;

	ImGui::Begin(
		lpGameObject->playerIndex == 0 ? "Player 1 State" : "Player 2 State",
		NULL,
		ImGuiWindowFlags_None
	);

	ImGui::SliderScalar("Health", ImGuiDataType_U16, &lpPlayerData->currentHealth, &MIN_HEALTH, &MAX_HEALTH);
	ImGui::SliderScalar("Burst", ImGuiDataType_U16, &lpPlayerData->currentBurst, &MIN_BURST, &MAX_BURST);
	ImGui::SliderScalar("Tension", ImGuiDataType_U16, &lpPlayerData->currentTension, &MIN_TENSION, &MAX_TENSION);
	ImGui::SliderScalar("Guard Balance", ImGuiDataType_S16, &lpPlayerData->guardBalance, &MIN_GUARD_BALANCE, &MAX_GUARD_BALANCE);
	ImGui::SliderScalar("Current Stun", ImGuiDataType_S32, &currentFaint, &MIN_FAINT, &maxFaint);
	ImGui::SliderScalar("Negative Penalty", ImGuiDataType_S32, &lpPlayerData->negativePenaltyCounter, &MIN_NEGATIVE_PENALTY, &MAX_NEGATIVE_PENALTY);

	ImGui::Separator();

	ImGui::Columns(2);
	ImGui::Text("Airdashes remaining"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->nAirDashesRemaining); ImGui::NextColumn();
	ImGui::Text("Airjumps remaining"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->nAirJumpsRemaining); ImGui::NextColumn();
	ImGui::Text("Rakusyo bonus"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->receiveRakushoBonus); ImGui::NextColumn();
	ImGui::Text("Instant kill mode"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->isInstantKillMode); ImGui::NextColumn();
	ImGui::Text("Projectile thrown"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->IsProjectileThrown); ImGui::NextColumn();
	ImGui::Text("Character meter 1"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->characterMeter1); ImGui::NextColumn();
	ImGui::Text("Character meter 2"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->characterMeter2); ImGui::NextColumn();
	ImGui::Text("Num times clean hit"); ImGui::NextColumn(); ImGui::Text("%d", lpPlayerData->numTimesCleanHit); ImGui::NextColumn();
	ImGui::Text("Max stun before faint"); ImGui::NextColumn(); ImGui::Text("%d", maxFaint); ImGui::NextColumn();
	ImGui::Columns(1);

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
	static bool show_p1_object_state = false;
	static bool show_p1_state = false;
	static bool show_p1_log = false;
	static bool show_p2_object_state = false;
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
					ImGui::MenuItem("Player 1 Object State", NULL, &show_p1_object_state, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 1 Action Log", NULL, &show_p1_log, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 2 State", NULL, &show_p2_state, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 2 Object State", NULL, &show_p2_object_state, *lpGameState->arrCharacters != 0);
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
	if (show_p1_object_state) {
		DrawObjectStateWindow(&(*lpGameState->arrCharacters)[0]);
	}
	if (show_p1_log) {
		DrawActionLogWindow(&(*lpGameState->arrCharacters)[0]);
	}
	if (show_p2_state) {
		DrawPlayerStateWindow(&(*lpGameState->arrCharacters)[1]);
	}
	if (show_p2_object_state) {
		DrawObjectStateWindow(&(*lpGameState->arrCharacters)[1]);
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