#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <string>
#include <DirectXMath.h>
#include <algorithm>

#include "pe32.h"
#include "Utils.h"
#include "../SDK/Interfaces.h"
#include "../SDK/Globals.h"

void* Utils::CreateInterface(const char* szModule, const char* szInterface) {
	tCreateInterface createInterface = (tCreateInterface)GetProcAddress(GetModuleHandle(szModule), "CreateInterface");
	int returnCode = 0;
	return createInterface(szInterface, &returnCode);
}

void* Utils::PatternScan(const char* szModule, const char* szPattern, int iOffset) {
    const char* pattern = szPattern;
    DWORD firstMatch = 0;
    DWORD rangeStart = (DWORD)GetModuleHandleA(szModule);
    MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
    DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
    for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
    {
        if (!*pattern)
            return (void*)(firstMatch + iOffset);

        if (*(PBYTE)pattern == '\?' || *(BYTE*)pCur == getByte(pattern))
        {
            if (!firstMatch)
                firstMatch = pCur;

            if (!pattern[2])
                return (void*)(firstMatch + iOffset);

            if (*(PWORD)pattern == '\?\?' || *(PBYTE)pattern != '\?')
                pattern += 3;

            else
                pattern += 2;    //one ?
        }
        else
        {
            pattern = szPattern;
            firstMatch = 0;
        }
    }
    return nullptr;
}

HANDLE Utils::CreateSimpleThread(void* func, void* params, size_t stackSize) {
    using CreateSimpleThread_t = HANDLE(__cdecl*)(void*, void*, size_t);
    static auto createSimpleThread = (CreateSimpleThread_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "CreateSimpleThread");
    return createSimpleThread(func, params, stackSize);
}

bool Utils::ThreadJoin(HANDLE thread, DWORD ms) {
    using ThreadJoin_t = bool(__cdecl*)(HANDLE, DWORD);
    static auto threadJoin = (ThreadJoin_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "ThreadJoin");
    return threadJoin(thread, ms);
}

std::string Utils::ToUTF8(const std::wstring_view str) {
    if (str.empty())
        return {};

    const auto len = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), 0, 0, 0, 0);

    std::string ret{};

    ret.resize(len);

    WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), ret.data(), len, 0, 0);

    return ret;
}

struct hud_weapons_t {
    std::int32_t* get_weapon_count() {
        return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
    }
};

