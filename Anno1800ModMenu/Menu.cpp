#include "pch.h"
#include "Menu.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <fstream>
#include <iostream>
#include <direct.h>
#include <sstream>
#include <string>
#include <filesystem>

namespace fs = std::experimental::filesystem;

std::string IntToString(int a)
{
	std::ostringstream temp;
	temp << a;
	return temp.str();
}


#define GetCurrentDir _getcwd

typedef int(__stdcall* PY_ISINITIALIZED)();
typedef int(__stdcall* PYRUN_SIMPLESTRING)(const char* str);
typedef int(__stdcall* PYRUN_SIMPLESTRINGFLAGS)(const char* str, DWORD* flags);
typedef DWORD* (__stdcall* PYGILSTATE_ENSURE)();
typedef void(__stdcall* PYGILSTATE_RELEASE)(DWORD* gstate);

HMODULE hModule = GetModuleHandle(L"python35.dll");
PY_ISINITIALIZED Py_IsInitialized = (PY_ISINITIALIZED)GetProcAddress(hModule, "Py_IsInitialized");
PYGILSTATE_ENSURE PyGILState_Ensure = (PYGILSTATE_ENSURE)GetProcAddress(hModule, "PyGILState_Ensure");
PYGILSTATE_RELEASE PyGILState_Release = (PYGILSTATE_RELEASE)GetProcAddress(hModule, "PyGILState_Release");
PYRUN_SIMPLESTRING PyRun_SimpleString = (PYRUN_SIMPLESTRING)GetProcAddress(hModule, "PyRun_SimpleString");
PYRUN_SIMPLESTRINGFLAGS PyRun_SimpleStringFlags = (PYRUN_SIMPLESTRINGFLAGS)GetProcAddress(hModule, "PyRun_SimpleStringFlags");

using namespace ImGui;

std::string curDir(std::string file)
{
	char buffer[MAX_PATH];
	GetCurrentDir(buffer, sizeof(buffer));
	std::string::size_type pos = std::string(buffer).find_last_of("\\/") - 1;
	return std::string(buffer) + "\\" + file;
}
 
void sendCommand(std::string cmd)
{
	std::ofstream outfile(curDir("script.lua"));
	outfile << cmd + "\r\n" << std::endl;
	outfile.close();

	std::string scriptPath = curDir("script.lua");
	std::size_t found = scriptPath.find_first_of("\\");
	while (found != std::string::npos)
	{
		scriptPath[found] = '/';
		found = scriptPath.find_first_of("\\", found + 1);
	}

	if (Py_IsInitialized != 0)
	{
		DWORD* gstate = PyGILState_Ensure();
		std::string command = "console.startScript(\"" + scriptPath + "\")";
		PyRun_SimpleStringFlags(command.c_str(), NULL);
		PyGILState_Release(gstate);
	}
}

float test = 1;

