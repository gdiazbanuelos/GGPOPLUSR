#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_demo.cpp>

#include "../game/game.h"

#define DEFAULT_ALPHA 0.87f

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 

void DrawGGPOConectionWindow(GameState* lpGameState, bool pOpen) {
	static char szOpponentIp[32];
	static unsigned short nOpponentPort;
	static unsigned short nOurPort;
	static int nOpponentPlayerPosition = 0;
	static GGPONetworkStats stats;
	int remotePlayerIndex;
	GGPOState* gs = &(lpGameState->ggpoState);

	ImGui::Begin("GGPO Connection", &pOpen);
	ImGui::InputScalar("Our port", ImGuiDataType_U16, &nOurPort);
	ImGui::InputText("Opponent IP", szOpponentIp, 32);
	ImGui::InputScalar("Opponent port", ImGuiDataType_U16, &nOpponentPort);
	ImGui::InputInt("Opponent player position", &nOpponentPlayerPosition);
	if (gs->ggpo == NULL) {
		if (ImGui::Button("Prepare for connection")) {
			PrepareGGPOSession(lpGameState, nOurPort, szOpponentIp, nOpponentPort, nOpponentPlayerPosition);
		}
	}
	else {
		remotePlayerIndex = gs->localPlayerIndex == 0 ? 1 : 0;
		ggpo_get_network_stats(gs->ggpo, gs->player_handles[remotePlayerIndex], &stats);
		ImGui::Text("Handles: %d, %d", gs->player_handles[0], gs->player_handles[1]);
		ImGui::Text("Player IDs: %d, %d", gs->p1.player_num, gs->p2.player_num);
		ImGui::Text("Player addresses: %p, %p", &(gs->p1), &(gs->p2));
		ImGui::Text("Remote player address: %p", gs->remotePlayer);
		ImGui::Text("Local player address: %p", gs->localPlayer);

		ImGui::Text("Remote player values");
		ImGui::Text("num: %d", gs->remotePlayer->player_num);
		ImGui::Text("IP: %s", gs->remotePlayer->u.remote.ip_address);
		ImGui::Text("port: %d", gs->remotePlayer->u.remote.port);
		ImGui::Text("type: %d", gs->remotePlayer->type);

		ImGui::Text("Stats");
		ImGui::Text("send_queue_len %d", stats.network.send_queue_len);
		ImGui::Text("recv_queue_len %d", stats.network.recv_queue_len);
		ImGui::Text("ping %d", stats.network.ping);
		ImGui::Text("kbps_sent %d", stats.network.kbps_sent);
		ImGui::Text("local_frames_behind %d", stats.timesync.local_frames_behind);
		ImGui::Text("remote_frames_behind %d", stats.timesync.remote_frames_behind);
	}
	ImGui::End();
}

