#pragma once
#include <string>

#include "../SDK/Misc/Vector.h"
#include "../SDK/Misc/CUserCmd.h"
#include "../SDK/Misc/Matrix.h"

#include "Math.h"

typedef void* (__cdecl* tCreateInterface)(const char*, int*);
typedef void(__cdecl* MsgFn)(const char*, va_list);
#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / GlobalVars->interval_per_tick))
#define TICKS_TO_TIME(tick) ((tick) * GlobalVars->interval_per_tick) 
#define LOG Utils::Print

namespace Utils {
	void*	CreateInterface(const char* szModule, const char* szInterface);
	void*	PatternScan(const char* szModule, const char* szPattern, int iOffset = 0x0);
	void*	GetModuleBaseHandle(const std::string_view szModuleName);
	void*	GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName);

	HANDLE	CreateSimpleThread(void* func, void* params, size_t stackSize = 0);
	bool	ThreadJoin(HANDLE thread, DWORD ms);

	std::string ToUTF8(const std::wstring_view str);

	void	ForceFullUpdate();

	void	Print(const char* szMessage, ...);
	void	RandomSeed(int seed);
	int		RandomInt(int min, int max);
	float	RandomFloat(float min, float max);

	Vector2 GetMousePos();
	bool	InBounds(Vector2 start, Vector2 end);
	void	WriteUserCmd(void* buf, CUserCmd* in, CUserCmd* out);

	void	FixMovement(CUserCmd* cmd, QAngle yaw);
	void	AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up);
	void	AngleVectors(const QAngle& angles, Vector& forward);
	QAngle	VectorToAngle(Vector src, Vector dst);
	void	VectorTransform(const Vector& vec1, const matrix3x4_t& mat, Vector& out);
	float	GetFOV(QAngle& angle1, QAngle& angle2);
	float	GetServerTime(CUserCmd* cmd);

	void	MatrixMove(matrix3x4_t* matrix, int size, const Vector& from, const Vector& to);
	void	MatrixMove(const matrix3x4_t* base, matrix3x4_t* result, int size, const Vector& from, const Vector& to);

	Vector2* BoxIntersection(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2);

	void	HandleInput(std::string& str, int key);

	void    SetClantag(const char* tag);
	template<class C>
	C* FindHudElement(const char* szName);
}