#pragma once
#pragma warning(disable: 4244)


#include <SDKDDKVer.h>
#include "ProcEnvReader.h"
#include "ProcMem.h"
#include "SimpleIni.h"

#include <stdio.h>
#include <tchar.h>
#include <thread>
#include <conio.h>
#include <cstdint>
#include <shellapi.h>


#define sleep(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#define shoot mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
#define M_PI 3.14159265358979323846f
#define M_PHI 1.618033988749895

#define D3DXToRadian(degree) ((degree) * (M_PI / 180.0f))
#define D3DXToDegree(radian) ((radian) * (180.0f / M_PI))


extern "C" { int _afxForceUSRDLL; }

class vec3
{
public:

	vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	vec3() : x(0), y(0), z(0) {}

	vec3(const vec3 &src)
	{
		*this = src;
	}

	vec3& operator =(const vec3 &src)
	{
		x = src.x;
		y = src.y;
		z = src.z;
		return *this;
	}

	vec3& operator +(const vec3 &src) const
	{
		return vec3(x + src.x, y + src.y, z + src.z);
	}

	vec3& operator -(const vec3 &src) const
	{
		return vec3(x - src.x, y - src.y, z - src.z);
	}

	vec3& operator *(const float &value) const
	{
		return vec3(x * value, y * value, z * value);
	}


	bool operator ==(const vec3 &value) const
	{
		if (x == value.x && y == value.y && z == value.z)
			return true;
		else
			return false;
	}



	float Length() {
		return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	float LengthXY() {
		return sqrtf(powf(x, 2) + powf(y, 2));
	}

	inline void Normalize()
	{
		float l = 1.0f / Length();
		x *= l;
		y *= l;
		z *= l;
	}

	inline float DotProduct(vec3 v)
	{
		return (x * v.x + y * v.y + z * v.z);
	}


	float x;
	float y;
	float z;

};

namespace nuggah
{
	const float M_RADPI = 180.0f / M_PI;

	inline vec3 CalcAngle(vec3 vDelta) // expects vecDiff to be [Destination Vector]-[Source Vector]
	{
		return vec3(std::atan2(-vDelta.z, vDelta.LengthXY())*M_RADPI, std::atan2(vDelta.y, vDelta.x)*M_RADPI, 0.0f);
	}




	inline float NormalizeAngles(float flAng)
	{
		if (!std::isfinite(flAng))
		{
			return 0.0f;
		}

		return std::remainder(flAng, 360.0f);
	}

	inline void ClampAngles(vec3& vecAng)
	{
		vecAng.x = max(-89.f, min(89.f, NormalizeAngles(vecAng.x)));
		vecAng.y = NormalizeAngles(vecAng.y);
		vecAng.z = 0.0f;
	}
};



float AngleDistance(float next, float cur);
void MakeVector(vec3 angle, vec3 & vector);
void AngleVectors(const vec3 &angles, vec3& forward);


vec3 PunchAng(vec3 src, vec3 dst);
vec3 CalcAngle(vec3 src, vec3 dst);

float GetRealDistanceFOV(float distance, vec3 angle);
float Dist3D(vec3 src, vec3 dst);

void ClampAngles(vec3 angle);
void NormalizeAngles(vec3 angle);


bool Initialize();
void Bhop();
void Trigger();
void Aimbot();




int GetClosestPlayer();
int ByDistance();

float GetFov(vec3 viewAngle, vec3 aimAngle, float dist);
float GetFov2(vec3 angle, vec3 src, vec3 dst);

void SmoothAim(vec3 angle, float smoothvalue);
void SetAngle(vec3 ang);




void LoadSettings(const char * file);
void SetCvar();
DWORD FindProcessId(char * processName);
void Matrix();
bool Auth(const char * steamid, const char * profile);




class Cvars {
public:
	int aimEnabled, triggerEnabled, radarEnabled, bhopEnabled, fixPunch, bone, aimteam, aimlock;
	float maxDistance, fov, smooth;

	int matrix;
};
Cvars cvar;






class localPlayer {
public:
	DWORD pLocalPlayer, dwEnginePtr;
	int iTeam, iCrosshair, iMaxPlayers, BoneMatrix, iFlags;

	float vecPunch[2];

	vec3 fPos, vecViewOffset, eyePos, viewAngles, vecOrigin;

	bool inGame, bAlive, bShoot;

	int totalPlayers = 20;
	void update();
};
localPlayer pLocal;


class entityList {
public:
	DWORD pBaseEntity;
	int iTeam, iHealth, fFlags, BoneMatrix;
	bool bDormant, bSpotted, bAlive;


	vec3 fPos, fAngle, vecBone;

	void __fastcall update(int player);

	bool isValid() {
	
		if (iTeam == 0 || iTeam == 1)
			return false;

		if (!bAlive)
			return false;

		if (iTeam == pLocal.iTeam && !cvar.aimteam)
			return false;

		//if (bDormant)
			//return false;

		return true;

		
	}

};
entityList pEntity[32];


class offsets {
public:
	const DWORD dwLocalPlayer = 0xAA66D4;
	const DWORD dwEntityList = 0x4AC91B4;
	const DWORD dwClientState = 0x5CA524;
	const DWORD dwViewMatrix = 0x4ABAD54;
	const DWORD dwInput = 0x4F13CB0;
	const DWORD dwRadarBase = 0x4EFDEBC;
	//
	const DWORD iCrosshairID = 0xAA70;
	const DWORD dwBoneMatrix = 0x2698;
	const DWORD fFlags = 0x100;
	const DWORD blifeState = 0x25B;
	const DWORD bDormant = 0xE9;
	const DWORD iTeam = 0xF0;
	const DWORD iHealth = 0xFC;
	const DWORD bSpotted = 0x939;
	const DWORD vecOrigin = 0x134;
	const DWORD iMaxPlayers = 0x308;
	const DWORD bInGame = 0x100;

};
offsets offset;

struct mapplayer_t {
	vec3  AbsOrigin;
	DWORD   unknown[10]; // to lazy to find out whats in there
	int     Health;
	wchar_t Name[32];
};


typedef struct {
	float flMatrix[4][4];
}WorldToScreenMatrix_t;
WorldToScreenMatrix_t W2S_M;