void Menu::Render()
{
	ImGui_ImplDX11_NewFrame();

	static bool no_titlebar = false;
	static bool no_border = true;
	static bool no_resize = true;
	static bool auto_resize = true;
	static bool no_move = false;
	static bool no_scrollbar = false;
	static bool no_collapse = true;
	static bool no_menu = true;
	static bool start_pos_set = false;

	

	ImGuiWindowFlags	window_flags = 0;
	if (no_titlebar)	window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_resize)		window_flags |= ImGuiWindowFlags_NoResize;
	if (auto_resize)	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	if (no_move)		window_flags |= ImGuiWindowFlags_NoMove;
	if (no_scrollbar)	window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (no_collapse)	window_flags |= ImGuiWindowFlags_NoCollapse;
	if (!no_menu)		window_flags |= ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowSize(ImVec2(450, 600), ImGuiSetCond_FirstUseEver);
	if (!start_pos_set) { ImGui::SetNextWindowPos(ImVec2(25, 25)); start_pos_set = true; }

	


	if (IsOpen)
	{
		//ImGui::GetIO().WantCaptureMouse = 0;


		ImGui::Begin("Anno 1800 Mod Menu v1.0", &IsOpen, window_flags);

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
		ImGui::PushItemWidth(-140);

		if (ImGui::CollapsingHeader("CHEATS"))
		{
			if (ImGui::TreeNode("Common"))
			{
				if (ImGui::Button("Toggle##1")) { sendCommand("ts.Cheat.GlobalCheats.ToggleIgnoreFertilities()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle ignore fertilities");

				if (ImGui::Button("Toggle##2")) { sendCommand("ts.Cheat.GlobalCheats.ToggleSuperShipSpeed()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle super ship speed");

				if (ImGui::Button("Toggle##3")) { sendCommand("ts.Cheat.GlobalCheats.ToggleElectricity()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle electricity");

				if (ImGui::Button("Toggle##4")) { sendCommand("ts.Cheat.GlobalCheats.ToggleIgnoreBuildingCosts()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle ignore building costs");

				if (ImGui::Button("Toggle##5")) { sendCommand("ts.Cheat.GlobalCheats.ToggleProductivity()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle productivity");

				if (ImGui::Button("Toggle##6")) { sendCommand("ts.Participants.CheatTotalTrade()"); }
				ImGui::SameLine(90);
				ImGui::Text("Total trade");

				if (ImGui::Button("Toggle##7")) { sendCommand("ts.Participants.CheatTotalWar()"); }
				ImGui::SameLine(90);
				ImGui::Text("Total war");

				if (ImGui::Button("Toggle##8")) { sendCommand("ts.Participants.CheatAlliance()"); }
				ImGui::SameLine(90);
				ImGui::Text("Total alliance");

				if (ImGui::Button("Add##1")) { sendCommand("ts.Area.Current.Economy.AddAmount(1010017, 100000)"); }
				ImGui::SameLine(90);
				ImGui::Text("Add 100k coins to current economy");

				if (ImGui::Button("Add##2")) { sendCommand("ts.Area.Current.Economy.AddAmount(10000)"); }
				ImGui::SameLine(90);
				ImGui::Text("Add Max goods to current economy");

				if (ImGui::Button("Add##3")) { sendCommand("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(1)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010196, 50)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010218, 50)"); }
				ImGui::SameLine(90);
				ImGui::Text("Add building materials to current selected ship");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Discovery"))
			{
				if (ImGui::Button("Toggle##9")) { sendCommand("TextSources.TextSourceRoots.Discovery.ShowAll()"); }
				ImGui::SameLine(90);
				ImGui::Text("Discover whole map");

				if (ImGui::Button("Toggle##10")) { sendCommand("TextSources.TextSourceRoots.Discovery.HideAll()"); }
				ImGui::SameLine(90);
				ImGui::Text("Hide whole map");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Weather"))
			{
				if (ImGui::Button("Toggle##11")) { sendCommand("ts.Weather.SetChangeWind()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle wind direction");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Incidents"))
			{
				if (ImGui::Button("Toggle##12")) { sendCommand("ts.Cheat.GlobalCheats.ToggleIncidents()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle incidents");

				if (ImGui::Button("Toggle##13")) { sendCommand("ts.Incidents.ToggleSpreading()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle incident spreading");

				if (ImGui::Button("Toggle##14")) { sendCommand("ts.Incidents.CheatUnlockAllIncidents()"); }
				ImGui::SameLine(90);
				ImGui::Text("Unlock all incidents");

				ImGui::TreePop();
			}
		}


		


		

		ImGui::Text("");
		ImGui::Text("By ChrisAnd1998");
		ImGui::Text("NOTE: Show/Hide this menu with F8 key. To unload hit DEL key.");

		ImGui::End();
	

	}
	else {
		//ImGui::GetIO().WantCaptureMouse = 1;
	}
	ImGui::Render();
}