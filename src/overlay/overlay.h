#pragma once

#include "../game/game.h"

void InitializeOverlay(GameState* lpGameState);
void DrawOverlay(GameMethods* lpGameMethods, GameState* lpGameState);
void FreeOverlay();
void InvalidateImGuiDeviceObjects();
void CreateImGuiDeviceObjects();

LRESULT WINAPI OverlayWindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);