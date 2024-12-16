#define _CRT_SECURE_NO_WARNINGS
#include "Cheats.h"
#include "Offsets.h"
#include "Resources/Language.h"
#include "Utils/Initial/Init.h"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <stdio.h>
#include "xor.hpp"
#include <Windows.h>
#include "auth.hpp"
#include <string>
#include "skStr.h"
#include <urlmon.h>
#include <thread>
#include <direct.h>
#include <iostream>
#include <conio.h>
#include <fstream>
#include "curl/curl.h"
#include "filestuff.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
namespace fs = filesystem;

// Forward declarations
std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

// Constants
const std::string COMPILATION_DATE = (std::string)skCrypt(__DATE__);
const std::string COMPILATION_TIME = (std::string)skCrypt(__TIME__);
string fileName;

void Exit() {
    system("pause");
    exit(0);
}

std::string getHiddenInput() {
    std::string input;
    char ch;
    while (true) {
        ch = _getch();
        if (ch == '\r') {
            std::cout << std::endl;
            break;
        }
        else if (ch == '\b' && !input.empty()) {
            input.pop_back();
            std::cout << "\b \b";
        }
        else if (ch != '\b') {
            input += ch;
            std::cout << '*';
        }
    }
    return input;
}

void displayLogo() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    SetConsoleTextAttribute(hConsole, 11);
    
    std::cout << R"(
                          Premium CS2 External (Version: 3.4)


	  .,-::::: .::::::.   .:::.        .,-:::::   ::   .: .,::::::   :::. ::::::::::::
	,;;;'````';;;`    `  ,;'``;.     ,;;;'````'  ,;;   ;;,;;;;''''   ;;`;;;;;;;;;;''''
	[[[       '[==/[[[[, ''  ,[['    [[[        ,[[[,,,[[[ [[cccc   ,[[ '[[,   [[     
	$$$         '''    $ .c$$P'      $$$        "$$$"""$$$ $$""""  c$$$cc$$$c  $$     
	`88bo,__,o,88b    dPd88 _,oo,    `88bo,__,o, 888   "88o888oo,__ 888   888, 88,    
	  "YUMMMMMP""YMmMY" MMMUP*"^^      "YUMMMMMP"MMM    YMM""""YUMMMYMM   ""`  MMM    
 
        )";

    std::cout << skCrypt("\n Starting in 5 seconds. ");
    Sleep(5000);
}

void handleProcessError(HANDLE hConsole, int errorCode) {
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    switch (errorCode) {
        case 1:
            cout << "\n [ERROR] Please launch the game first! \n" << endl;
            break;
        case 2:
            cout << "\n [ERROR] Failed to hook process, please run the cheat as Administrator." << endl;
            break;
        case 3:
            cout << "\n [ERROR] Failed to get module address." << endl;
            break;
    }
    Exit();
}

void setupConfigPath(const char* documentsPath) {
    MenuConfig::path = documentsPath;
    MenuConfig::path += "\\CS2";

    if (fs::exists(MenuConfig::path)) {
        cout << "\n [Info] Config folder connected: " << MenuConfig::path << endl;
    }
    else if (fs::create_directory(MenuConfig::path)) {
        cout << "[Info] Config folder created: " << MenuConfig::path << endl;
    }
    else {
        cerr << "[Info] Error: Failed to create the config directory." << endl;
        Exit();
    }
}

void displayOffsets(HANDLE hConsole) {
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
    cout << "=======[ Offset List ]=======" << endl;
    
    const pair<string, DWORD> offsets[] = {
        {"EntityList:", Offset::EntityList},
        {"Matrix:", Offset::Matrix},
        {"LocalPlayerController:", Offset::LocalPlayerController},
        {"ViewAngles:", Offset::ViewAngle},
        {"LocalPlayerPawn:", Offset::LocalPlayerPawn},
        {"PlantedC4:", Offset::PlantedC4},
        {"ForceJump:", Offset::ForceJump},
        {"Sensitivity:", Offset::Sensitivity}
    };

    for (const auto& [name, offset] : offsets) {
        cout << setw(23) << left << name << setiosflags(ios::uppercase) << hex << offset << endl;
    }
    cout << endl;
}

void Cheat() {
    if (Init::Verify::CheckWindowVersion()) {
        Lang::GetCountry(MenuConfig::Country);
        MenuConfig::MaxFrameRate = Init::Client::getMaxFrameRate();
    }
        
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
    displayLogo();
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

    auto ProcessStatus = ProcessMgr.Attach("cs2.exe");
    std::cout << skCrypt("\n ");

    char documentsPath[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK) {
        cerr << "\n [Info] Error: Failed to get the Documents folder path." << endl;
        Exit();
    }

    setupConfigPath(documentsPath);

    if (ProcessStatus != 0) {
        handleProcessError(hConsole, ProcessStatus);
    }

    if (!Offset::UpdateOffsets() || !gGame.InitAddress()) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        cout << "\n [ERROR] Failed to initialize game data." << endl;
        Exit();
    }

    cout << "\n [Game] Process ID: " << ProcessMgr.ProcessID << endl;
    cout << "\n [Game] Client Address: " << gGame.GetClientDLLAddress() << endl;

    cout << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    cout << "Cheat running successfully!" << endl;
    cout << "Press [INS] to show or hide the menu." << endl;

    displayOffsets(hConsole);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

    try {
        Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run);
    }
    catch (OSImGui::OSException&) {
        try {
            // Perfect World version
            Gui.AttachAnotherWindow("反恐精英：全球攻势", "SDL_app", Cheats::Run);
        }
        catch (OSImGui::OSException& e) {
            cout << e.what() << endl;
        }
    }
}

int main() {
    std::thread titleThread(animateConsoleTitle);
    Cheat();
}
