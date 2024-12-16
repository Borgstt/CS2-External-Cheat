#pragma once
#include <optional>
#include "Utils/ProcessManager.hpp"
#include "Offsets.h"
#include "View.hpp"

class CGame {
private:
    // Structure to hold memory addresses for game features
    struct GameAddresses {
        // Movement control addresses
        DWORD64 ForceJump;
        DWORD64 ForceCrouch; 
        DWORD64 ForceForward;
        DWORD64 ForceLeft;
        DWORD64 ForceRight;

        // Core DLL addresses
        DWORD64 ServerDLL;
        DWORD64 ClientDLL;

        // Game state addresses
        DWORD64 EntityList;
        DWORD64 Matrix;
        DWORD64 ViewAngle;
        DWORD64 EntityListEntry;
        DWORD64 LocalController;
        DWORD64 LocalPawn;
        DWORD64 ServerPawn;
        DWORD64 GlobalVars;
    } Address;

public:
    CView View;

    // Initialize game addresses
    bool InitAddress();

    // DLL address getters
    DWORD64 GetClientDLLAddress();
    DWORD64 GetServerDLLAddress();

    // Game state address getters 
    DWORD64 GetEntityListAddress();
    DWORD64 GetMatrixAddress();
    DWORD64 GetViewAngleAddress();
    DWORD64 GetEntityListEntry();
    DWORD64 GetLocalControllerAddress();
    DWORD64 GetLocalPawnAddress();
    DWORD64 GetServerPawnAddress();
    DWORD64 GetGlobalVarsAddress();

    // Entity list management
    bool UpdateEntityListEntry();

    // View control
    bool SetViewAngle(float Yaw, float Pitch);

    // Movement control methods
    bool SetForceJump(int Value);
    bool GetForceJump(int& Value);
    bool SetForceCrouch(int Value);
    bool GetForceCrouch(int& Value);
    bool SetForceMove(int MovingType, int Value);
    bool GetForceMove(int MovingType, int& Value);
};

// Global game instance
inline CGame gGame;
