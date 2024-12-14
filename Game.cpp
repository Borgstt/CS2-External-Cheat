#include "Game.h"

bool CGame::InitAddress() {
    // Initialize client and server DLL addresses
    Address.ClientDLL = reinterpret_cast<DWORD64>(ProcessMgr.GetProcessModuleHandle("client.dll"));
    Address.ServerDLL = reinterpret_cast<DWORD64>(ProcessMgr.GetProcessModuleHandle("server.dll"));

    if (!Address.ClientDLL) {
        return false;
    }

    // Initialize all client DLL based addresses
    const auto clientBase = GetClientDLLAddress();
    Address.EntityList = clientBase + Offset::EntityList;
    Address.Matrix = clientBase + Offset::Matrix; 
    Address.ViewAngle = clientBase + Offset::ViewAngle;
    Address.LocalController = clientBase + Offset::LocalPlayerController;
    Address.LocalPawn = clientBase + Offset::LocalPlayerPawn;
    Address.ForceJump = clientBase + Offset::ForceJump;
    Address.ForceCrouch = clientBase + Offset::ForceCrouch;
    Address.ForceForward = clientBase + Offset::ForceForward;
    Address.ForceLeft = clientBase + Offset::ForceLeft;
    Address.ForceRight = clientBase + Offset::ForceRight;
    Address.GlobalVars = clientBase + Offset::GlobalVars;

    // Initialize server DLL based addresses
    Address.ServerPawn = GetServerDLLAddress() + Offset::LocalPlayerPawn;

    return true;
}

// Getter methods for addresses
DWORD64 CGame::GetClientDLLAddress() { return Address.ClientDLL; }
DWORD64 CGame::GetServerDLLAddress() { return Address.ServerDLL; }
DWORD64 CGame::GetEntityListAddress() { return Address.EntityList; }
DWORD64 CGame::GetMatrixAddress() { return Address.Matrix; }
DWORD64 CGame::GetViewAngleAddress() { return Address.ViewAngle; }
DWORD64 CGame::GetEntityListEntry() { return Address.EntityListEntry; }
DWORD64 CGame::GetLocalControllerAddress() { return Address.LocalController; }
DWORD64 CGame::GetLocalPawnAddress() { return Address.LocalPawn; }
DWORD64 CGame::GetServerPawnAddress() { return Address.ServerPawn; }
DWORD64 CGame::GetGlobalVarsAddress() { return Address.GlobalVars; }

bool CGame::UpdateEntityListEntry() {
    DWORD64 entityListEntry = 0;
    
    if (!ProcessMgr.ReadMemory<DWORD64>(GetEntityListAddress(), entityListEntry) ||
        !ProcessMgr.ReadMemory<DWORD64>(entityListEntry + 0x10, entityListEntry)) {
        return false;
    }

    Address.EntityListEntry = entityListEntry;
    return entityListEntry != 0;
}

bool CGame::SetViewAngle(float Yaw, float Pitch) {
    return ProcessMgr.WriteMemory<Vec2>(Address.ViewAngle, {Pitch, Yaw});
}

template<typename T>
bool ReadWriteMemory(DWORD64 address, T& value, bool isWrite) {
    return isWrite ? 
        ProcessMgr.WriteMemory<T>(address, value) :
        ProcessMgr.ReadMemory<T>(address, value);
}

bool CGame::SetForceJump(int value) {
    return ReadWriteMemory(Address.ForceJump, value, true);
}

bool CGame::GetForceJump(int& value) {
    return ReadWriteMemory(Address.ForceJump, value, false);
}

bool CGame::SetForceCrouch(int value) {
    return ReadWriteMemory(Address.ForceCrouch, value, true);
}

bool CGame::GetForceCrouch(int& value) {
    return ReadWriteMemory(Address.ForceCrouch, value, false);
}

bool CGame::SetForceMove(int MovingType, int Value) {
    DWORD64 targetAddress;
    
    switch (MovingType) {
        case 0: targetAddress = Address.ForceForward; break;
        case 1: targetAddress = Address.ForceLeft; break;
        case 2: targetAddress = Address.ForceRight; break;
        default: return false;
    }
    
    return ReadWriteMemory(targetAddress, Value, true);
}

bool CGame::GetForceMove(int MovingType, int& Value) {
    DWORD64 targetAddress;
    
    switch (MovingType) {
        case 0: targetAddress = Address.ForceForward; break;
        case 1: targetAddress = Address.ForceLeft; break;
        case 2: targetAddress = Address.ForceRight; break;
        default: return false;
    }
    
    return ReadWriteMemory(targetAddress, Value, false);
}
