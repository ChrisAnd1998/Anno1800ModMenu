#include <string>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
#include <direct.h>
#include <wtypes.h>
#include <vector>
#include "Menu.h"
#include "../ImGui/imgui.h"
#include "TextEditor.h"
#include "imfilebrowser.h"

#define GetCurrentDir _getcwd

typedef int(__stdcall* PY_ISINITIALIZED)();
typedef int(__stdcall* PYRUN_SIMPLESTRING)(const char* str);
typedef int(__stdcall* PYRUN_SIMPLESTRINGFLAGS)(const char* str, DWORD* flags);
typedef DWORD* (__stdcall* PYGILSTATE_ENSURE)();
typedef void(__stdcall* PYGILSTATE_RELEASE)(DWORD* gstate);

HMODULE hModule = GetModuleHandle("python35.dll");
PY_ISINITIALIZED Py_IsInitialized = (PY_ISINITIALIZED)GetProcAddress(hModule, "Py_IsInitialized");
PYGILSTATE_ENSURE PyGILState_Ensure = (PYGILSTATE_ENSURE)GetProcAddress(hModule, "PyGILState_Ensure");
PYGILSTATE_RELEASE PyGILState_Release = (PYGILSTATE_RELEASE)GetProcAddress(hModule, "PyGILState_Release");
PYRUN_SIMPLESTRING PyRun_SimpleString = (PYRUN_SIMPLESTRING)GetProcAddress(hModule, "PyRun_SimpleString");
PYRUN_SIMPLESTRINGFLAGS PyRun_SimpleStringFlags = (PYRUN_SIMPLESTRINGFLAGS)GetProcAddress(hModule, "PyRun_SimpleStringFlags");

using namespace ImGui;

TextEditor editor;

// create a file browser instance
ImGui::FileBrowser fileDialog;

bool savefile = false;

static const char* fileToEdit;

std::string currentFile = "";

void fileEdit(std::string file) {
	fileToEdit = NULL;
	fileToEdit = (char*)file.c_str();
	//currentFile = file;

	//	static const char* fileToEdit = "test.cpp";
		std::ifstream inFile;
		inFile.open(fileToEdit); //open the input file

		std::stringstream strStream;
		strStream << inFile.rdbuf(); //read the file
		std::string str = strStream.str(); //str holds the content of the file

		editor.SetText(str);
}

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

bool showModEditor = false;