template <class C>
C* Utils::FindHudElement(const char* szName)
{
    static auto fn = *reinterpret_cast<DWORD**>(Utils::PatternScan(("client.dll"), ("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"), 0x1));
    static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Utils::PatternScan(("client.dll"), ("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
    return (C*)find_hud_element(fn, szName);
}

void Utils::ForceFullUpdate()
{
    const auto fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(Utils::PatternScan(("client.dll"), ("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B")));
    const auto element = FindHudElement<std::uintptr_t*>(("CCSGO_HudWeaponSelection"));
    if (!element || !fn) { return; }

    const auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
    if (!hud_weapons || !*hud_weapons->get_weapon_count()) { return; }

    for (auto i = 0; i < *hud_weapons->get_weapon_count(); i++) {
        i = fn(hud_weapons, i);
    }
}

void Utils::Print(const char* szMessage, ...) {
    static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg");
    char buffer[989];
    va_list list;
    va_start(list, szMessage);
    _vsnprintf_s(buffer, 989, szMessage, list);
    va_end(list);
    fn(buffer, list);
}

void Utils::RandomSeed(int seed) {
    typedef void(*RandomSeed_t)(int);
    static RandomSeed_t m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed");
    return m_RandomSeed(seed);
}

int Utils::RandomInt(int min, int max) {
    static auto randomInt = (int(*)(int, int))GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomInt");
    return randomInt(min, max);
}

float Utils::RandomFloat(float min, float max) {
    static auto randomFloat = (float(*)(float, float))GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat");
    return randomFloat(min, max);
}

Vector2 Utils::GetMousePos() {
    POINT p;
    GetCursorPos(&p);
    return Vector2((int)p.x, (int)p.y);
}

bool Utils::InBounds(Vector2 s, Vector2 e) {
    Vector2 mp = GetMousePos();
    return(mp.x > s.x && mp.x < e.x&& mp.y > s.y && mp.y < e.y);
}

void Utils::WriteUserCmd(void* buf, CUserCmd* in, CUserCmd* out) {
    static auto Fn = PatternScan("client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9");

    __asm
    {
        mov     ecx, buf
        mov     edx, in
        push    out
        call    Fn
        add     esp, 4
    }
}

void Utils::FixMovement(CUserCmd* cmd, QAngle ang) {
    float yaw = ang.yaw;

    float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
    float newYaw = cmd->viewangles.yaw + (cmd->viewangles.yaw < 0.0f ? 360.0f : 0.0f);
    float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
    yawDelta = 360.0f - yawDelta;

    const float forwardmove = cmd->forwardmove;
    const float sidemove = cmd->sidemove;
    cmd->forwardmove = std::cos(DEG2RAD(yawDelta)) * forwardmove + std::cos(DEG2RAD(yawDelta + 90.0f)) * sidemove;
    cmd->sidemove = std::sin(DEG2RAD(yawDelta)) * forwardmove + std::sin(DEG2RAD(yawDelta + 90.0f)) * sidemove;

    cmd->upmove = sin(DEG2RAD(cmd->viewangles.roll)) * sin(DEG2RAD(cmd->viewangles.pitch)) * cmd->sidemove;
    cmd->forwardmove = cos(DEG2RAD(cmd->viewangles.roll)) * cmd->forwardmove + sin(DEG2RAD(cmd->viewangles.pitch)) * sin(DEG2RAD(cmd->viewangles.roll)) * cmd->sidemove;
}

QAngle Utils::VectorToAngle(Vector src, Vector dst) {
    QAngle vAngle;
    Vector delta = src - dst;
    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

    vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
    vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
    vAngle.roll = 0.0f;

    if (delta.x >= 0.0)
        vAngle.yaw += 180.0f;

    vAngle.Normalize();

    return vAngle;
}

float Utils::GetFOV(QAngle& viewAngle, QAngle& aimAngle)
{
    Vector ang, aim;

    AngleVectors(viewAngle, aim);
    AngleVectors(aimAngle, ang);

    auto res = RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
    if (std::isnan(res))
        res = 0.f;
    return res;
}

inline Vector CrossProduct2(const Vector& a, const Vector& b)
{
    return Vector({ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x });
}

void Utils::AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up)
{
    float sr, sp, sy, cr, cp, cy;

    DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.pitch));
    DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.yaw));
    DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles.roll));

    forward.x = (cp * cy);
    forward.y = (cp * sy);
    forward.z = (-sp);
    right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
    right.y = (-1 * sr * sp * sy + -1 * cr * cy);
    right.z = (-1 * sr * cp);
    up.x = (cr * sp * cy + -sr * -sy);
    up.y = (cr * sp * sy + -sr * cy);
    up.z = (cr * cp);
}
void Utils::AngleVectors(const QAngle& angles, Vector& forward)
{
    float	sp, sy, cp, cy;

    DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.pitch));
    DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.yaw));

    forward.x = cp * cy;
    forward.y = cp * sy;
    forward.z = -sp;
}

void Utils::VectorTransform(const Vector& vec1, const matrix3x4_t& mat, Vector& out) {
    out.x = vec1.Dot(Vector(mat[0][0],  mat[0][1],  mat[0][2])) + mat[0][3];
    out.y = vec1.Dot(Vector(mat[1][0],  mat[1][1],  mat[1][2])) + mat[1][3];
    out.z = vec1.Dot(Vector(mat[2][0],  mat[2][1],  mat[2][2])) + mat[2][3];
}

float Utils::GetServerTime(CUserCmd* cmd) {
    static int iTick = 0;

    if (cmd != nullptr)
    {
        static CUserCmd* pLastCmd = nullptr;

        // if command was not predicted - increment tickbase
        if (pLastCmd == nullptr || pLastCmd->hasbeenpredicted)
            iTick = Cheat.LocalPlayer->m_nTickBase();
        else
            iTick++;

        pLastCmd = cmd;
    }

    return TICKS_TO_TIME(iTick);
}

