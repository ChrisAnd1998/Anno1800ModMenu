#include "pch.h"
#include "InputHook.h"
#include "imgui.h"
#include "Menu.h"

using namespace ImGui;

WNDPROC	oWndProc;

void InputHook::Init(HWND hWindow)
{
	oWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
}

void InputHook::Remove(HWND hWindow)
{
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)oWndProc);
}

LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
		
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			GetIO().MouseDown[0] = true; //return DefWindowProc(hwnd, uMsg, wParam, lParam);
			if(ImGui::IsAnyWindowHovered()) {
				return 0; //block input to game
			}
			break;
		case WM_LBUTTONUP:
			GetIO().MouseDown[0] = false; 
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_RBUTTONDOWN:
			GetIO().MouseDown[1] = true;
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_RBUTTONUP:
			GetIO().MouseDown[1] = false;
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_MBUTTONDOWN:
			GetIO().MouseDown[2] = true;
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_MBUTTONUP:
			GetIO().MouseDown[2] = false;
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_MOUSEWHEEL:
			GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_MOUSEMOVE:
			GetIO().MousePos.x = (signed short)(lParam); GetIO().MousePos.y = (signed short)(lParam >> 16);
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_MOUSEHOVER:
			//GetIO().MousePos.x = (signed short)(lParam); GetIO().MousePos.y = (signed short)(lParam >> 16);
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		case WM_MOUSELAST:
			//GetIO().MousePos.x = (signed short)(lParam); GetIO().MousePos.y = (signed short)(lParam >> 16);
			if (ImGui::IsAnyWindowHovered()) {
				return 0;
			}
			break;
		}
	
	

	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}