void Menu::Render(bool* p_open)
{
	// Render the menu
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = true;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool unsaved_document = false;
	static bool auto_resize = true;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (auto_resize)		window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(1.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 2.0f);
	style.FrameRounding = 4.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(10.0f, 2.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 12.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.9176470637321472f, 0.9176470637321472f, 0.9176470637321472f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.85f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1098039224743843f, 0.1098039224743843f, 0.1098039224743843f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1098039224743843f, 0.1098039224743843f, 0.1098039224743843f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.5299999713897705f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2078431397676468f, 0.2078431397676468f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8078431487083435f, 0.8274509906768799f, 0.8078431487083435f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.929411768913269f, 0.6470588445663452f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2078431397676468f, 0.2078431397676468f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2078431397676468f, 0.2078431397676468f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.09803921729326248f, 0.1490196138620377f, 0.9700000286102295f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.2588235437870026f, 0.4196078479290009f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);

	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Anno 1800 Mod Menu v1.3", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
		ImGui::PushItemWidth(-140);

		if (ImGui::CollapsingHeader("CHEATS", 32))
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

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Items"))
			{
				static int i0 = 100000;
				if (ImGui::Button("Add##1")) { sendCommand("ts.Area.Current.Economy.AddAmount(1010017, " + std::to_string(i0) + ")"); }
				ImGui::SameLine(90);
				ImGui::PushItemWidth(125);
				ImGui::InputInt(" coins to current economy##inputi0", &i0);

				static int i1 = 100;
				if (ImGui::Button("Add##2")) { sendCommand("ts.Area.Current.Economy.AddAmount(" + std::to_string(i1) + ")"); }
				ImGui::SameLine(90);
				ImGui::PushItemWidth(125);
				ImGui::InputInt(" goods to current economy##inputi1", &i1);

				static const char* items[] = { "191868@Blown-Apart 12-Pounder", "111018@(A_Clash_of_Couriers) Freedom For All Poster", "111018@Freedom For All Poster", "112550@Spoondrift ? Nathaniel's Pressurised Diving Bell", "190841@The Cutting Edge", "134715@1801 Nights", "191853@18-Pounder Long Gun", "134705@50 Shades of Gravy", "192213@A Bottle of Champagne", "192240@A Child's Letter", "133321@A Dithyrambic Note From the Queen", "104935@A Letter to Kahina", "104934@A Letter to Kahina", "104933@A Letter to Kahina", "104896@A Letter to Kahina", "191761@A Mighty Martial Law Declaration", "192207@A Nice Cup Of Darjeeling", "133322@A Note From the Queen", "192362@A Remedy", "192336@A Shimmering Stone", "191821@A Study For Men", "192483@(A_Clash_of_Couriers) A Vindication of Women's Rights", "192483@A Vindication of Women's Rights", "1010250@(Seasons_of_Silver) ??--24890--??", "125122@Aaden Issack, World-Famous Enbesan Chef", "190711@Abb?", "120016@(Pride_and_Peddlers) Absinthe", "191867@Abus Gun", "190728@Actor", "125066@Adjustable Mannequin", "191610@Admiral Haynes, Beholder of Far Horizons.", "112825@Admiral Nadasky's Sabre", "24151@(Eden_Burning) Advanced Agricultural Textbook", "24151@Advanced Agricultural Textbook", "24107@(Eden_Burning) Advanced Fertiliser", "24107@Advanced Fertiliser", "24023@(Eden_Burning) Advanced Industrial Toxin Filter", "24023@Advanced Industrial Toxin Filter", "24163@(Eden_Burning) Advanced Pyrphorian Calciner", "24163@Advanced Pyrphorian Calciner", "125052@Advanced Sluice Gate", "24014@(Eden_Burning) Advanced Soil Sampler", "24014@Advanced Soil Sampler", "24044@(Eden_Burning) Advanced Trash Filter", "24044@Advanced Trash Filter", "112556@Advanced Umbilical Cable", "1010223@Advanced Weapons", "125132@Aesop, the Fabled Storyteller", "111175@Affiliated Anarchist", "190687@Agriculturalist", "2833@Airmail Stamp", "4133@(Pride_and_Peddlers) Airship Cargo Loader", "4133@Airship Cargo Loader", "4132@Airship Engine Mechanic", "4277@Airship Logistician", "2232@Airship Munitions Loader", "2232@(A_Clash_of_Couriers) Airship Munitions Loader", "4117@Airship Technician", "124374@Akdamot Port Registry", "191507@Albino Alligator", "191106@Albino Fennec", "193192@Albino Gorilla", "191922@Alcohol Crate", "193187@Alexander Hancock", "24065@(Eden_Burning) Algologist", "24065@Algologist", "113907@Alligator Gar", "192198@Alonso's Blunted Bone Saw", "192196@Alonso's Hand Drill", "192199@Alonso's Tranquilizers", "191113@Alpaca", "120036@(Seasons_of_Silver) Alpaca Wool", "120036@Alpaca Wool", "190516@Altar of Uppsidaisi", "112374@AlterN8 Current Generator", "838@(Pride_and_Peddlers) Aluminium Profiles", "838@Aluminium Profiles", "121004@Amadi Ilga, Ketema's Chief Civil Engineer", "129022@Amazing Fashion Designer", "4134@Amelia Coleman, Expert Aviator", "5399@Ammonia", "190483@Ammonites", "191788@(A_Clash_of_Couriers) Amnesty on Religious Symbology", "191788@Amnesty on Religious Symbology", "111039@(A_Clash_of_Couriers) An Anarchist: A Self-Portrait", "111039@An Anarchist: A Self-Portrait", "111033@(A_Clash_of_Couriers) An Essay on Birth Control", "111033@An Essay on Birth Control", "192312@An Old Medallion", "110965@Anarchist's Bomb Pack", "190482@Ancient Manifest", "130777@Ancient Tabot of Waha Desher", "111108@Angel Wings Lotus", "190237@Angela Meg Iver, The Polyvalent", "190868@Angelic Anthrax Vaccine", "24061@(Eden_Burning) Angler", "24061@Angler", "190615@Angler", "124869@Animal Figure", "1964@Animal Nutritionist", "192465@Anna Union, Engineer of Extravagant Steamers", "191568@Anne Kenyatta, Special Needs Teacher", "192236@Anne's Gramophone", "192237@Anne's Message in A Bottle", "113361@Anthony Anchovy, Salvager of Sunken Sumps", "110994@Anti-Bourgeois Cocktail", "110986@Anti-Bourgeois Gas Pipe", "110995@Anti-Bourgeois Midnight Infusion", "111176@Anti-Clerical Lecturer", "190478@Antique Novel", "191907@Antiseptics", "116097@Antler Iggaak", "190504@Apatosaurus", "24655@Aquafarm Fish Eggs (Limited Use)", "24372@Aquafarm Fish Eggs (Persistent)", "118137@Arabesque Trinkets", "24068@(Eden_Burning) Arboriculturist", "24068@Arboriculturist", "190690@(Pride_and_Peddlers) Arborist", "190690@Arborist", "190717@Arch-Archbishop Archibald", "191719@Archbishop Stipwick's Stiff-Lipped Curfew", "115469@Arctic Fox", "112706@Arctic Gas", "1772@Arctic Gas Expert", "4272@Arctic Science Reporter", "191086@Arctic Wolf", "192440@Arek Lisowski, the Architect", "191470@Aristelia Bataille, of the Novelty Store", "124875@Arksum Obelisk", "192028@Armagnac Bottles", "116133@Arnarunn the Seamless", "111127@Arsonist", "191709@Art and Craft Festival Poster", "116149@Artful Inuit Hunter", "191752@(A_Clash_of_Couriers) Arthur Ramshackle's Grand Reservoir Plans", "191752@Arthur Ramshackle's Grand Reservoir Plans", "191433@Artillerywoman", "192215@Artur's Magnifying Glass", "192216@Artur's Portfolio", "116159@Aspirin Machine", "112387@AssassiN8r", "110950@(A_Clash_of_Couriers) Assembly Hall of Blissful Joy and Tireless Labour", "110950@Assembly Hall of Blissful Joy and Tireless Labour", "190888@Assembly Line", "125142@Associate Professor of Faculty", "116143@Astute Dog-Breeder", "112824@Atlantean Tablet", "112478@Atlantean Wall", "1010258@(Seasons_of_Silver) Atole", "1010257@(Eden_Burning) Atole", "25131@Atole", "113354@Audrey the Marine Archaeologist", "190511@Augustan Forum", "190536@Aureus Coins", "131223@Authentic Ark of the Covenant", "116183@Automatic Souvenir Kiosk", "191889@Auxetophone", "191381@Award-Winning Brewer", "191925@Bag of Money", "191108@Baiji", "7225@Bait Specialist", "190628@Baker", "190860@(Pride_and_Peddlers) Balance Plough", "190860@Balance Plough", "192223@Ball and Chain", "112554@Ballast Cable", "112548@Ballasted Diving Bell", "190464@Balloon Fish", "191800@Bank Holiday Decree", "191576@Banker", "192228@Banner of La Corona", "125121@Banquet Chef", "192006@Bar Shots", "190476@Barbarossa's Bones", "190669@Bartender", "24114@(Eden_Burning) Basic Agricultural Textbook", "24114@Basic Agricultural Textbook", "24100@(Eden_Burning) Basic Fertiliser", "24100@Basic Fertiliser", "191085@Bat-Eared Fennec", "836@(Pride_and_Peddlers) Bauxite", "836@Bauxite", "191367@Bean Sifter", "112695@Bear Fur", "101897@Beard of the Year Award", "124060@Bearded Vulture", "190716@Beatific Bishopess", "190832@(Pride_and_Peddlers) Bechamel Converter", "190832@Bechamel Converter", "1010193@Beef", "1010214@(Seasons_of_Silver) Beer", "1010214@Beer", "114370@Beeswax", "111179@(A_Clash_of_Couriers) Bekonin, Spirit of Liberty", "111179@Bekonin, Spirit of Liberty", "136885@Belinda San Pedro, Head of Arcade Acquisitions", "191802@Bente Jorgensen's Perfectly Paid Vacations Act", "192190@Bente's Rarefied Seedling", "192193@Bente's Travel Journal", "125050@Berkeley Meat Sunderer", "191725@Bertie Pinkie-Toff's Edict of Elitism", "192206@Beryl's Barrel", "192358@Beryl's Fosterling", "192204@Beryl's Mink Coat", "192205@Beryl's Nail Clippers", "125137@Bespoke Tailor", "192033@Bicycle Repair Kit", "191923@Big Alcohol Crate", "110966@Big Anarchist's Bomb Pack", "191926@Big Bag of Money", "192016@Big Box of Dynamite", "112562@Big MagN8", "114540@Bilberry", "135232@Billiard Tables", "125053@Biniam's Floodgate", "125058@Birdhouse", "191093@Bird-of-Paradise", "135229@(Pride_and_Peddlers) Biscuits", "135229@Biscuits", "191077@Bison", "191072@Black Bear", "191091@Black Caiman", "193903@Black Flag", "192173@Black Flag", "190004@Black Jaguar", "191969@Black Powder Fireworks", "124164@Black Rhino", "112812@Black Swallower", "8155@Blending Tank", "112813@Blobfish", "125067@Blotherhithe's Fusing Machine", "137797@Blue Skies Delivery Service", "137799@Blue Skies Janitor", "137798@Blue Skies Maid", "192214@Blueprint For An Infeasibly Large Ocean Liner", "190699@Boilermaker", "191999@Bomb", "120037@(Seasons_of_Silver) Bombins", "120037@Bombins", "116101@Bone Adze", "125044@Book-Binding Frame", "191075@Boreal Caribou", "191447@Bosun", "24077@(Eden_Burning) Botanist", "24077@Botanist", "190449@Bottlenose Dolphin", "124867@Bovine Ivory Ring", "116080@Bow Of The Lnu", "190850@Bow Saw", "191706@Boy's Choir Flyer", "1010204@Brass", "190673@Brasserie Patron Mertens", "1010213@Bread", "191382@Brewmaster Bill", "191461@(Pride_and_Peddlers) Brickmaker", "191461@Brickmaker", "1010205@Bricks", "191971@Brilliant Fireworks", "125072@Broadcast Relay Station", "190515@Br?n Dolmen", "190523@Bronze Helmet", "137288@Bronze Performance Medal", "190526@Bronze Sword", "191383@Brother Hilarius, Purveyor of Monastic Mixtures", "191052@Brown Bear", "190551@Brownbeard's Cutlass", "190664@Bruiser", "191422@Bruno Ironbright, Engineering Giant", "191980@Builder's Tools", "125139@Bumm Brimmell, the Original Dandy", "110943@(A_Clash_of_Couriers) Bureau of Worker-Owned Cooperatives", "110943@Bureau of Worker-Owned Cooperatives", "190740@Burner", "190844@Burrstone", "191384@Butcher", "190454@Butterflies", "191429@Cabinet Maker", "191703@Cabinet of Curiosities Pamphlet", "116079@Cable-Backed Bow", "191705@Cake Sale Flyer", "5380@Calamari", "190830@Calciner", "191071@California Gull", "125116@Calla Lily, Of The Desert Bloom", "114857@Camellia Garden", "114870@Camellia Garden", "114869@Camellia Garden", "134616@Camphor Wax", "8154@Can Seamer", "190931@Cane Saplings", "191964@Canister Shot", "1010217@(Pride_and_Peddlers) Canned Food", "1010217@Canned Food", "191394@Canner", "191434@Cannoneer", "132847@Canterbarry's Countryside Cruise Leaflet", "1010255@(Seasons_of_Silver) Caoutchouc", "1010255@Caoutchouc", "112555@Caoutchouc Cable", "190943@Caoutchouc Saplings", "24018@(Eden_Burning) Capacious Waste Treatment Tank", "24018@Capacious Waste Treatment Tank", "191755@(A_Clash_of_Couriers) Caprice Ltd, The Monopoly On Local Construction", "191755@Caprice Ltd, The Monopoly On Local Construction", "191451@Captain Felicity Phoenix, Back From the Grave.", "190766@Captain Ibrahim", "191450@(Pride_and_Peddlers) Captain Kadijah", "191450@Captain Kadijah", "190622@Captain Moby, Old Dog of the Sea", "125057@Captain Nineveh's Miraculous Lobster Trap", "191600@Captain of Marines", "191452@Captain Richard Moses, Skipper of Schooners", "191453@Captain Smith, Titan of the Sea.", "111026@(A_Clash_of_Couriers) Captains Of Industry, Vol IV", "111026@Captains Of Industry, Vol IV", "111027@(A_Clash_of_Couriers) Captains Of Industry, Vol XVI", "111027@Captains Of Industry, Vol XVI", "111028@(A_Clash_of_Couriers) Captains Of Industry, Vol XXXI", "111028@Captains Of Industry, Vol XXXI", "124157@Caracal", "112576@CarboN8r", "125126@Careful Craftsman", "112694@Caribou Meat", "191425@Carpenter", "190694@Carpenter", "191979@Carpenter's Tools", "192211@Carriage Wheel", "129663@Cartographic Sketchbook", "115471@Carved Stone Bear", "190892@(A_Clash_of_Couriers) Cast Iron Stove", "190892@Cast Iron Stove", "192165@Cast Mortar", "191058@Cattle", "1010193@(Eden_Burning) Cattle", "192031@Cauldron of Fish Stew", "191370@(Pride_and_Peddlers) Cecilia the Coffeessieur", "191370@Cecilia the Coffeessieur", "190645@Cellar Man", "135150@(Pride_and_Peddlers) Celluloid", "135150@Celluloid", "1010231@Cement", "191958@Cemented Steel Shells", "191462@Cementer", "191771@Censored Books Act", "118724@Ceramics", "191676@(A_Clash_of_Couriers) Certificate of Industrial Standard", "191676@Certificate of Industrial Standard", "191675@(A_Clash_of_Couriers) Certificate of Inspection", "191675@Certificate of Inspection", "192005@Chain Shots", "120016@Champagne", "190647@Champagne Taster", "114388@Champak", "191315@Chandler", "191401@Chantelle the Charcutier", "191403@Charcutier", "191333@(Pride_and_Peddlers) Charlotte the Chocoholic", "191333@Charlotte the Chocoholic", "1010211@Chassis", "124160@Cheetah", "191406@Chef Michel", "190863@Chemical Fertiliser", "135087@Cherry Wood", "135186@(Pride_and_Peddlers) Chewing Gum", "24824@Chewing Gum", "135186@Chewing Gum", "191059@Chicken", "190658@Chief George Doughty, Smouldering Hero", "191009@(Pride_and_Peddlers) Chief Superintendent Clifford", "191009@(A_Clash_of_Couriers) Chief Superintendent Clifford", "191009@Chief Superintendent Clifford", "191751@(A_Clash_of_Couriers) Chimney Sweep Charter", "191751@Chimney Sweep Charter", "191073@Chital", "1010258@Chocolate", "191331@(Pride_and_Peddlers) Chocolatier", "191331@Chocolatier", "130945@Chronicles of Enbesa", "130939@Chronicles of Enbesa", "130938@Chronicles of Enbesa", "130937@Chronicles of Enbesa", "191323@Chronometrist Chiara", "191742@Church Tax Law", "190641@(Pride_and_Peddlers) Cigar Afficionado", "190641@Cigar Afficionado", "1010259@(Seasons_of_Silver) Cigars", "1010259@Cigars", "133093@Cinnamon", "190909@Circular Rip Saw", "190908@Circular Saw", "133097@Citrus", "191691@(A_Clash_of_Couriers) City Map Act", "191691@City Map Act", "1010201@Clay", "116081@Clay Kudlik", "114414@Clay Pipes", "129238@Cliffside Map", "191321@Clockmaker", "1010226@Coal", "191754@(A_Clash_of_Couriers) Coal Burner Advertisement", "191754@Coal Burner Advertisement", "191684@Coal Permit", "112567@Coastal Dredge", "191110@Coastal Leopard", "125068@Cobbler's Last", "1010254@(Pride_and_Peddlers) Cocoa", "1010254@Cocoa", "190940@Cocoa Beans", "133095@Coconut Oil", "111121@Coconut Palm", "113469@Coelacanth", "120032@(Seasons_of_Silver) Coffee", "120032@Coffee", "190946@Coffee Beans", "120031@(Seasons_of_Silver) Coffee Beans", "120031@Coffee Beans", "191369@(Pride_and_Peddlers) Coffee Sommelier", "191369@Coffee Sommelier", "135234@(Pride_and_Peddlers) Cognac", "135234@Cognac", "1010017@Coins", "24821@Coke", "116185@Cola Mola Wola Machine", "114637@Collection Of Lost Expedition Relics", "191723@Collegiate Bill", "191435@Colonel Barzagli", "191970@Coloured Fireworks", "190528@Colourful Textiles", "958@Combustion Chemist", "190727@Comedian", "191611@Commmodore Stephen Drake, The Besieger.", "113535@Commodore's Medal", "111112@Common Reed", "24054@(Eden_Burning) Communal Garden", "24054@Communal Garden", "136699@Concierge", "191069@Condor", "191330@Confectioner", "111162@Confessor of the Burgeois", "191852@Congrave Rocket", "8417@Constable Chaos", "191816@Contraception Regulation", "191831@Controllable-Pitch Propeller", "190887@Conveyor", "1010230@Copper", "190894@Copper Distiller", "190553@Copper Ring", "110937@(A_Clash_of_Couriers) Copperplate Printer", "110937@Copperplate Printer", "120034@(Seasons_of_Silver) Corn", "120034@Corn", "190949@Corn Seeds", "190752@Cosmo Castelli, Agronomic Trailblazer", "191348@Costume Designer", "5389@Costumes", "1010253@Cotton", "1010240@Cotton Fabric", "190937@Cotton Seeds", "112484@Court of Laws", "129236@Court-Sealed Map", "116182@Craft Beer To Takeaway", "112560@Crate of Convulsing Bait", "191707@Crimbles Choral Society Poster", "104639@Criminal Esoterica", "191347@Cristobal Taffeta, The Trendsetter", "190731@Critically-Acclaimed Tragedian", "191506@Crocodile", "190745@Cropper", "191915@CTC Extinguisher", "190461@Curly-Hair Tarantula", "192355@Cursed Mummy of Cabot", "119217@CV and Cover Letter from 'Decorated General' Tutor", "119216@CV and Cover Letter from 'Master Craftsman' Tutor", "119215@CV and Cover Letter from 'Noble' Tutor", "24852@Cyanide", "190525@Damaged Statue", "114539@Dandelion", "190620@(Pride_and_Peddlers) Daring Scuba Diver", "190620@Daring Scuba Diver", "191424@Dario the Mechanical Engineer", "125143@Dean, the Dean of Deansville University", "190472@Decaying Mummy", "112574@DeciN8r", "191781@Decreased Alcohol Tax Act", "191780@Decreased Alcohol Tax Bylaws", "117303@Dedicated Airship Hoist", "117303@(A_Clash_of_Couriers) Dedicated Airship Hoist", "132401@Deep Blue Ship Blueprints", "112816@Deep-Sea Dragonfish", "191882@Deke's Indispensible Dockyard Crate", "191397@Delicatesseur", "191623@Delores the Diplomat", "192029@Demijohn of Milk", "111177@Democratically Elected Minister", "191760@Demonstration Ban", "192441@Dennis Brammen, the Food Critic", "192226@Depleted Treasure Chest", "190547@Diamond", "124464@Diary of Mekdem's Guard", "24160@(Eden_Burning) Dietary Advice", "24160@Dietary Advice", "191586@Digger", "190513@Dionysian Mosaic", "191107@Dire Wolf", "191380@Distiller", "190616@Diver", "125144@Djimon, the Melancholic Expatriate", "191881@Dockyard Tool System", "191879@Dockyard Toolbox", "191880@Dockyard Toolchest", "191078@Dog Fox", "116142@Dog-Breeder", "191057@Domestic Cat", "191060@Domestic Dog", "191065@Domestic Pig", "112370@DomiN8r", "191061@Donkey", "191965@Double Canister Shot", "190819@(A_Clash_of_Couriers) Double Iron Grillwork", "190819@Double Iron Grillwork", "136825@Double Redundancy", "190693@(A_Clash_of_Couriers) Dr. Ali Al-Zahir, the Botanical Director", "190693@Dr. Ali Al-Zahir, the Botanical Director", "8347@Dr. ??igo Flores", "190410@(Pride_and_Peddlers) Dr. Maurice Slim, Former Head of Promise Trust", "190410@(A_Clash_of_Couriers) Dr. Maurice Slim, Former Head of Promise Trust", "190410@Dr. Maurice Slim, Former Head of Promise Trust", "111020@(A_Clash_of_Couriers) Dr. Mercier's Anarchy Poster", "111020@Dr. Mercier's Anarchy Poster", "110955@Dr. Mercier's Feverishly Federating Flag of the Free", "190723@(A_Clash_of_Couriers) Dr. Salvador", "190723@Dr. Salvador", "193188@Drae von Gast", "190518@Drakkar Carving", "191419@Draughtswoman", "191587@Dredger", "191337@Dressmaker", "192444@Drew Durnil, the Speculative Historian", "114359@Dried Meat", "190903@Drift Net", "116041@Driftwood Baleen Boat", "116096@Driftwood Iggaak", "190879@Drill", "191585@Driller", "120032@(Eden_Burning) Drip Coffee Pot", "124075@Dromedary", "190857@Druid's Sickle", "125065@Drum of Selamawi", "132844@Drunken Dave's Distillery Dive Leaflet", "113679@Dubious Treasure Map", "113678@Dubious Treasure Map", "191963@Dunnite Shells", "191095@Dusky Dolphin", "191890@Dynamic Loudspeaker", "1010222@Dynamite", "192013@Dynamite", "24248@Dynamite", "24248@(Eden_Burning) Dynamite", "190827@Dynamo", "191695@Eagerly's Every-Fifth-Day-a-Half-Day Act", "191053@Eagle", "112248@Eagle of the Sceptre of Capon", "191694@Early Closure Act", "191112@Eastern Elephant", "124041@Eastern Giant Snail", "193190@Eastern Water Buffalo", "112551@Echo Sounder", "111169@Eduardo Bernal, the Father of Public Relations", "191578@Eduardo the Expert Broker", "101638@Edvard's Judicial Sentencing", "125130@Elder Selassy'e", "114541@Elderberry", "1010207@(Eden_Burning) Electric Cable", "6280@(Pride_and_Peddlers) Electric Cables", "6280@Electric Cables", "124163@Elephant", "134623@Elevator", "2226@Elite Airship Gunner", "2226@(A_Clash_of_Couriers) Elite Airship Gunner", "191083@Elk", "111130@Ella Golden, The Anarchist Luminary", "192468@(A_Clash_of_Couriers) Elliot Blitz, the Security Engineer", "192468@Elliot Blitz, the Security Engineer", "190893@(Pride_and_Peddlers) Elmer's Electric Oven", "190893@(A_Clash_of_Couriers) Elmer's Electric Oven", "190893@Elmer's Electric Oven", "190457@Emperor Scorpion", "1966@Enbesan Animal Nutritionist", "125124@Enbesan Bishop", "125043@Enbesan Book-Binding Kit", "8908@Enbesan Envoy", "131387@Enbesan Fire Tamer", "124147@Enbesan Wolf", "132310@Enbesa's Bronze Age (Notes)", "132219@Enbesa's Silver Age (Notes)", "128852@Encyclopaedia Universalis Vol. IV", "192326@Endangered Butterflies", "191596@Ensign", "125120@Enthusiast Cook", "136649@Entremetier", "191624@Ermenegilda Di Mercante, Purveyor of Tall Ships.", "191567@Ernest the Educator", "125146@Eshe, the Eager Enbesan Entrepreneur", "114383@Espeletia", "111032@(A_Clash_of_Couriers) Essays on the Freedom to Love", "111032@Essays on the Freedom to Love", "135130@(Pride_and_Peddlers) Ethanol", "135130@Ethanol", "191791@Evangeliser's Tracts", "191718@Evening Curfew", "112388@Evil ExsanguiN8r", "191736@Exchange Rate Increase", "191792@Excommunication Act", "191856@(Pride_and_Peddlers) Exhaust Blower", "191856@Exhaust Blower", "24152@(Eden_Burning) Expert Agricultural Textbook", "24152@Expert Agricultural Textbook", "125112@Expert Grazer", "190737@Expert Hunter", "190749@Expert Planter", "191620@(Pride_and_Peddlers) Exporter of Goods", "191620@Exporter of Goods", "191766@Extended Schooling Act", "112369@ExtermiN8r", "112883@Extra Ore Extractor", "112887@Extra Virgin Oily Sucker", "112884@Extractor of Ore-someness", "192305@(Pride_and_Peddlers) Extremely Loud Bell", "192305@(A_Clash_of_Couriers) Extremely Loud Bell", "192305@Extremely Loud Bell", "125069@Eyeleting Machine", "190871@Fabulous Feed Yard", "24155@(Eden_Burning) Factory Overcrowding Regulations", "24155@Factory Overcrowding Regulations", "24153@(Eden_Burning) Factory Regulations", "24153@Factory Regulations", "8156@Fake Fur Frame Loom", "191100@False Killer Whale", "5395@Fans", "116162@Fat Chunk Candle Maker", "120037@(Eden_Burning) Fedora", "190869@Feed Yard", "190675@(A_Clash_of_Couriers) Fellow", "190675@Fellow", "120044@(Seasons_of_Silver) Felt", "120044@Felt", "190455@Fennec", "192450@(A_Clash_of_Couriers) Feras Alsarami, the Persuader", "192450@Feras Alsarami, the Persuader", "191464@(Pride_and_Peddlers) Ferhat the Experienced Builder", "191464@Ferhat the Experienced Builder", "113800@Fernando de Faro, Coffee-Lover and All-Round Cad", "24808@(Pride_and_Peddlers) Fertiliser", "24808@Fertiliser", "191436@Field Marshall Matravers, Conductor of Armies.", "1010243@Filaments", "5392@Film Reel", "190630@Fine Cake Decorator", "191327@Fine Couturier", "125049@Fine Curing Chamber", "190870@Fine Feed Yard", "190902@Fine Fishing Net", "190881@Fine Handheld Power Auger", "191838@Fine Spyglass", "191849@Fine Steel Hull", "191848@Fine Wooden Hull", "114401@Finery", "190656@(Pride_and_Peddlers) Fire Chief Mills", "190656@(A_Clash_of_Couriers) Fire Chief Mills", "190656@Fire Chief Mills", "5393@Fire Extinguishers", "191682@(A_Clash_of_Couriers) Fire Prevention Directive", "191682@Fire Prevention Directive", "191681@(A_Clash_of_Couriers) Fire Safety Poster", "191681@Fire Safety Poster", "113905@Firefly Squid", "190759@(Pride_and_Peddlers) First-Rate Sapper", "190759@(A_Clash_of_Couriers) First-Rate Sapper", "190759@First-Rate Sapper", "1010200@(Eden_Burning) Fish", "1010200@Fish", "120042@Fish Oil", "120033@(Seasons_of_Silver) Fish Stew", "191683@(A_Clash_of_Couriers) Fizgig's Fabulous Fireproofing Failsafe", "191683@Fizgig's Fabulous Fireproofing Failsafe", "116173@Fjordbuster Mk. VI Propulsion Array", "116173@(A_Clash_of_Couriers) Fjordbuster Mk. VI Propulsion Array", "192430@Flame Hose", "192433@Flame Thrower", "191505@Flamingo", "1010235@Flour", "125115@Flower Girl", "191913@Foam Extinguisher", "24109@(Eden_Burning) Fodder Supplements", "24109@Fodder Supplements", "132092@Foghorn", "190520@Folk Tales", "190726@Fool", "191570@Footman", "191825@Fore-and-Aft Rig Sails", "190735@Forester", "24656@Forester Seed Collection (Limited Use)", "24368@Forester Seed Collection (Persistent)", "191955@Forged Steel Shells", "112365@(Pride_and_Peddlers) ForkTuN8 Mk.1", "112365@(A_Clash_of_Couriers) ForkTuN8 Mk.1", "112365@ForkTuN8 Mk.1", "112366@(A_Clash_of_Couriers) ForkTuN8 Mk.2", "112366@ForkTuN8 Mk.2", "112367@(A_Clash_of_Couriers) ForkTuN8 Mk.3 - Very ForktuN8 Indeed", "112367@ForkTuN8 Mk.3 - Very ForktuN8 Indeed", "191601@Former Corsair", "191598@Former Pirate", "191377@Former Pyrphorian Whizz", "114538@Foxglove", "190674@Franck von Lewenstein, Warmest of Hosts", "190627@Fran?ois Strindberg, Crown Jeweller", "191465@Fran?ois Thorne, The Industrial Reinforcer", "191769@(Pride_and_Peddlers) Free Press Act", "191769@Free Press Act", "191756@Freedom of Association Act", "191768@Freedom of Faith Act", "190876@Freight Elevator", "120033@Fried Plantains", "114794@Frozen Woolly Mammoth", "1010234@(Pride_and_Peddlers) Fuel", "1010208@(Eden_Burning) Fuel", "24837@Fuel", "270042@Fuel", "1010247@(Pride_and_Peddlers) Fur Coats", "1010247@Fur Coats", "190859@Furrow Plough", "1010209@Furs", "191895@Gantry Crane", "191884@Gantry Repair Crane", "192360@Gasparov's Chest of Shame", "190456@Geckos", "124159@Gelada", "191313@(A_Clash_of_Couriers) Gelatier", "191313@Gelatier", "191442@General Ewing Thomson, Planter of Flags.", "191463@(A_Clash_of_Couriers) General Foreman", "191463@General Foreman", "192030@Generous Cheese Board", "190672@Generous Innkeeper", "191334@Gennaro Garibaldi, Chocolatier by Royal Appointment", "114878@Gentian Garden", "114877@Gentian Garden", "114861@Gentian Garden", "191811@George Smith's Statute of Universal Collaboration", "192230@George's Ear Trumpet", "192232@George's Police Badge", "192233@George's Sewing Machine", "191609@Georgina Duffy, The Call your Bluffy.", "191310@Gerhard Fuchs, of the Patent Eyeglass.", "192208@Germ Resistant Soap", "136821@(Pride_and_Peddlers) Get Rich Quick Vol I: Pestilential Profit", "136821@Get Rich Quick Vol I: Pestilential Profit", "136823@Get Rich Quick Vol II: What Shape is a Banana Anyway?", "136819@(Pride_and_Peddlers) Get Rich Quick Vol III: Pretty Polyculture", "136819@Get Rich Quick Vol III: Pretty Polyculture", "136827@Get Rich Quick Vol IV: The Wasteland", "136831@Get Rich Quick Vol IX: Paid To Perform", "136835@Get Rich Quick Vol V: The Geppetto Gambit", "136838@Get Rich Quick Vol VI: Night Moves", "137631@Get Rich Quick Vol VII: Automation Nation", "136833@Get Rich Quick Vol VIII: Backlogs On The Fire", "124884@Ghebbi Diorama", "191105@Giant Deer", "112612@Giant Water Lily", "192457@Gideon Small, Guru of Pirate Gunboats", "125133@Giorgis, the Eminent Hymnodist", "124161@Giraffe", "1010241@Glass", "190905@Glass Blower", "191428@Glass Maker", "191426@Glassblower", "116164@Gl?gg Splurger", "124870@Glory of Kings", "191081@Glossy Black Cockatoo", "191062@Goat", "1010193@(Seasons_of_Silver) Goat Meat", "114371@Goat Milk", "112811@Goblin Shark", "190514@Goidelic Stone Circle", "190519@Goidelic Stone Knot", "1010249@Gold", "1010233@(Pride_and_Peddlers) Gold Ore", "1010233@Gold Ore", "137281@Gold Performance Medal", "116190@Golden Harvester From The Future", "190548@Golden War Helm", "190626@Goldsmith Gilbert", "191733@Goods Duties Reduction Act", "112697@Goose Feathers", "191469@Gordon the Master Grocer", "190451@Gorilla", "1010215@Goulash", "191564@Governess", "1010192@(Seasons_of_Silver) Grain", "1010192@Grain", "190915@Grain Seeds", "1010248@Gramophones", "191374@Gramophonist", "192008@Grand Trading Charter", "190896@Grandiloquent Copper Distiller", "190925@Grape Vines", "120014@Grapes", "192166@Grapeshot Cast Mortar", "192197@Grave Human Skull", "191744@(A_Clash_of_Couriers) Graveyard Relocation Measures", "191744@Graveyard Relocation Measures", "115470@Great Auk", "111107@Great Star Coral", "191991@Great White Flag", "191099@Great White Shark", "112381@Greedy Mitts", "192001@Greek Fire", "190466@Green Turtle", "191689@Green's Law to Postpone The Apocalypse", "124151@Grey Crowned Crane", "190446@Grey Wolf", "191590@Grigor the Geologist", "116167@Gritty Gas Extractor", "191466@(A_Clash_of_Couriers) Grocer", "191466@Grocer", "190746@Grower", "112380@Grubby Hands", "111119@Guayacan de Manizales", "190680@Guide", "114667@Gungurru", "191432@Gunner", "191597@Gunsmith", "112880@Gyroscopic Gold Panner", "192186@Gyroscopic Torpedo", "113715@Haha, you're dead meat", "102179@Half-Frozen Rats", "110942@(A_Clash_of_Couriers) Hall of Collective Property", "110942@Hall of Collective Property", "110944@(A_Clash_of_Couriers) Hall of Mutualist Ideologies", "110944@Hall of Mutualist Ideologies", "191919@Hand-Blown Grenade", "191414@Handicrafter", "191616@Handler", "136712@(Pride_and_Peddlers) Hand-Pollination Method", "136712@Hand-Pollination Method", "191626@Hans Eichendorf, A Wholesale Success", "191854@Hans Gun", "191324@Hans Klein, Old Grandfather Time", "190748@Happy Homesteader", "190910@Harker's Electric Treadle Lathe", "192405@Harlow Family Heirloom", "192222@Harlow Family Ring", "191708@Harvest Fayre Flyer", "190685@Harvester", "191342@Hat Maker", "191344@Hatter", "191566@Headmistress", "191727@Headteacher's Charter", "191921@Heat Activated CTC Grenade", "191857@Heat Exchanger", "116166@Heated Jacket Of Pouchiness", "191685@Heavy Coal Permit", "24286@Heavy Drill", "24286@(Eden_Burning) Heavy Drill", "114886@Hedge Maze", "114885@Hedge Maze", "114865@Hedge Maze", "190540@Heimskringla", "190840@Helical Planer", "840@(Pride_and_Peddlers) Helium", "840@Helium", "2223@Helium Expert", "112815@Helmet Jellyfish", "114543@Hemp", "190705@Henri Zanchi, Man of Steel", "191317@Herbal Hygienist", "24320@Herbarium", "5383@Herbs", "191581@(Pride_and_Peddlers) Herdsman", "191581@(A_Clash_of_Couriers) Herdsman", "191581@Herdsman", "191319@Herv? Savonne, Suppressor of Grime", "111120@Hevea Tree", "114860@Hibiscus Garden", "114876@Hibiscus Garden", "114875@Hibiscus Garden", "114364@Hibiscus Petals", "114390@Hibiscus Tea", "116087@Hide Scraper", "191743@Hieromonk Bede's Cheeky Little Tithe Law", "191724@High Fees Law", "191839@High-Powered Scope", "116186@(A_Clash_of_Couriers) Hilarious Helium Party Balloon Pump", "116186@Hilarious Helium Party Balloon Pump", "124158@Hippopotamus", "191753@(A_Clash_of_Couriers) Historical Preservation Act", "191753@Historical Preservation Act", "125046@Hive Smoker", "114795@HMS Hades Replica", "113359@Hobby Diver", "113792@Hogarth the Harbourmaster, Veteran Of Trelawney", "125063@Hollow-Tree Drum", "114678@Holy Moly", "190919@Hop Seeds", "1010194@Hops", "124878@Hor Statue", "191063@Horse", "190692@Horticulturalist Hermann", "25506@(Seasons_of_Silver) Hot Sauce", "25506@Hot Sauce", "190725@(Pride_and_Peddlers) House Doctor", "190725@(A_Clash_of_Couriers) House Doctor", "190725@House Doctor", "191569@Housemaid", "191924@Huge Alcohol Crate", "191927@Huge Bag of Money", "112373@(A_Clash_of_Couriers) Human IncarN8 Nr. 9", "112373@Human IncarN8 Nr. 9", "112698@Huskies", "112703@Husky Sleds", "190933@Hybrid Cane Saplings", "190945@Hybrid Caoutchouc Saplings", "190942@Hybrid Cocoa Beans", "190948@Hybrid Coffee Beans", "190951@Hybrid Corn Seeds", "190939@Hybrid Cotton Seeds", "190917@Hybrid Grain Seeds", "190927@Hybrid Grape Vines", "190921@Hybrid Hop Seeds", "191902@Hybrid Oyster Spawn", "191899@Hybrid Plantain Saplings", "190924@Hybrid Potato Sprouts", "190930@Hybrid Red Pepper Seeds", "190936@Hybrid Tobacco Seeds", "192036@Hydraulic Crane", "191886@Hydraulic Repair Crane", "112383@Hydroelectric Wheel", "192185@Hydrostatic Torpedo", "5382@Ice Cream", "116180@Ice Maker", "111019@(A_Clash_of_Couriers) Iconography of the Human Mind", "111019@Iconography of the Human Mind", "104828@Idol of Hueyaca", "114798@Igluit", "191092@Iguaca", "190532@Illuminated Manuscript", "117698@Illuminated Script", "190625@Illustrious Gemologist", "124156@Impala", "190512@Imperial Eagle Mosaic", "113472@Imperial Figurehead", "124149@Imperial Zebra", "112371@(A_Clash_of_Couriers) ImpersoN8r Nr. 531", "112371@ImpersoN8r Nr. 531", "191732@Import Tax Reduction Act", "111126@Incendiary", "1010221@(Pride_and_Peddlers) Incendiary Grenades", "192002@Incendiary Grenades", "114368@Indigo Dye", "112368@IndiscrimiN8r", "125045@Industrial Binding Machine", "24078@(Eden_Burning) Industrial Chemist", "24078@Industrial Chemist", "1414@(Pride_and_Peddlers) Industrial Lubricant", "1414@Industrial Lubricant", "190856@Industrial Thresher", "24019@(Eden_Burning) Industrial Toxin Filter", "24019@Industrial Toxin Filter", "125127@Industrious Embroidress", "110998@Inflammable Concoction", "1010190@Influence", "191710@Inma's International Trade Fair Billboard", "191007@(A_Clash_of_Couriers) Inspector", "191007@Inspector", "136739@Interspecies Grafting", "111161@Intoxicator", "124882@Intricate Baked Mud Tile", "116089@Inuinnaq Case", "116146@Inuit Hunter", "116130@Inuit Seamster", "114796@Inunnguaq Inuksuk", "114816@Iris Garden", "114868@Iris Garden", "114867@Iris Garden", "1010227@Iron", "190820@(A_Clash_of_Couriers) Iron and Glass Grillwork", "190820@Iron and Glass Grillwork", "190825@Iron Factory Bell", "190701@Iron Founder", "190818@(A_Clash_of_Couriers) Iron Grillwork", "190818@Iron Grillwork", "190529@Iron Helm", "190891@(A_Clash_of_Couriers) Iron Oven", "190891@Iron Oven", "190533@Iron Shield", "190535@Iron Sword", "2038@(A_Clash_of_Couriers) Island Charter", "2038@Island Charter", "191621@Itsuko the International Exporter", "191082@Ivory Gull", "116098@Ivory Iggaak", "116083@Ivory Kudlik", "190683@Jaafan the Cruise Impresario", "190489@Jade Pearl", "191090@Jaguar", "191579@Jakob Sokow, The Charitable Banker", "5381@Jalea", "133183@Jam", "112558@Jar of Worms", "192241@Jean's Little Helper", "190453@Jellyfish", "114669@Jerusalem Sage", "112378@Jet Propeller ? Cyclone", "112379@Jet Propeller ? Hurricane", "112377@Jet Propeller ? Squall", "190623@Jeweller", "1010250@Jewellery", "111128@Jittery Pyromaniac", "191354@Johan the Inventor", "191427@Joiner", "190761@J?rg von Malching, Augur of the Auric", "192195@Jorgensen Trust Manifesto", "113194@J?rmungandr's Lair", "191573@Joseph Beaumont, Historic Society Founder", "191720@Journal of Common Thought", "191721@Journal of Popular Thought", "190473@Juanita the Mummy", "116179@Jukebox", "125114@Kaldi, Infuser Of Teas", "191387@(A_Clash_of_Couriers) Kantyi of the Quinoa", "191387@Kantyi of the Quinoa", "116088@Kaputaq Awl", "131014@Ketema's Cousin", "125055@Ketema's Industrial Kiln", "121020@Khadija Tsegaye, Angereb's Market Master", "124372@Kidusi Anitoni Port Registry", "124885@Kidusi Church Diorama", "190741@(Pride_and_Peddlers) Kiln Keeper", "190741@Kiln Keeper", "124624@King Wag's Last Testament", "190459@Kingfisher", "112390@(A_Clash_of_Couriers) Kiosk of Flighty Fortunes", "112390@Kiosk of Flighty Fortunes", "191396@Kitchen Help", "191341@(Pride_and_Peddlers) Knitter", "191341@Knitter", "191104@Kolponomos", "111178@Krapotkin, Evolutionist of Kindness", "119987@Kyria's Old Diary", "191698@La Condesa Del Amor's Majestic Dance Billboard", "111003@La Tribune Libre", "135129@Lacquer", "191615@(Pride_and_Peddlers) Lader", "191615@Lader", "190668@Lady Banes, Proponent of Protectionism.", "191731@Lady Cerebellum's Ickle Bickle Classrooms Charter", "191728@Lady Dumbedown's Colossal Class Size Charter", "191823@Lady Hunt's Advice on Being A Patient Spouse", "191773@Lady Hunt's Order of Silence", "191329@Lady Jane Smythe, Queen of Haute Couture", "192449@Lady Lu, the Redeemed Pirate", "191111@Lake Orangutan", "191625@Lalla Mazigh, Merchant of a Free Market", "113363@Lamie The Wistful Pirate", "24079@(Eden_Burning) Landscape Planner", "24079@Landscape Planner", "117699@Lanterns", "190624@Lapidary", "24048@(Eden_Burning) Large Communal Garden", "24048@Large Communal Garden", "112566@Large Looting Net", "191013@(Pride_and_Peddlers) Lateen Style Sails", "191013@Lateen Style Sails", "191748@(A_Clash_of_Couriers) Lax Water Treatment Law", "191748@Lax Water Treatment Law", "111002@Le Tocsin", "191376@Leading Electrical Engineer", "114428@Leather Boots", "190639@Lector", "190676@(Pride_and_Peddlers) Lecturer", "190676@(A_Clash_of_Couriers) Lecturer", "190676@Lecturer", "190854@Leghold Trap", "133185@(Pride_and_Peddlers) Lemonade", "133185@Lemonade", "191575@Lender", "192224@Length of Rope", "191308@Lens Designer", "191986@Letter of Immunity", "191987@Letter of Marque", "1010208@Light Bulbs", "114880@Lily Garden", "114879@Lily Garden", "114862@Lily Garden", "191328@Lily the Fashion Designer", "190617@Line Fisherman", "191373@Lineman", "125140@Lineman", "114391@Linen", "114365@Linseed", "124162@Lion", "124871@Lion of Selamawi", "190462@Lionfish", "112818@Lion's Mane Jellyfish", "112561@Little MagN8", "191582@(A_Clash_of_Couriers) Livestock Farmer", "191582@Livestock Farmer", "118728@Lobster", "125136@Lobsterman", "535@(A_Clash_of_Couriers) Local Mail", "535@Local Mail", "2832@Local Stamp", "190904@Long Range Bottom Trawl", "191782@Lord Farquahar's Alcohol Tax Amendment", "191686@Lord Footprint's Gargantuan Smokestack Act", "191608@Lord High Admiral Nadasky, The Astute.", "191737@Lord Stringer's Stringent Usury Laws", "191785@Lord Wainthrop's Booze Taxationations Most Severe!", "115980@Lost Expedition Scrap", "190724@(A_Clash_of_Couriers) Louis P. Hecate, Arm-Puncturing Pioneer", "190724@Louis P. Hecate, Arm-Puncturing Pioneer", "190845@Lovelyman's Grinding Machine", "125113@Loving Cattleherd", "191793@Lucius' Long-Lost Doctrine of the Faith", "104537@Luggage Size Regulations", "190733@(Pride_and_Peddlers) Lumberjack", "190733@Lumberjack", "190493@Lungnibbel Ring", "191416@Machinist", "1948@Mad Mary, Dynamite Enthusiast", "192356@Mad Monkey's Incisor", "136697@Madame Blanc, Ma?tre d'h?tel", "191814@Madame Kahina's Executive Tourism Drive", "125128@Magan, the Master Craftsman", "191375@Magnetist", "190899@Magnifying Glass", "190898@Magnifying Glasses", "113364@Mai Kalan, Descryer Of Brilliant Jewels", "190855@Maimer of Mustelids", "111047@Major Cyclideon Exhibition: Literature", "111046@Major Cyclideon Exhibition: Science", "191795@Major Income Tax Amendment", "191813@Major Tourism Campaign", "191805@Malching's Back-to-Back Itinerary for Serfs", "1010236@Malt", "191739@Mandatory Mass Law", "190463@Manta Ray", "129248@Map from a Recent Shipwreck", "113192@Map of a Barrier Reef", "113190@Map of a Coral Garden", "113197@Map of a Domestic Skirmish", "112937@Map of a Forgotten Wreck", "115482@Map of a Ghost Ship Sighting", "115476@Map of a Huddlesome Bay Trading Post", "113188@Map of a Kelp Forest", "129239@Map of a Lost Shepherds' Camp", "112934@Map of a Lost Temple", "113198@Map of a Rebel Incursion", "112936@Map of a Remote Trading Post", "113189@Map of a Seagrass Meadow", "115478@Map of a Seal Feeding Ground", "112939@Map of a Sunken City", "129243@Map of a Sunken Estate", "113202@Map of a Sunken La Corona Clipper", "113201@Map of a Sunken Rebel Supply Ship", "113200@Map of a Sunken Royal Clipper", "113199@Map of a Trelawney Skirmish", "115475@Map of an Abandoned Inuit Village", "113193@Map of an Abyssal Zone", "129222@Map of an Ancient Naval Battlefield", "129240@Map of an Ancient Outpost", "115479@Map of an Arctic Basin", "115481@Map of an Inuit Dispute", "113191@Map of an Oceanic Trench", "112938@Map of an Underwater Pyramid", "112933@Map of Ancient Ruins", "113204@Map of Isabel Sarmento's Emergency Vault", "115483@Map of Old Nate's Prototype Airship Crash Site", "118634@Map of Sile'Amidos", "113205@Map of Vicente Silva's Sunken Ship-of-the-Line", "113203@Map of Willie Wibblesock's Wondrous Stash", "129237@Map Provided By Imperial Sailors", "129249@Map Taken from an Insurance Report", "191408@Marcel Forcas, Celebrity Chef", "191371@Marco de la Mocha, Crusher of Beans", "8351@Maria Maravilla", "192460@Maria Pizarro, Maker of Pirate Monitors", "191346@Mariana the Master Stylist", "190632@Marie-Antoine, P?tissier Royale", "136647@Marie-Louise Car?me, Chef de Cuisine", "114882@Marigold Garden", "114881@Marigold Garden", "114863@Marigold Garden", "24064@(Eden_Burning) Marine Biologist", "24064@Marine Biologist", "113356@Marine Biologist", "191584@Mark van der Mark, Breeder of Shepherd Dogs", "112733@Marsh-Mallow", "191460@Mason", "112563@Massive MagN8", "191356@Master Confectioner", "192039@Master Craftsman Franke", "191430@Master Craftsman Morris", "190704@(Pride_and_Peddlers) Master of the Forges", "190704@Master of the Forges", "124876@Masu Masu's Trinket", "124877@Masu's Mapamundi", "191445@Mate", "24087@(Eden_Burning) Mathematician", "24087@Mathematician", "192331@Mathilde the Parrot", "191388@Maxime Graves, Delicatesseur Extraordinaire", "192438@Maxime Renard the One-Man Show", "114662@Meadow Sage", "192212@Meaningless Bling", "125048@Meat Guillotine", "191413@Mechanic", "190880@(Pride_and_Peddlers) Mechanical Drill", "190880@Mechanical Drill", "190883@Mechanical Loom", "112879@Mechanical Panner", "190906@Mechanised Glass Blower", "190751@Medal-Winning Producer", "5397@Medicine", "112565@Medium Looting Net", "115480@Megalodon's Abyss", "191888@Megaphone", "101363@Melancholic Artisan", "191962@Melinite Shells", "190848@Melting Point Apparatus", "190517@Menhirs", "191617@Merchant", "111190@Mercier's Cookbook", "111189@Mercier's Manifesto of the Free", "111187@Mercier's Megaphone", "191790@(A_Clash_of_Couriers) Mercier's Separate Church and State Clause", "191790@Mercier's Separate Church and State Clause", "190847@Mercury Thermometer", "190501@Mesoan Calendar", "190500@Mesoan Glyphs", "190498@Mesoan Harbour", "190497@Mesoan House", "190499@Mesoan Temple", "112731@Metaxya Fern", "6600@Mezcal", "114673@Mezoan Horn Cone", "190760@(A_Clash_of_Couriers) Micaela the Mining Engineer", "190760@Micaela the Mining Engineer", "190913@(Pride_and_Peddlers) Mickleton's Hydraulic Compacting Press", "190913@(A_Clash_of_Couriers) Mickleton's Hydraulic Compacting Press", "190913@Mickleton's Hydraulic Compacting Press", "24081@(Eden_Burning) Microbiologist", "24081@Microbiologist", "191717@Midnight Curfew", "191446@Midshipman", "191572@(Pride_and_Peddlers) Miles the Master Butler", "191572@(A_Clash_of_Couriers) Miles the Master Butler", "191572@Miles the Master Butler", "1895@Military Logistician", "5385@Milk", "192484@(A_Clash_of_Couriers) Millicent's Manifesto of the Suffragette", "192484@Millicent's Manifesto of the Suffragette", "191349@(Pride_and_Peddlers) Milliner", "191349@Milliner", "190843@Millstone", "192231@Mined Quartz", "190755@Miner", "2224@Mineralogist", "5398@Minerals", "191765@Minimum Schooling Act", "191763@Minimum Working Age 6 Act", "191762@(Pride_and_Peddlers) Minimum Working Age 8 Act", "191762@Minimum Working Age 8 Act", "111044@Minor Cyclideon Exhibition: Agriculture", "111045@Minor Cyclideon Exhibition: Industry", "191794@Minor Income Tax Amendment", "191812@Minor Tourism Campaign", "192299@(Pride_and_Peddlers) Miraculous Steel Plough", "192299@Miraculous Steel Plough", "191796@Miserable Max's Brutal Tax Legislation", "191767@Miss Jorgensen's Exemplary School-to-16 Proposal", "190738@(Pride_and_Peddlers) Miss Rodriguez, the Conservationist", "190738@Miss Rodriguez, the Conservationist", "191395@Mixer", "190650@Mme. Elise The Nose Bouquet", "191784@Moderate Alcohol Tax", "191778@Moderation Act", "191978@Modern Ship Tools", "191345@Modiste", "191386@Mole Master", "190282@Monkey-Tooth Amulet", "114665@Moon Orchid", "112549@Moon Pool Diving Bell", "191379@Moonshiner", "111163@Mordant Conservative Playwright", "114657@Morning Glory", "191481@Mortar", "192947@Morticromicron", "190505@Mosasaurus Fossil", "125059@Mosquito Net", "124873@Mother and Child Figure", "190621@Mother of Pearls", "114671@Mother-In-Law's Tongue", "5390@Motor", "191459@Moulder", "124152@Mountain Nyala", "136668@Mr. Bertram, Hotel Manager", "111167@Mr. Garrick, Founder of the Entrepreneur Gentlemen's Club", "191746@(A_Clash_of_Couriers) Mr. Slippery's Splendiferous Sewage Proposal", "191746@Mr. Slippery's Splendiferous Sewage Proposal", "190697@Mr. Swell, the Hydrodynamist", "191749@(A_Clash_of_Couriers) Mrs. Beetle's Repulsive Law on Pestilence", "191749@Mrs. Beetle's Repulsive Law on Pestilence", "190691@Mrs. Brown the Cultivator", "191407@Mrs. Mayson, The Very Good Housekeeper", "191799@Mrs. O'Mara's Marvellous Tax Relief Regulations", "191779@Ms. Amelia's Essential Prohibition Measures", "191692@(A_Clash_of_Couriers) Mssr. Fontaine's Street Signage Solution", "191692@Mssr. Fontaine's Street Signage Solution", "114402@Mud Bricks", "125134@Mulatu, the Musical Maestro", "191869@Mulcher M1897", "192168@Mullivann's Multi-Shot Mortar", "191834@Multiple Expansion Steam Engine", "190471@Mummy Bandages", "116160@Music Box", "191076@Musk Ox", "112389@(A_Clash_of_Couriers) Mystery Vending Box", "112389@Mystery Vending Box", "112553@N.8.D.A.R", "116153@Naaqtuuq the Sled-Builder", "112243@Nadasky's Journal", "112241@Nadasky's Journal (Incomplete)", "112242@Nadasky's Journal (Near Complete)", "113473@Nadasky's Medal", "113360@Nadia the Spotter", "5401@Nandu Feathers", "5384@Nandu Leather", "115273@Narwhal", "116169@Nate's Darling Kettle", "112392@(A_Clash_of_Couriers) Nate's Experimental Illuminations", "112392@Nate's Experimental Illuminations", "112393@(A_Clash_of_Couriers) Nate's Magically Morphomatic Rockets", "112393@Nate's Magically Morphomatic Rockets", "190696@Naval Architect", "190496@Nazcan Tomb", "190475@Neanderthal Skeleton", "125056@Net Trap", "4270@New World Science Reporter", "112520@Nice Scrap", "191688@Night and Day Pollution By-Law", "191687@Night Pollution By-Law", "192229@Non-Functional Sea Mine", "191693@Noon Closure Act", "191103@Norfolk Kaka", "190720@Nurse", "116069@Nuyakpak", "121023@Ocean Plotting Sheet of the Southern Hemisphere", "112568@Oceanic Dredge", "1010566@Oil", "112702@Oil Lamps", "112885@Oily Sucker", "112886@Oily Super-Sucker", "190824@(A_Clash_of_Couriers) Old Factory Bell", "190824@Old Factory Bell", "190508@Olympian Temple Remains", "111038@(A_Clash_of_Couriers) On Civil Disobedience", "111038@On Civil Disobedience", "191822@On the Inferior Nature of Women", "191696@One-Man-Band Billboard", "191830@(Pride_and_Peddlers) One-Turn Screw Propeller", "191830@One-Turn Screw Propeller", "191307@Optician", "192319@Optimised Automatic Loom", "136741@(Pride_and_Peddlers) Optimized Pruning", "136741@Optimized Pruning", "191309@Optometrist Otto", "191101@Orangutan", "190450@Orca", "5386@Orchid", "112882@Ore Extractor", "112485@Orichalcum Statue", "190907@Oriel's Automatic Blowing Machine", "111115@Oriental Cherry", "192219@Oriental Fireworks", "114666@Oriental Tassel Fern", "136203@Original Vanity Screen Advertisement", "117701@Ornate Candles", "113358@Orvette Charles, of The Royal Society of Oceanographers.", "124153@Oryxe", "191503@Ostrich", "116148@Outdoor Cook", "116161@Outdoorsy Grill", "191064@Ox", "190826@(A_Clash_of_Couriers) Oxford Electrical Bell", "190826@Oxford Electrical Bell", "191900@Oyster Spawn", "124868@Painted Mask", "190481@Painted Stones", "133095@(Eden_Burning) Palm Tree", "191404@Pantry Chef", "191398@Pantry Maid", "191817@Papal Paper of Prenatal Preservation", "117702@Paper", "112735@Papyrus", "191741@Parish Tax Law", "190742@Park Ranger", "112700@Parkas", "190007@Parrot", "116174@Particularly Pull-y Pulley", "116174@(A_Clash_of_Couriers) Particularly Pull-y Pulley", "190629@Pastry Chef", "191312@(A_Clash_of_Couriers) Pastry Cook", "191312@Pastry Cook", "190861@(A_Clash_of_Couriers) Patent Steel Seed Drill", "190861@Patent Steel Seed Drill", "125123@Pater Dekama", "190631@P?tissier Patrice", "191740@(A_Clash_of_Couriers) Patriarch Grigor's Calendar of Holy Days", "191740@Patriarch Grigor's Calendar of Holy Days", "125125@Patriarch Matteos X", "111034@(A_Clash_of_Couriers) Patriarchy, the Root of Class Struggle", "111034@Patriarchy, the Root of Class Struggle", "125138@Paul Poiret, Shocking Fashion Designer", "191079@Peacock", "190618@(Pride_and_Peddlers) Pearl Diver", "190618@Pearl Diver", "1010256@Pearls", "191467@Peddler", "190689@(Pride_and_Peddlers) Pedologist", "190689@Pedologist", "8356@Pedro, Captain of the Panama", "191080@Pelican", "112705@Pemmican", "191908@Penicillin", "1010245@Penny Farthings", "110948@(A_Clash_of_Couriers) People's Jury Hall", "110948@People's Jury Hall", "114542@Peppermint", "5388@Perfumes", "191318@Perfumier Prunella", "112611@Perry's Giant Sunburst", "190829@Pertwee's Turbo Generator", "1010017@(Pride_and_Peddlers) Pes?os", "113355@Peter Flinders, Conservator of Artefacts", "192458@Petra Piper, Fashioner of Pirate Frigates", "118594@Phoenix Egg", "110999@Phosphorus Dissipation Recipe", "191352@Physicist", "191697@Piano Concerto Billboard", "190686@(Pride_and_Peddlers) Picker", "190686@Picker", "111180@Pietro Jonah Proud, The Philosopher of the Public Good", "191054@Pigeon", "5400@Pigments", "1010199@Pigs", "112732@Pillar Coral", "191449@Pilot", "190962@Pink River Dolphin", "114668@Pink Sugarbush", "192172@Pirate Captain's Black Flag", "114799@Pirujaqarvik Inuksuk", "116078@Pititsi", "190839@Planer", "191897@Plantain Saplings", "120041@(A_Clash_of_Couriers) Plantains", "120041@(Eden_Burning) Plantains", "120041@Plantains", "191858@Plate Heat Exchanger", "190744@Ploughman", "190734@Poacher", "1010246@(Pride_and_Peddlers) Pocket Watches", "1010246@Pocket Watches", "104518@Poetry Subscription", "190448@Polar Bear", "5394@Police Equipment", "191759@Political Club Dissolution Order", "118103@Polyglot Scholar Rahim", "112247@Pommel of the Sceptre of Capon", "120043@(Seasons_of_Silver) Ponchos", "120043@Ponchos", "190719@Pope Lucius I, The Rejuvenator", "192155@Poppinjay's Pulleyed Patcher-Upper", "114661@Poppy", "110949@(A_Clash_of_Couriers) Popular People's Court", "110949@Popular People's Court", "191619@Port Dealer", "190889@Porthampton Mass Conveyor", "191577@Postal Banker", "112144@(A_Clash_of_Couriers) Poster of the Leader", "112144@Poster of the Leader", "191399@Potager", "190922@Potato Sprouts", "1010195@(Seasons_of_Silver) Potatoes", "1010195@Potatoes", "191690@(A_Clash_of_Couriers) Potholes Decree", "191690@Potholes Decree", "125054@Potter's Wheel", "190884@(Pride_and_Peddlers) Power Loom", "190884@Power Loom", "116132@Practised Inuit Seamster", "192361@Precious Royal Jewellery", "191322@Precision Horologist", "190465@Prickly Sea-Devil", "114674@Pride of Madeira", "190712@Priest", "190469@Primitive Fabrics", "110938@(A_Clash_of_Couriers) Printing Press", "110938@Printing Press", "111024@Proclamation of Knighthood", "190679@Prof. Iwa Ebashi, Pioneer of the Radioactive", "191355@Prof. Ram Devi, The Bundle of Energy", "190678@Professor Razzaq", "190756@(Pride_and_Peddlers) Prospector", "190756@Prospector", "112881@Prospector Of Auspiciousness", "102491@Prosperity Contract", "190549@Protoceratops Eggs", "192184@Prototype Torpedo", "24159@(Eden_Burning) Public Campaign: Eat those Proteins!", "24159@Public Campaign: Eat those Proteins!", "190671@Publican", "191089@Puma", "190722@(A_Clash_of_Couriers) Punctilious Physician", "190722@Punctilious Physician", "114381@Purple Pitcher Plant", "111116@Purple Saxifrage", "124074@Pygmy Hedgehog", "191097@Pygmy Killer Whale", "190487@Pyrite Crystals", "24162@(Eden_Burning) Pyrphorian Calciner", "24162@Pyrphorian Calciner", "192434@Pyrphorian Flame Thrower", "24164@(Eden_Burning) Pyrphorian Machine Instructions", "24164@Pyrphorian Machine Instructions", "116042@Qajak", "116314@QI_Treasure_Dummy", "116313@QI_Treasure_Dummy", "116312@QI_Treasure_Dummy", "113112@QI_Treasure_Dummy", "113109@QI_Treasure_Dummy", "113106@QI_Treasure_Dummy", "113105@QI_Treasure_Dummy", "113104@QI_Treasure_Dummy", "113103@QI_Treasure_Dummy", "113102@QI_Treasure_Dummy", "113101@QI_Treasure_Dummy", "113100@QI_Treasure_Dummy", "113099@QI_Treasure_Dummy", "113098@QI_Treasure_Dummy", "113097@QI_Treasure_Dummy", "112959@QI_Treasure_Dummy", "112958@QI_Treasure_Dummy", "129413@QI_Treasure_Dummy", "129412@QI_Treasure_Dummy", "129411@QI_Treasure_Dummy", "129410@QI_Treasure_Dummy", "129408@QI_Treasure_Dummy", "129407@QI_Treasure_Dummy", "129406@QI_Treasure_Dummy", "129404@QI_Treasure_Dummy", "129403@QI_Treasure_Dummy", "129402@QI_Treasure_Dummy", "116320@QI_Treasure_Dummy", "116319@QI_Treasure_Dummy", "116318@QI_Treasure_Dummy", "116317@QI_Treasure_Dummy", "116316@QI_Treasure_Dummy", "116315@QI_Treasure_Dummy", "112957@QI_Treasure_Dummy", "112956@QI_Treasure_Dummy", "112955@QI_Treasure_Dummy", "112954@QI_Treasure_Dummy", "112952@QI_Treasure_Dummy", "112951@QI_Treasure_Dummy", "112946@QI_Treasure_Dummy", "192220@Qing's Eyeglasses", "192221@Qing's Volume of Poetry", "190932@Quality Cane Saplings", "190944@Quality Caoutchouc Saplings", "191332@Quality Chocolatier", "190941@Quality Cocoa Beans", "190947@Quality Coffee Beans", "190950@Quality Corn Seeds", "190938@Quality Cotton Seeds", "190916@Quality Grain Seeds", "190926@Quality Grape Vines", "190920@Quality Hop Seeds", "191901@Quality Oyster Spawn", "191898@Quality Plantain Saplings", "190923@Quality Potato Sprouts", "190929@Quality Red Pepper Seeds", "190935@Quality Tobacco Seeds", "191589@Quarry Foreman", "191588@(Pride_and_Peddlers) Quarryman", "191588@Quarryman", "190522@Quartz Crystals", "1010228@Quartz Sand", "114385@Queen of the Andes", "125047@Queenby Apiary Suit", "191801@Queen's Birthday Decree", "6055@Quipu", "191909@Rabies Vaccine", "112385@RacioN8d Super Spinner", "116158@Radiator", "191896@Railroad Crane", "191885@Railway Repair Crane", "191702@Rare Artifacts Pamphlet", "190467@Rats", "121022@Rear Admiral Richard B. Sharpe, The Eagle-Eyed", "191400@Recipe Archivist", "24086@(Eden_Burning) Recycling Expert", "24086@Recycling Expert", "24082@(Eden_Burning) Recycling Specialist", "24082@Recycling Specialist", "190447@Red Deer", "191068@Red Fox", "190928@Red Pepper Seeds", "1010198@Red Peppers", "111109@Red Tiger Lotus", "114859@Red White Petunia Garden", "114874@Red White Petunia Garden", "114873@Red White Petunia Garden", "24108@(Eden_Burning) Refined Fertiliser", "24108@Refined Fertiliser", "24113@(Eden_Burning) Refined Fodder Supplements", "24113@Refined Fodder Supplements", "24033@(Eden_Burning) Refined Industrial Toxin Filter", "24033@Refined Industrial Toxin Filter", "24036@(Eden_Burning) Refined Water Distiller", "24036@Refined Water Distiller", "190758@Refiner", "116178@RefridgerN8r", "190550@Regal Gold Ring", "536@(A_Clash_of_Couriers) Regional Mail", "536@Regional Mail", "119989@Register of the IXth Regiment", "104173@Registration Confirmation: Stage One", "104175@Registration Confirmation: Stage Three", "104174@Registration Confirmation: Stage Two", "191850@Reinforced Bulkhead", "1010202@(Eden_Burning) Reinforced Concrete", "1010202@Reinforced Concrete", "1010017@(Seasons_of_Silver) R?is", "191729@Religious Schooling Charter", "113353@Renowned Excavator", "191417@Repairman", "136746@Re-Rendering", "103977@Researcher's Old Map", "135086@Resin", "190648@Respected Oenologist", "190831@Reverberatory Furnace", "111123@Rhododendron", "112255@Richardson's Log", "191757@Right of Protest Act", "191084@Ringed Seal", "191809@Rioter Amesty Law", "191810@Rioter Immunity Act", "191368@Roaster", "1517@Rocket Barrage", "1516@Rocket Salvo", "112245@Rod of the Sceptre of Capon", "191583@(Pride_and_Peddlers) Rodrigo the Ranchero", "191583@Rodrigo the Ranchero", "190698@Rohit Bhargava, the Naval Architect", "116175@Royal Airship Stamp Mimic", "116175@(A_Clash_of_Couriers) Royal Airship Stamp Mimic", "111188@Royal Crown", "102803@Royal Letter", "191992@Royal Letter of Immunity", "191993@Royal Letter of Marque", "111122@Royal Palm", "192012@Royal Trading Charter", "190492@Rudimentary Necklace", "116151@Rugged Outdoor Cook", "1010257@(Seasons_of_Silver) Rum", "1010257@Rum", "190646@Rum Blender", "190644@Rum Runner", "190521@Rusty Sestertius", "191715@Sabbath Repeal Law", "111106@Sacred Blue Lily", "190480@S?mundar Edda", "191750@(A_Clash_of_Couriers) Safer Roofs Fund", "191750@Safer Roofs Fund", "190877@Safety Elevator", "191444@Sailor", "1010210@(A_Clash_of_Couriers) Sails", "1010210@Sails", "190193@Saint D'Artois, Vision of the Valley", "116071@Sakku", "191468@Sales Clerk", "191622@Salima the Shipping Operator", "114358@Salt", "1010232@Saltpetre", "8355@Salvador, the Stalwart", "190510@Sanctuary of Hellebore", "116154@Sandro the Nature Chef", "114357@Sanga Cow", "190506@Santillana Cave Painting", "191747@(A_Clash_of_Couriers) Saponification Tax", "191747@Saponification Tax", "190732@Sarah Bartok, The Golden Voice", "1010238@Sausages", "190666@Savvy Customs Officer", "190665@Scammer", "113357@Scara the Marine Biologist", "1010216@(Seasons_of_Silver) Schnapps", "1010216@Schnapps", "191726@School Discipline Charter", "191565@Schoolteacher", "191353@Science Whizz", "5391@Scooter", "112518@(A_Clash_of_Couriers) Scrap", "112518@Scrap", "112857@Scrap", "114658@Sea Heart Liana", "110983@Sea Mine", "114670@Sea Thrift", "114410@Seafood Stew", "190006@Seagull", "190458@Seahorses", "112696@Seal Skin", "114656@Sea-lavender", "191343@(Pride_and_Peddlers) Seamstress", "191343@Seamstress", "125135@Seasoned Salter", "191789@(A_Clash_of_Couriers) Secularised Bylaws", "191789@Secularised Bylaws", "191431@Seraphim Papadikas, The Window Dresser", "112391@(A_Clash_of_Couriers) Serendipitous Tombola of Inexplicable Curiosity", "112391@Serendipitous Tombola of Inexplicable Curiosity", "191006@(A_Clash_of_Couriers) Sergeant", "191006@Sergeant", "191599@Sergeant", "1010206@Sewing Machines", "192034@Sewing Threads", "190269@(A_Clash_of_Couriers) Sgt. Nicolas Gabriel, the Over-Achiever", "190269@Sgt. Nicolas Gabriel, the Over-Achiever", "133181@(Pride_and_Peddlers) Shampoo", "133181@Shampoo", "116150@Sharp Sled Builder", "125119@Shayi Shaah, Tea House Empress", "191066@Sheep", "114152@Sheet Music Anno 1404", "112487@Sheet Music Anno 1701", "114155@Sheet Music Anno 1800", "114153@Sheet Music Anno 2070", "114154@Sheet Music Anno 2205", "191580@Shepherd", "192459@Shih Yang, Sirer of Pirate Ships-of-the-Line", "192194@Shiny Pearls", "190695@Ship Builder", "191833@Ship Piston Engine", "192152@Ship Tool Chest", "192153@Ship Tool Trolley", "192151@Ship Toolbox", "191977@Ship Tools", "191832@(Pride_and_Peddlers) Ship Turbine", "191832@Ship Turbine", "191618@Shipper", "191602@Ship's Chaplain", "129251@Shipwrecked Captain's Map", "190546@Shroud of the Great King", "190537@Silken Fabrics", "192227@Silva's Tortilla", "24846@Silver Amalgam", "24850@Silver Bars", "24851@Silver Coins", "190538@Silver Helmet", "190530@Silver Necklace", "1010233@(Seasons_of_Silver) Silver Ore", "137286@Silver Performance Medal", "190541@Silver Ring", "190542@Silver Shield", "190544@Silver Sword", "191102@Silverback Gorilla", "190878@Simmons Electrifying Elevator", "104827@Simon Sarmento's Sword", "191851@Singh's Steel Double-Hull", "191734@Sir Archie's Royal Minty Printy Thingy", "191010@(A_Clash_of_Couriers) Sir Charles Rafferty, Metropolitan Commissioner", "191010@Sir Charles Rafferty, Metropolitan Commissioner", "192443@Sir Lewis Brindley, the Chemist", "192464@Sir Milfoy Credenza-Belfry-Guscott, HM Ship Architect", "192404@Slave Owner's Ledger", "116147@Sled Builder", "112704@Sleds", "112701@Sleeping Bags", "192463@Slinky Sinistra, Warlock of Pyrphorian Warships", "125062@Sluggish Surge-Wheel", "190874@Slurry Pit", "190873@Slurry Tank", "191783@Small Alcohol Tax", "112564@Small Looting Net", "116177@Smoke Detector", "112734@Snow Gum", "1010203@Soap", "191316@Soap Maker", "116082@Soapstone Kudlik", "5803@Soccer Balls", "191914@Soda-Acid Extinguisher", "125117@Soft Drink Seller", "24012@(Eden_Burning) Soil Sampler", "24012@Soil Sampler", "125070@Sole-Searcher's Stitcher", "190649@Sommelier Raymond", "190484@Song of Ilion", "190485@Song of Odysseus", "190875@Sophisticated Slurry Lagoon", "136648@Sous-Chef", "191070@Southern Gull", "190681@Souvenir Seller", "133532@Souvenirs", "125061@Spare Wheel", "191351@Sparky", "191730@Spartan Schooling Charter", "124694@Spear of Selamawi", "124124@Spear of Selamawi", "112523@Special Scrap", "191094@Spectacled Bear", "191306@Spectacle-Maker", "120030@Spectacles", "191098@Sperm Whale", "125131@Spice Master", "114408@Spiced Flour", "114369@Spices", "190460@Spitting Cobra", "117302@Sports Gondola", "117302@(A_Clash_of_Couriers) Sports Gondola", "191096@Spotted Dolphin", "124154@Spotted Hyena", "191824@Square Rig", "111113@Staghorn Coral", "112384@Stainless Hydroelectric Wheel", "192235@Standard Pirate Flag", "114387@Star Jasmine", "192217@Statue of a Jade Dragon", "124879@Statue of Anubis", "190543@Statue of Enesidaone", "191803@Statutory Night Work Act", "191804@Statutory Twelve Hour Day Act", "1010225@Steam Carriages", "191418@Steam Engineer", "1010224@Steam Motors", "190895@Steam Powered Brewing Machine", "190828@(Pride_and_Peddlers) Steam Turbine", "190828@Steam Turbine", "1010219@Steel", "1010218@(Pride_and_Peddlers) Steel Beams", "1010218@(A_Clash_of_Couriers) Steel Beams", "1010218@Steel Beams", "190912@Steel Moulds", "190703@Steely-Eyed Steelsmith", "190503@Stegosaurus", "191591@Steven MacLeod, Geological Surveyor", "191571@Steward", "112382@Sticky Shiny Claws", "192461@Stilton Snood, Purveyor of Pyrphorian Monitors", "190534@Stone Statue", "125051@Stop Gate", "136887@Store Greeter", "191056@Stork", "192225@Strange Serum", "192238@Strange Talisman", "124874@Striking Terracotta Figure", "191855@Strongarm's Disappearing Gun", "111118@Subalpine Fir", "112552@Submarine Oscillator", "112372@(A_Clash_of_Couriers) SubordiN8 Nr. 360", "112372@SubordiN8 Nr. 360", "1010239@(Pride_and_Peddlers) Sugar", "1010239@(Seasons_of_Silver) Sugar", "1010239@Sugar", "1010251@(Pride_and_Peddlers) Sugar Cane", "1010251@(Seasons_of_Silver) Sugar Cane", "1010251@Sugar Cane", "112821@Summit of Atlantis", "191738@Sunday Best Law", "192032@Sunday Roast", "191008@(Pride_and_Peddlers) Superintendent", "191008@(A_Clash_of_Couriers) Superintendent", "191008@Superintendent", "190864@Superphosphate Fertiliser", "113799@Susannah Brightwoman, A Glimmer In The Darkness", "191420@Susannah the Steam Engineer", "191920@Suspended Frosted Glass Grenade", "192462@Suspicia Slyhood, Begetter of Pyrphorian Battle Cruisers", "191067@Swan", "191055@Swift", "191087@Swordfish", "1897@Synthetic Chemist", "191772@Taboo Books Act", "191326@Tailor", "114430@Tailored Suits", "190474@Takal's Necklace", "116144@Taliriktug The Husky-Voiced", "1010234@Tallow", "124881@Tambacto Diorama", "191336@Tanner", "114404@Tapestries", "190539@Tara Brooch", "190527@Tarnished Silver Coins", "191405@Tatiana the Taste-Tester", "192239@Tattered White Flag", "191797@Tax Rebates", "191798@Tax Reduction Legislation", "190667@Taxman Tim", "125118@Tea Selector", "191372@Technician", "191777@Teetotalers Manifesto", "114367@Teff", "191840@Telemobiloscope", "125071@Telephone Repeater", "114431@Telephones", "1956@Temperate Pedologist", "190509@Temple of Enesidaone", "124880@Temple of Nafir Diorama", "112483@Temple of Poseidon", "125064@Ten-String Krar", "190858@Teutonic Technical Secateurs", "112394@(A_Clash_of_Couriers) The Bish Bang Bibbly Experimental Display", "112394@The Bish Bang Bibbly Experimental Display", "116187@The Country Bumpkin Cigar Roller", "116184@The Don't Leave It 'Til L8r! EspressoN8r", "116188@The Magnificone Ice Cream Maker", "191704@The Valley of Death: A Treatise on the Upper Kingdoms.", "124883@The Ark of the Covenant", "190468@The Art of Medicine", "111014@The Art of Rule", "112728@The Banyan Tree", "116189@(A_Clash_of_Couriers) The Bizarro-Balloono-Magn8", "116189@The Bizarro-Balloono- Magn8", "112730@The Brazil Nut Tree", "111012@The Capitalist", "114677@The Elusive Fern Flower", "132352@The Epic of the Land of Lions", "132351@The Epic of the Land of Lions", "132350@The Epic of the Land of Lions", "132348@The Epic of the Land of Lions", "111013@The Exquisite", "111004@The Firebrand and the Free", "112940@The Gates to Paititi", "114679@The Hades Asphodel", "112941@The Herdsman's Journal", "111040@(A_Clash_of_Couriers) The Historical Sophisms of the Doctrinaire School of Communism", "111040@The Historical Sophisms of the Doctrinaire School of Communism", "118148@The Holy Book", "190885@(Pride_and_Peddlers) The Hopedale Loom", "190885@The Hopedale Loom", "111048@The International Liberal Forum on Art and Craft", "111117@The Jujube Lotus", "191722@The Kaleidoscopeum Quarterly Review", "112476@The Kraken", "113196@The Kraken's Pit", "191714@The Lord's Blessing", "112578@The Mine-o-taur", "113195@The Nest of Gukulmatz", "192234@The Old Pirate Hat", "112935@The Palaces of Baiea", "115477@The Portal to Hyperborea", "191758@The Pulltoddle Freedom of Assembly Act", "192949@The Rings Of The Kings", "112272@The Sceptre of Capon", "192482@(Pride_and_Peddlers) The Subjection of Women", "192482@(A_Clash_of_Couriers) The Subjection of Women", "192482@The Subjection of Women", "190545@The Treasure of Ben Sahid", "112375@The TurbiN8r", "116165@The Uncanny Pocket Cannery", "192359@The Von Malching Family Pennant", "192209@The Von Malching Family Seal", "190900@The Watchmaker's Loupe", "191815@The Withdrawal Amendment", "190849@Thielemann's Tube", "190684@Thomasina Langton, Promoter Extraordinaire!", "192177@Thundering Loudspeaker Network", "191088@Tiger", "114672@Tiger-Spotted Stanhopea", "1010196@(Pride_and_Peddlers) Timber", "1010196@(Seasons_of_Silver) Timber", "1010196@Timber", "110982@Time Bomb", "191716@Timothy's Blurry Law of Utter Depravity", "192035@Tinned Fish", "114675@Titan Arum", "191402@Tlayolotl Savor, King of the Corn", "192000@TNT Bomb", "1010252@(Seasons_of_Silver) Tobacco", "1010252@Tobacco", "190640@Tobacco Blender", "190934@Tobacco Seeds", "116152@Toklo the Trapper", "191735@Toll and Loan Rates Increase", "798@Tools", "114884@Topiary Avenue", "114883@Topiary Avenue", "114864@Topiary Avenue", "114888@Topiary Garden", "114887@Topiary Garden", "114866@Topiary Garden", "190642@(Pride_and_Peddlers) Torcedor Lucia", "190642@Torcedor Lucia", "191474@Torpedo", "120035@(Seasons_of_Silver) Tortillas", "120035@Tortillas", "191385@Tortillera", "114676@Tower Of Jewels", "192364@Toy Horse", "115472@Toy Qamutiik", "135231@Toys", "192007@Trading Charter", "128985@Transcript of Oracle's Etchings", "190853@Trap", "190736@Trapper", "24043@(Eden_Burning) Trash Filter", "24043@Trash Filter", "190682@Travel Agent", "113362@Treasure Seeker", "118122@Treasures of Sile'Amidos", "191770@Treatise on Freedom of Speech", "190688@Tree Surgeon", "24187@(Eden_Burning) Trees", "24187@Trees", "112386@TrepanN8r", "190502@T-Rex", "1961@(Pride_and_Peddlers) Tropical Pedologist", "1961@Tropical Pedologist", "192164@Trunnion Mortar", "112559@Tub of Tasty Tubeworms", "191448@Tugboatman", "114389@Tulsi", "125141@Tuned-In Radio Operator", "192948@Twenty-Thousand Days Around The Centre Of The Earth", "124148@Two-Horned Chameleon", "190851@Two-Man Saw", "191829@Two-Turn Screw Propeller", "135230@Typewriter", "112376@UltiN8 Power Matrix", "116099@Ulu", "116043@Umiak", "116070@Unaaq", "192415@Unflattering Portrait of Artur", "112817@Unknown Luminescent Fish", "190739@Ursula Green, Guardian of the Forests", "115473@Uummannaq Mummy", "8348@Valentina ?lvarez", "190470@Valkyrie's Helmet", "190507@Valojoulx Cave Painting", "136202@Vandalised Vanity Screen Advertisement", "191109@Vaquita", "112280@Vasco's Honourable Agreement", "129442@Vase of Sile'Amidos", "190747@Vegetable Grower", "191415@Velocipedalist", "190757@(A_Clash_of_Couriers) Ventilator", "190757@Ventilator", "190677@Very Senior Lecturer", "190653@(A_Clash_of_Couriers) Veteran Firefighter", "190653@Veteran Firefighter", "190619@Veteran Whaler", "190867@Veterinary Antibiotics", "191603@Vice-Admiral Adams", "191604@Vice-Admiral Romanov", "191605@Vice-Almirante Gon?alves", "191606@Vice-Almirante Paulista", "192363@Vicente's Informant", "190643@Victor Perfecto, Cigar Daddy", "190852@(Pride_and_Peddlers) Vincent Drag Saw", "190852@Vincent Drag Saw", "190753@Vintner", "135233@Violins", "116181@Viridescent Billiard Table", "190866@Vitamin Supplements", "190652@Volunteer Fireman", "191677@(A_Clash_of_Couriers) Von Malching's Labour Law Proposal", "191677@Von Malching's Labour Law Proposal", "191764@Von Malching's Minimum Working Age 4 Proposal", "128690@Waha Desher's Workers", "125129@Wahenoor, the Paper Mill Pro", "116040@Walrus", "132840@Walter Waterlogged's Wine Wanderings Leaflet", "114356@Wanza Timber", "191745@(A_Clash_of_Couriers) Waste Treatment Act", "191745@Waste Treatment Act", "24016@(Eden_Burning) Waste Treatment Tank", "24016@Waste Treatment Tank", "25073@Waste Water", "191320@Watchmaker", "24305@Water", "116176@Water Cooler", "24034@(Eden_Burning) Water Distiller", "24034@Water Distiller", "112736@Water Hyacinth", "111111@Water Lily", "24654@Water Purifier Filters (Limited Use)", "24373@Water Purifier Filters (Persistent)", "190651@(Pride_and_Peddlers) Water-Carrier", "190651@Water-Carrier", "1010221@Weapons", "191325@Weaver", "190700@Welder", "112699@Whale Oil", "191412@Wheelwright", "191878@White Flag", "193191@White Peacock", "190452@White Tiger", "190552@White Tooth, Sword of Rhydderch", "192357@Wibble Soc. Sorcery Association Membership and Tarot Set", "112569@Wild Abyssal Dredge", "111131@Wild Anarchist Sacchetti", "111129@Wild Anarchist Vacco", "124150@Wild Dog", "190743@Wild Frontiersman Steen", "124155@Wildebeest", "4135@Willa Cochran, Ace Pilot", "192200@Willie's Autobiography", "192201@Willie's Bar of Gold", "192203@Willie's Completely Ordinary Wooden Box", "192202@Willie's Sausage Ring", "191607@Wilma the Wild, Bearer of Barracudas.", "136886@Window-Dresser", "1010207@Windows", "190754@Wine Grower", "125145@Wise Enbesan Homesteader", "115474@Wolf Pup Mummy", "120008@(Pride_and_Peddlers) Wood", "120008@Wood", "190862@Wood Ash", "1010242@Wood Veneers", "116100@Wooden Adze", "191894@Wooden Crane", "190911@Wooden Moulds", "191883@Wooden Repair Crane", "190494@Wooden Ring", "190524@Wooden Shield", "1010197@Wool", "120043@(Eden_Burning) Work Clothes", "1010237@Work Clothes", "24154@(Eden_Burning) Working Hours Adjustments", "24154@Working Hours Adjustments", "110936@(A_Clash_of_Couriers) Writing Machine", "110936@Writing Machine", "190702@Wrought Ironsmith", "125060@Yebeba's Sturdy Greenhouse", "114858@Yellow Daisy Garden", "114872@Yellow Daisy Garden", "114871@Yellow Daisy Garden", "111110@Yellow Lotus", "190750@Yvonne the Yeowoman", "116163@Zappy Battery #2205", "1010229@Zinc" };
				static int i2 = 0;
				if (ImGui::Button("Add##3")) { std::string item = (items[i2]); sendCommand("ts.Selection.Object.ItemContainer.SetEquipSlot(0, -1)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(" + item.substr(0, item.find("@")) + ", 1)"); }
				ImGui::SameLine(90);
				ImGui::PushItemWidth(125);
				ImGui::Combo(" (GUID) to current selected ship##inputi2", &i2, items, IM_ARRAYSIZE(items), 20);

				if (ImGui::Button("Add##4")) { sendCommand("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(1)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010196, 50)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010218, 50)"); }
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

		if (ImGui::CollapsingHeader("MODDING TOOLS", 32))
		{
			if (ImGui::Button("Show/Hide File Editor")) {
				if (showModEditor) {
					showModEditor = false;
				}
				else {
					showModEditor = true;
				}
			}
		}

		ImGui::Text(" ");
		ImGui::Text("By ChrisAnd1998");
		ImGui::Text("NOTE: Show/Hide this menu with F7 key.");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		isWindowHovered = ImGui::IsWindowHovered(0);
		isItemHovered = ImGui::IsAnyItemHovered();

		ImGui::End();


		///////////////////////////////////////////////////////////////////////////////// Start Mod Editor
		if (showModEditor) {
			
			ImGui::Begin("File Editor", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
			ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open")) {
						//openfile = true;
						fileDialog.Open();
					}

					if (ImGui::MenuItem("Save"))
					{
						savefile = true;
					}

					if (ImGui::MenuItem("Quit", "Alt-F4"))
						showModEditor = false;

					ImGui::EndMenu();
					
				}
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = editor.IsReadOnly();
					if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
						editor.SetReadOnly(ro);
					ImGui::Separator();

					if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
						editor.Undo();
					if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
						editor.Redo();

					ImGui::Separator();

					if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
						editor.Copy();
					if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
						editor.Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
						editor.Delete();
					if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						editor.Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select all", nullptr, nullptr))
						editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Palette"))
				{
					if (ImGui::MenuItem("Dark palette"))
						editor.SetPalette(TextEditor::GetDarkPalette());
					if (ImGui::MenuItem("Dark palette"))
						editor.SetPalette(TextEditor::GetMarianaPalette());
					if (ImGui::MenuItem("Light palette"))
						editor.SetPalette(TextEditor::GetLightPalette());
					if (ImGui::MenuItem("Retro blue palette"))
						editor.SetPalette(TextEditor::GetRetroBluePalette());
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Language"))
				{
					//if (ImGui::MenuItem("XML"))
						//editor.SetLanguageDefinition(TextEditor::LanguageDefinition::XML());
					if (ImGui::MenuItem("C++"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
					if (ImGui::MenuItem("HLSL"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
					if (ImGui::MenuItem("GLSL"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
					if (ImGui::MenuItem("Python"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Python());
					if (ImGui::MenuItem("C"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
					if (ImGui::MenuItem("SQL"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::SQL());
					if (ImGui::MenuItem("Angel Script"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
					if (ImGui::MenuItem("Lua"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
					ImGui::EndMenu();


					


				}

				ImGui::EndMenuBar();
			}


			if (currentFile == "") {
				ImGui::Text("No file selected. [File] -> [Open]");
			}
			else {
				ImGui::Text(currentFile.c_str());
			}

			//ImGui::Text(currentFile.c_str());

			//editor.GetLanguageDefinitionName();
			

			editor.Render("TextEditor");

			

			ImGui::End();

			
		}
		//////////////////////////////////////////////////////////////////////////////////// End Mod Editor

		///////////////////////////////////////////////////////////////////////////////// Start Open File
		fileDialog.SetTitle("Open File");
		fileDialog.SetTypeFilters({ ".xml", ".lua", ".py" });

		fileDialog.Display();

		if (fileDialog.HasSelected())
		{
			//MessageBox(NULL, fileDialog.GetSelected().string().c_str(), "", MB_OK);
			//fileEdit(fileDialog.GetSelected().string());	
			currentFile = fileDialog.GetSelected().string();
			fileEdit(currentFile);
			editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
			fileDialog.ClearSelected();
		}
	
		if (savefile) {
			//MessageBox(NULL, currentFile.c_str(), "", MB_OK);
			auto textToSave = editor.GetText();
			std::ofstream outfile(currentFile);
			outfile << textToSave << std::endl;
			outfile.close();
			savefile = false;
		}
		///////////////////////////////////////////////////////////////////////////////// End Open File

		ImGui::Render();
}