void DrawGlobalStateWindow(GameState* lpGameState, bool* pOpen) {
	ImGui::Begin("Global State", pOpen, ImGuiWindowFlags_None);

	ImGui::Columns(2, NULL, false);

	ImGui::Text("Hitbox display enabled:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->bHitboxDisplayEnabled); ImGui::NextColumn();
	ImGui::Text("Camera x position:"); ImGui::NextColumn(); ImGui::Text("%f", *lpGameState->fCameraXPos); ImGui::NextColumn();
	ImGui::Text("Camera hold timer:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->nCameraHoldTimer); ImGui::NextColumn();
	ImGui::Text("Camera zoom:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->nCameraZoom); ImGui::NextColumn();
	ImGui::Text("Playfield left edge:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->nPlayfieldLeftEdge); ImGui::NextColumn();
	ImGui::Text("Playfield top edge:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->nPlayfieldTopEdge); ImGui::NextColumn();
	ImGui::Text("Round time remaining:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->nRoundTimeRemaining); ImGui::NextColumn();
	ImGui::Text("RNG index:"); ImGui::NextColumn(); ImGui::Text("%d", lpGameState->nRandomTable[0]); ImGui::NextColumn();
	ImGui::Text("Character root:"); ImGui::NextColumn(); ImGui::Text("%p", *lpGameState->arrCharacters); ImGui::NextColumn();
	ImGui::Text("NPC root:"); ImGui::NextColumn(); ImGui::Text("%p", *lpGameState->arrNpcObjects); ImGui::NextColumn();
	ImGui::Text("Player data root:"); ImGui::NextColumn(); ImGui::Text("%p", lpGameState->arrPlayerData); ImGui::NextColumn();
	ImGui::Text("Window handle:"); ImGui::NextColumn(); ImGui::Text("%d", *lpGameState->hWnd); ImGui::NextColumn();

	ImGui::Columns(1);

	ImGui::End();
}

void DrawObjectStateWindow(GameObjectData* lpGameObject, bool* pOpen) {
	ImGui::Begin(
		lpGameObject->playerIndex == 0 ? "Player 1 Object State" : "Player 2 Object State",
		pOpen,
		ImGuiWindowFlags_None
	);

	ImGui::Columns(2, NULL, false);

	ImGui::Text("Object address:");
	ImGui::Text("Object ID:");
	ImGui::Text("Action ID:");
	ImGui::Text("Object Facing:");
	ImGui::Text("Object Side:");
	ImGui::Text("Player data address:");
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
	ImGui::Text("%p", lpGameObject->playerData);
	ImGui::Text("%i", lpGameObject->xPos);
	ImGui::Text("%i", lpGameObject->ypos);
	ImGui::Text("%i", lpGameObject->xvel);
	ImGui::Text("%i", lpGameObject->yvel);

	ImGui::End();
}

void DrawPlayerStateWindow(TCHAR* windowName, PlayerData* lpPlayerData, bool* pOpen) {
	// Current faint gets stored as a short and divided by 100 before compared
	// to the character's maxFaint, which is stored as a single byte. Be careful
	// with integer truncation- comparing currentFaint to maxFaint*100 is not the
	// same as comparing currentFaint / 100 to maxFaint!
	int currentFaint = (int)(lpPlayerData->currentFaint / 100);
	int maxFaint = (int)lpPlayerData->maxFaint;

	ImGui::Begin(
		windowName,
		pOpen,
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
	ImGui::Text("Address"); ImGui::NextColumn(); ImGui::Text("%p", lpPlayerData); ImGui::NextColumn();
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

void DrawActionLogWindow(GameObjectData* lpGameObject, bool* pOpen) {
	static int lastAction[2] = { 0, 0 };
	static ExampleAppLog actionLogs[2];

	if (lpGameObject->actNo != lastAction[lpGameObject->playerIndex]) {
		actionLogs[lpGameObject->playerIndex].AddLog("Action ID: %04X\n", lpGameObject->actNo);
		lastAction[lpGameObject->playerIndex] = lpGameObject->actNo;
	}

	actionLogs[lpGameObject->playerIndex].Draw(
		lpGameObject->playerIndex == 0 ? "Player 1 Action Log" : "Player 2 Action Log", 
		pOpen
	);
}

void DrawSaveLoadStateWindow(GameState* lpGameState, bool* pOpen) {
	static SavedGameState savedState;
	static int nFramesToSkipRender = 0;
	static int nMinSkip = 0;
	static int nMaxSkip = 60;
	static int nFrameStep = 1;

	ImGui::Begin("Save/Load State", pOpen, ImGuiWindowFlags_None);
	if (nFramesToSkipRender < 0) {
		nFramesToSkipRender = 0;
	}
	else if (nFramesToSkipRender > 60) {
		nFramesToSkipRender = 60;
	}

	ImGui::InputInt("Num frames to skip", &nFramesToSkipRender);
	for (int p = 0; p < 2; p++) {
		const char* headerLabel = p == 0 ? "P1 inputs during skip" : "P2 inputs during skip";
		PlayerData* lpPlayerData = &lpGameState->arrPlayerData[p];
		if (ImGui::CollapsingHeader(headerLabel)) {
			// left
			static int selected = 0;
			ImGui::BeginChild("left pane", ImVec2(150, 0), true);
			for (int i = 0; i < nFramesToSkipRender; i++)
			{
				char label[128];
				sprintf(label, "Frame %d", i);
				if (ImGui::Selectable(label, selected == i))
					selected = i;
			}
			ImGui::EndChild();
			ImGui::SameLine();

			// right
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::CheckboxFlags("Left", &lpGameState->arrInputsDuringFrameSkip[selected][p], Left); ImGui::NextColumn();
			ImGui::CheckboxFlags("Down", &lpGameState->arrInputsDuringFrameSkip[selected][p], Down); ImGui::NextColumn();
			ImGui::CheckboxFlags("Up", &lpGameState->arrInputsDuringFrameSkip[selected][p], Up); ImGui::NextColumn();
			ImGui::CheckboxFlags("Right", &lpGameState->arrInputsDuringFrameSkip[selected][p], Right); ImGui::NextColumn();
			ImGui::CheckboxFlags("P", &lpGameState->arrInputsDuringFrameSkip[selected][p], lpPlayerData->ctrlP); ImGui::NextColumn();
			ImGui::CheckboxFlags("K", &lpGameState->arrInputsDuringFrameSkip[selected][p], lpPlayerData->ctrlK); ImGui::NextColumn();
			ImGui::CheckboxFlags("S", &lpGameState->arrInputsDuringFrameSkip[selected][p], lpPlayerData->ctrlS); ImGui::NextColumn();
			ImGui::CheckboxFlags("H", &lpGameState->arrInputsDuringFrameSkip[selected][p], lpPlayerData->ctrlH); ImGui::NextColumn();
			ImGui::CheckboxFlags("D", &lpGameState->arrInputsDuringFrameSkip[selected][p], lpPlayerData->ctrlD); ImGui::NextColumn();
			ImGui::CheckboxFlags("Respect", &lpGameState->arrInputsDuringFrameSkip[selected][p], lpPlayerData->ctrlRespect); ImGui::NextColumn();
			ImGui::EndChild();
		}
	}

	if (ImGui::Button("Save")) {
		SaveGameState(lpGameState, &savedState);
	}

	if (ImGui::Button("Load")) {
		// This should probably trigger a load on the _next_ frame, or we're
		// likely to do something bad to graphics memory.
		lpGameState->nFramesSkipped = 0;
		lpGameState->nFramesToSkipRender = nFramesToSkipRender;
		LoadGameState(lpGameState, &savedState);
	}

	ImGui::End();
}

void DrawHelpWindow(bool* pOpen) {
	ImGui::Begin(
		"ImGui Help", 
		pOpen, 
		ImGuiWindowFlags_None
	);

	ImGui::ShowUserGuide();

	ImGui::End();
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
	static bool show_global_state = false;
	static bool show_p1_object_state = false;
	static bool show_p1_state = false;
	static bool show_p1_log = false;
	static bool show_p2_object_state = false;
	static bool show_p2_state = false;
	static bool show_p2_log = false;
	static bool show_hitboxes = false;
	static bool show_saveload = false;
	static bool show_help = false;
	static bool show_ggpo = false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::IsMousePosValid() && ImGui::GetIO().MousePos.y < 200) {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Windows")) {
				ImGui::MenuItem("Global State", NULL, &show_global_state);

				if (ImGui::BeginMenu("Player State")) {
					ImGui::MenuItem("Player 1 State", NULL, &show_p1_state);
					ImGui::MenuItem("Player 2 State", NULL, &show_p2_state);
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Object State")) {
					ImGui::MenuItem("Player 1 Object State", NULL, &show_p1_object_state, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 1 Object Action Log", NULL, &show_p1_log, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 2 Object State", NULL, &show_p2_object_state, *lpGameState->arrCharacters != 0);
					ImGui::MenuItem("Player 2 Object Action Log", NULL, &show_p2_log, *lpGameState->arrCharacters != 0);
					ImGui::EndMenu();
				}

				ImGui::MenuItem("Save/Load State", NULL, &show_saveload);
				ImGui::MenuItem("GGPO", NULL, &show_ggpo);
				ImGui::EndMenu();
			}

			ImGui::MenuItem(
				show_hitboxes ? "Hide Hitboxes" : "Show Hitboxes",
				NULL,
				&show_hitboxes
			);

			ImGui::MenuItem(
				show_help ? "Hide Help" : "Show Help",
				NULL,
				&show_help
			);

			ImGui::EndMainMenuBar();
		}
	}
	if (show_global_state) {
		DrawGlobalStateWindow(lpGameState, &show_global_state);
	}
	if (show_p1_state) {
		DrawPlayerStateWindow("Player 1 State", &lpGameState->arrPlayerData[0], &show_p1_state);
	}
	if (show_p1_object_state) {
		DrawObjectStateWindow(&(*lpGameState->arrCharacters)[0], &show_p1_object_state);
	}
	if (show_p1_log) {
		DrawActionLogWindow(&(*lpGameState->arrCharacters)[0], &show_p1_log);
	}
	if (show_p2_state) {
		DrawPlayerStateWindow("Player 2 State", &lpGameState->arrPlayerData[1], &show_p2_state);
	}
	if (show_p2_object_state) {
		DrawObjectStateWindow(&(*lpGameState->arrCharacters)[1], &show_p2_object_state);
	}
	if (show_p2_log) {
		DrawActionLogWindow(&(*lpGameState->arrCharacters)[1], &show_p2_log);
	}
	if (show_help) {
		DrawHelpWindow(&show_help);
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
	if (show_saveload) {
		DrawSaveLoadStateWindow(lpGameState, &show_saveload);
	}
	if (show_ggpo) {
		DrawGGPOConectionWindow(lpGameState, &show_ggpo);
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