void Utils::MatrixMove(matrix3x4_t* matrix, int size, const Vector& from, const Vector& to) {
    for (int i = 0; i < size; i++) {
        matrix[i][0][3] = matrix[i][0][3] - from.x + to.x;
        matrix[i][1][3] = matrix[i][1][3] - from.y + to.y;
        matrix[i][2][3] = matrix[i][2][3] - from.z + to.z;
    }
}

void Utils::MatrixMove(const matrix3x4_t* matrix, matrix3x4_t* result, int size, const Vector& from, const Vector& to) {
    for (int i = 0; i < size; i++) {
        result[i][0][3] = matrix[i][0][3] - from.x + to.x;
        result[i][1][3] = matrix[i][1][3] - from.y + to.y;
        result[i][2][3] = matrix[i][2][3] - from.z + to.z;
    }
}

void* Utils::GetModuleBaseHandle(const std::string_view szModuleName)
{
    const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30)); // mov eax, fs:[0x30]
    //const _TEB32* pTEB = reinterpret_cast<_TEB32*>(__readfsdword(0x18)); // mov eax, fs:[0x18]
    //const _PEB32* pPEB = pTEB->ProcessEnvironmentBlock;

    if (szModuleName.empty())
        return pPEB->ImageBaseAddress;

    const std::wstring wszModuleName(szModuleName.begin(), szModuleName.end());

    for (LIST_ENTRY* pListEntry = pPEB->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
    {
        const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        if (pEntry->BaseDllName.Buffer && wszModuleName.compare(pEntry->BaseDllName.Buffer) == 0)
            return pEntry->DllBase;
    }

    return nullptr;
}

void* Utils::GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName)
{
    const std::uint8_t* pAddress = static_cast<const std::uint8_t*>(pModuleBase);
    const IMAGE_DOS_HEADER* pDosHeader = static_cast<const IMAGE_DOS_HEADER*>(pModuleBase);
    const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(pAddress + pDosHeader->e_lfanew);
    const IMAGE_OPTIONAL_HEADER* pOptionalHeader = &pNtHeaders->OptionalHeader;

    const std::uintptr_t uExportSize = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    const std::uintptr_t uExportAddress = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    if (uExportSize <= 0U)
        return nullptr;

    const IMAGE_EXPORT_DIRECTORY* pExportDirectory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(pAddress + uExportAddress);
    const std::uintptr_t* pNamesRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfNames);
    const std::uintptr_t* pFunctionsRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfFunctions);
    const std::uint16_t* pNameOrdinals = reinterpret_cast<const std::uint16_t*>(pAddress + pExportDirectory->AddressOfNameOrdinals);

    // perform binary search
    std::uintptr_t uRight = pExportDirectory->NumberOfNames;
    std::uintptr_t uLeft = 0;

    while (uRight != uLeft)
    {
        const std::uintptr_t uMiddle = uLeft + ((uRight - uLeft) >> 1U);
        const int iResult = szProcedureName.compare(reinterpret_cast<const char*>(pAddress + pNamesRVA[uMiddle]));

        if (iResult == 0)
            return const_cast<void*>(static_cast<const void*>(pAddress + pFunctionsRVA[pNameOrdinals[uMiddle]]));

        if (iResult > 0)
            uLeft = uMiddle;
        else
            uRight = uMiddle;
    }

    return nullptr;
}

Vector2* Utils::BoxIntersection(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2) {
    Vector2 box[2] = {
        Vector2(max(start1.x, start2.x), max(start1.y, start2.y)),
        Vector2(min(end1.x, end2.x), min(end1.y, end2.y))
    };

    return box;
}

void Utils::HandleInput(std::string& str, int key) {
    if (key == VK_BACK) {
        if (str.size() == 0)
            return;
        str.erase(str.size() - 1);
        return;
    }

    BYTE keyStates[256];
    GetKeyboardState(keyStates);

    char input[2];
    int res = ToAscii(key, 0, keyStates, (WORD*)input, 0);

    if (res == 1) {
        str.insert(str.end(), input[0]);
    }
}

void Utils::SetClantag(const char* tag)
{
    using Fn = int(__fastcall*)(const char*, const char*);
    static auto fn = reinterpret_cast<Fn>(Utils::PatternScan(("engine.dll"), ("53 56 57 8B DA 8B F9 FF 15")));

    fn(tag, tag);
};