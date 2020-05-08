#include "rec0de.h"

DWORD dwClient, dwEngine;
HANDLE  hConsole;
RECT m_Rect;
CSimpleIniA ini;
ProcMem mem;
const char* steamid = "X";


bool Initialize() {

	SetConsoleTitle("Matrix");
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetWindowRect(FindWindow(NULL, "Counter-Strike: Global Offensive"), &m_Rect);
	SetConsoleTextAttribute(hConsole, 2);



	cout << "[+] Waiting for csgo.exe ...\n\n";

	sleep(137);

	while (!FindProcessId("csgo.exe"))
		sleep(23337);


	if (!Auth(steamid, "X")) {  //steamID
		cout << "[-] Not authorized!\n\n"; sleep(5000); return false;
	}

	sleep(1000);

	mem.Process("csgo.exe");
	dwClient = mem.Module("client.dll"); dwEngine = mem.Module("engine.dll");
	//game modules loaded

	LoadSettings("settings.ini");

	//ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	return true;
}


/* Credits to UnKnoWnCheaTs Community */
int main()
{
	if (!Initialize())
		return -1;


	while (!GetAsyncKeyState(VK_END)) {


		while (GetAsyncKeyState(VK_MENU) && cvar.triggerEnabled)
			Trigger();



		while (GetAsyncKeyState(VK_MBUTTON) && cvar.aimEnabled)
			Aimbot();


		if (!GetAsyncKeyState(VK_MBUTTON))
			pLocal.bShoot = false;


		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && cvar.bhopEnabled)
			Bhop();


		if (GetAsyncKeyState(VK_F11))
			SetCvar();

		if (cvar.matrix)
			Matrix();

		sleep(31);


	}

	//TerminateProcess(mem.hProcess, 0);
	//CloseHandle(mem.hProcess);

	return 0;
}
void Bhop() {
	if (pLocal.iFlags & (0x1 == 1)) {
		keybd_event(MapVirtualKey(VK_SPACE, 0), 0x39, 0, 0);
		Sleep(5);
		keybd_event(MapVirtualKey(VK_SPACE, 0), 0x39, KEYEVENTF_KEYUP, 0);

	}
}

void Trigger() {

	pLocal.update();

	for (int i = 0; i < 32; i++)
		pEntity[i].update(i);

	if (!(pLocal.totalPlayers - 1))
		return;

	if (pLocal.iCrosshair > 0 && pLocal.iCrosshair < 32 && pLocal.iTeam != pEntity[pLocal.iCrosshair - 1].iTeam)
		shoot; sleep(1);

	sleep(5)
}


void Aimbot() {

	if (!cvar.aimlock && pLocal.bShoot)
		return;

	pLocal.update();

	for (int i = 0; i < 32; i++)
		pEntity[i].update(i);


	int target = GetClosestPlayer(); //ByDistance()
	if (target == -1)
		return;


	vec3 localPos = pLocal.eyePos;
	vec3 targetPos = pEntity[target].vecBone;


	vec3 aimAngle = vec3();
	if (cvar.fixPunch) {
		aimAngle = PunchAng(localPos, targetPos); //fix punch //untrusted
	}
	else {
		aimAngle = nuggah::CalcAngle(targetPos - localPos);
		//aimAngle = CalcAngle(localPos, targetPos);
	}




	//SmoothAim(aimAngle, 10.71337f); //smooth 1
	//smooth2

	//chocolatepc's smooth
	if (cvar.smooth != 0) {
		//SmoothAim(aimAngle, cvar.smooth);
		//vec3 qDelta(aimAngle.x - curView.x, aimAngle.y - curView.y, 0.0f);

		vec3 qDelta(aimAngle - pLocal.viewAngles);


		nuggah::ClampAngles(qDelta);


		aimAngle.x = pLocal.viewAngles.x + qDelta.x / cvar.smooth;
		aimAngle.y = pLocal.viewAngles.y + qDelta.y / cvar.smooth;


		//vec3 curView = pLocal.viewAngles;
		//vec3 smoothedAngle = aimAngle - curView;
		//ClampAngles(smoothedAngle);
		//NormalizeAngles(smoothedAngle);

		//smoothedAngle.x = curView.x + smoothedAngle.x / 400 * cvar.smooth;
		//smoothedAngle.y = curView.y + smoothedAngle.y / 400 * cvar.smooth;
		//smoothedAngle.z = 0;

		//aimAngle = smoothedAngle * (1 / cvar.smooth) + curView;

	}


	nuggah::ClampAngles(aimAngle);


	if (aimAngle == vec3(0, 0, 0)) //dont aim to origin
		return;

	SetAngle(aimAngle);

	sleep(5);

	shoot; pLocal.bShoot = true;

}




int GetClosestPlayer() {
	int closestEntity = -1;

	float bestFov = cvar.fov;
	float bestRealDistance = cvar.fov * 5.0f;
	float bestDistance = cvar.maxDistance;

	//system("cls");
	for (int i = 0; i < 32; i++)
	{
		if (!pEntity[i].isValid())
			continue;


		vec3 localPos = pLocal.eyePos; vec3 targetPos = pEntity[i].vecBone; vec3 viewAngles = pLocal.viewAngles;


		vec3 aimAng = nuggah::CalcAngle(targetPos - localPos);
		nuggah::ClampAngles(aimAng);

		float distance = Dist3D(localPos, targetPos);
		float fov = GetFov2(viewAngles, localPos, targetPos);	//float fov = GetFov(viewAngles, CalcAngle(pVecTarget, eVecTarget));
		float real_distance = GetRealDistanceFOV(distance, aimAng);


		if (distance > bestDistance)
			continue;

		if (fov > bestFov)
			continue;

		if (real_distance > bestRealDistance)
			continue;


		closestEntity = i;
		bestFov = fov;
		bestRealDistance = real_distance;
		bestDistance = distance;
	}

	return closestEntity;
}


/*

int GetClosestPlayer() {
	int closestEntity = -1;

	float bestFov = cvar.fov;
	float bestRealDistance = cvar.fov * 5.0f;
	float bestDistance = cvar.maxDistance;


	system("cls");

	for (int i = 1; i < 32; ++i)
	{
		//if (!pEntity[i].isValid())
			//continue;
		if (!pEntity[i].pBaseEntity)
			continue;

		if (pEntity[i].iTeam == pLocal.iTeam)
			continue;


		if (!pEntity[i].bAlive)
			continue;


		if (pEntity[i].bDormant)
			continue;



		vec3 pVecTarget = pLocal.eyePos;
		vec3 eVecTarget = pEntity[i].vecBone;

		vec3 viewAngles = pLocal.viewAngles;
		vec3 vecView = pLocal.vecViewOffset;


		//float distance = sqrtf(powf(eVecTarget.x - pVecTarget.x, 2.f) + powf(eVecTarget.y - pVecTarget.y, 2.f) + powf(eVecTarget.z - pVecTarget.z, 2.f));
		float distance = Dist3D(pLocal.fPos, pEntity[i].fPos);
		float fov = GetFov(viewAngles, CalcAngle(pVecTarget, eVecTarget), distance);	//float fov = GetFov(viewAngles, CalcAngle(pVecTarget, eVecTarget));

		float real_distance = GetRealDistanceFOV(distance, CalcAngle(pVecTarget, eVecTarget));

		cout << fov << "\n";

		if (distance > bestDistance)
			continue;

		//cout << distance << " < " << bestDistance << "\n";

		if (fov > bestFov)
			continue;

		//cout << fov << " < " << bestFov << "\n";

		//if (real_distance > bestRealDistance)
			//continue;

		closestEntity = i;
		bestFov = fov;
		//bestRealDistance = real_distance;
		bestDistance = distance;


	}

	return closestEntity;
}
*/






void localPlayer::update() {
	totalPlayers = 0;

	if (GetAsyncKeyState(VK_END))
		return;

	dwEnginePtr = mem.Read<DWORD>(dwEngine + offset.dwClientState);
	inGame = mem.Read<bool>(dwEnginePtr + offset.bInGame);

	if (!inGame) { sleep(4000); return; }

	pLocalPlayer = mem.Read<DWORD>(dwClient + offset.dwLocalPlayer);

	bAlive = !(mem.Read<bool>(pLocalPlayer + offset.blifeState));

	iTeam = mem.Read<int>(pLocalPlayer + offset.iTeam);
	iCrosshair = mem.Read<int>(pLocalPlayer + offset.iCrosshairID);
	iFlags = mem.Read<int>(pLocalPlayer + offset.fFlags);

	vecPunch[0] = mem.Read<float>(pLocalPlayer + 0x3010);
	vecPunch[1] = mem.Read<float>(pLocalPlayer + 0x3014);


	fPos = mem.Read<vec3>(pLocalPlayer + 0x134);
	vecViewOffset = mem.Read<vec3>(pLocalPlayer + 0x104);

	viewAngles = mem.Read<vec3>(dwEnginePtr + 0x4D0C);

	eyePos = (fPos + vecViewOffset);

}

void __fastcall entityList::update(int player) {
	if (GetAsyncKeyState(VK_END))
		return;

	pBaseEntity = mem.Read<DWORD>(dwClient + offset.dwEntityList + (player * 0x10));

	if (!pBaseEntity) //null ptr, invalid player
		return;

	iHealth = mem.Read<int>(pBaseEntity + offset.iHealth);

	pLocal.totalPlayers++;

	bAlive = !(mem.Read<bool>(pBaseEntity + offset.blifeState));
	iTeam = mem.Read<int>(pBaseEntity + offset.iTeam);


	iHealth = mem.Read<int>(pBaseEntity + offset.iHealth);
	fFlags = mem.Read<int>(pBaseEntity + offset.fFlags);
	bSpotted = mem.Read<bool>(pBaseEntity + offset.bSpotted);
	bDormant = mem.Read<bool>(pBaseEntity + offset.bDormant);


	BoneMatrix = mem.Read<int>(pBaseEntity + offset.dwBoneMatrix);
	fPos = mem.Read<vec3>(pBaseEntity + 0x134);

	vecBone.x = mem.Read<float>(BoneMatrix + 0x30 * cvar.bone + 0x0C); //8 is head
	vecBone.y = mem.Read<float>(BoneMatrix + 0x30 * cvar.bone + 0x1C); //8 is head
	vecBone.z = mem.Read<float>(BoneMatrix + 0x30 * cvar.bone + 0x2C); //8 is head


	//radar
	static const bool on = true;
	if (cvar.radarEnabled)
		if (bAlive)
			if (iTeam != pLocal.iTeam)
				if (!bSpotted)
					WriteProcessMemory(mem.hProcess, (DWORD*)(pBaseEntity + offset.bSpotted), &on, sizeof(on), NULL);

}

void AngleVectors(const vec3 &angles, vec3& forward)
{


	float sp, sy, cp, cy;


	sy = sin(angles.x*M_PI / 180.f); cy = cos(angles.x*M_PI / 180.f);
	sp = sin(angles.y*M_PI / 180.f); cp = cos(angles.y*M_PI / 180.f);


	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}



vec3 PunchAng(vec3 src, vec3 dst)
{

	float myPunch[2];

	myPunch[0] = pLocal.vecPunch[0];
	myPunch[1] = pLocal.vecPunch[1];

	vec3 delta = src - dst;
	float hyp = sqrtf(pow(delta.x, 2) + pow(delta.y, 2));

	vec3 angles;

	angles.x = (float)(atanf(delta.z / hyp) * 57.295779513082f - myPunch[0] * 2.0f);
	angles.y = (float)(atanf(delta.y / delta.x) * 57.295779513082f - myPunch[1] * 2.0f);
	angles.z = 0.0f;

	if (delta.x >= 0.0)
	{
		angles.y += 180.0f;
	}

	return angles;
}



vec3 CalcAngle(vec3 src, vec3 dst)
{//smurfstomp
	vec3 aimAng;
	vec3 vDelta = src - dst;

	//float fHyp = sqrtf(powf(vDelta.x,2.0) + powf(vDelta.y, 2.0));

	float fHyp = vDelta.LengthXY();

	//aimAng.x = (atanf(vDelta.z / fHyp)) * (180.0f / M_PI);
	aimAng.x = D3DXToDegree(atanf(vDelta.z / fHyp));
	aimAng.y = D3DXToDegree(atanf(vDelta.y / vDelta.x));


	if (vDelta.x >= 0.0f)
		aimAng.y += 180.0f;

	return aimAng;
}





int ByDistance()
{//nether source

	float maxDist = cvar.maxDistance, tmpDist;
	int index = -1;


	for (int i = 1; i < 32; i++)
	{
		//if (!pEntity[i].isValid())
			//continue;

		tmpDist = Dist3D(pLocal.eyePos, pEntity[i].vecBone);


		if (tmpDist < maxDist)
		{
			maxDist = tmpDist;
			index = i;

		}

	}

	return index;
}




float GetFov(vec3 viewAngle, vec3 aimAngle, float dist) {
	//SEGnosis's cod3
	/***************/
	float     fRealDistance,
		fYawDifference;
	//		fPitchDifference




	// Get the distance between your current yaw and the yaw to aim at the enemy
	fYawDifference = sqrt(powf(AngleDistance(aimAngle.y, viewAngle.y), 2.f));

	// Calculate the real distance
	fRealDistance = sin(D3DXToRadian(fYawDifference)) * dist;




	return fRealDistance;

}

float AngleDistance(float next, float cur)
{
	float delta = next - cur;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	return delta;
}



void MakeVector(vec3 angle, vec3& vector)
{
	float pitch = float(angle.x * M_PI / 180);
	float yaw = float(angle.y * M_PI / 180);
	float tmp = float(cos(pitch));
	vector.x = float(-tmp * -cos(yaw));
	vector.y = float(sin(yaw)*tmp);
	vector.z = float(-sin(pitch));
}

float GetFov2(vec3 angle, vec3 src, vec3 dst)
{
	vec3 aim;
	float fov;

	vec3 ang = CalcAngle(src, dst);

	MakeVector(angle, aim);
	MakeVector(ang, ang);
	float mag_s = sqrt((aim.x * aim.x) + (aim.y * aim.y) + (aim.z * aim.z));
	float mag_d = sqrt((aim.x * aim.x) + (aim.y * aim.y) + (aim.z * aim.z));
	float u_dot_v = aim.x * ang.x + aim.y * ang.y + aim.z * ang.z;
	fov = acos(u_dot_v / (mag_s*mag_d)) * (180.0 / M_PI);

	if (fov != fov) // check if fov is NaN(ind)
		return 0.0f;
	else
		return fov;
}



float GetRealDistanceFOV(float distance, vec3 angle)
{
	/*    n
	w + e
	s        'real distance' aimtuX
	|
	a point -> x --..  v
	|     ''-- x <- a guy
	|          /
	|         /
	|       /
	| <------------ both of these lines are the same length
	|    /      /
	|   / <-----'
	| /
	o
	localplayer
	*/

	vec3 aimingAt;
	AngleVectors(pLocal.viewAngles, aimingAt);
	aimingAt = aimingAt*distance;

	vec3 aimAt;
	AngleVectors(angle, aimAt);
	aimAt = aimAt*distance;

	return Dist3D(aimingAt, aimAt);
}




void SmoothAim(vec3 angle, float smoothvalue) //SmoothAim(aimAngle, .1337f);
{//aimtux


	vec3 viewAngles;

	viewAngles = pLocal.viewAngles;

	vec3 delta = angle - viewAngles;

	NormalizeAngles(delta);

	float smooth = powf(smoothvalue, 0.4f); // Makes more slider space for actual useful values

	float minsmooth = min(0.99f, smooth);


	vec3 toChange;


	/*
	if (type == (int)SmoothType::SLOW_END)
		toChange = delta - delta * smooth;
	else if (type == (int)SmoothType::CONSTANT || type == (int)SmoothType::FAST_END)
	{
		float coeff = (1.0f - smooth) / delta.Length() * 4.f;

		if (type == (int)SmoothType::FAST_END)
			coeff = powf(coeff, 2.f) * 10.f;

		coeff = std::min(1.f, coeff);
		toChange = delta * coeff;
	}
	*/

	float coeff = (1.0f - smooth) / delta.Length() * 4.f;

	coeff = powf(coeff, 2.f) * 10.f;

	coeff = min(1.f, coeff);
	toChange = delta * coeff;



	//toChange = delta - delta * minsmooth;

	angle = viewAngles + toChange;
}

void NormalizeAngles(vec3 angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

void ClampAngles(vec3 angle)
{

	if (angle.x > 89.0f)
		angle.x = 89.0f;

	if (angle.x < -89.0f)
		angle.x = -89.0f;

	while (angle.y > 180)
		angle.y -= 360;

	while (angle.y < -180)
		angle.y += 360;

	angle.z = 0;

	//return angle;


	/*
	if (angle.y > 180.0f)
		angle.y = 180.0f;
	else if (angle.y < -180.0f)
		angle.y = -180.0f;

	if (angle.x > 89.0f)
		angle.x = 89.0f;
	else if (angle.x < -89.0f)
		angle.x = -89.0f;

	angle.z = 0;

	*/
}





void SetAngle(vec3 ang) {
	WriteProcessMemory(mem.hProcess, (DWORD*)(pLocal.dwEnginePtr + 0x4D0C), &ang, sizeof(vec3), NULL);
	//mem.Write(pLocal.dwEnginePtr + 0x4D0C, ang);
}




float Dist3D(vec3 src, vec3 dst)
{
	return sqrtf(powf(dst.x - src.x, 2.f) + powf(dst.y - src.y, 2.f) + powf(dst.z - src.z, 2.f));
}


bool WorldToScreen(float * from, float * to)
{
	W2S_M = mem.Read<WorldToScreenMatrix_t>(dwClient + offset.dwViewMatrix);

	to[0] = W2S_M.flMatrix[0][0] * from[0] + W2S_M.flMatrix[0][1] * from[1] + W2S_M.flMatrix[0][2] * from[2] + W2S_M.flMatrix[0][3];
	to[1] = W2S_M.flMatrix[1][0] * from[0] + W2S_M.flMatrix[1][1] * from[1] + W2S_M.flMatrix[1][2] * from[2] + W2S_M.flMatrix[1][3];

	float w = W2S_M.flMatrix[3][0] * from[0] + W2S_M.flMatrix[3][1] * from[1] + W2S_M.flMatrix[3][2] * from[2] + W2S_M.flMatrix[3][3];

	if (w < 0.01f)
		return false;

	float invw = 1.0f / w;
	to[0] *= invw;
	to[1] *= invw;

	int width = (int)(m_Rect.right - m_Rect.left);
	int height = (int)(m_Rect.bottom - m_Rect.top);

	float x = width / 2;
	float y = height / 2;

	x += 0.5 * to[0] * width + 0.5;
	y -= 0.5 * to[1] * height + 0.5;

	to[0] = x + m_Rect.left;
	to[1] = y + m_Rect.top;

	return true;
}


void LoadSettings(const char* file) {


	ini.SetUnicode();
	SI_Error loadfile = ini.LoadFile(file);
	if (loadfile < 0)
	{
		cout << "[-] Error loading settings.ini\n\n";

		//set cvars
		cvar.aimEnabled = true;
		cvar.triggerEnabled = true;
		cvar.radarEnabled = true;
		cvar.bhopEnabled = true;
		cvar.fixPunch = true;
		cvar.matrix = false;
		cvar.fov = 15;
		cvar.maxDistance = 7000;
		cvar.smooth = 0;
		cvar.bone = 8;
		cvar.aimteam = false;
		cvar.aimlock = false;

		cout << "[+] Hardcoded settings loaded!\n\n";

		return;
	}

	CSimpleIniA::TNamesDepend keys;
	ini.GetAllKeys("settings", keys);
	const char * pValue;


	keys.sort(CSimpleIniA::Entry::LoadOrder());
	for (CSimpleIniA::TNamesDepend::const_iterator i = keys.begin(); i != keys.end(); ++i) {
		pValue = ini.GetValue("settings", i->pItem, NULL);

		stringstream buf;
		std::string key;
		buf << i->pItem;
		key = buf.str();

		//printf("'%s' = ", i->pItem);
		//printf("'%s'\n", pszValue);

		if (key == "aim") { cvar.aimEnabled = atoi(pValue); }
		if (key == "trigger") { cvar.triggerEnabled = atoi(pValue); }
		if (key == "radar") { cvar.radarEnabled = atoi(pValue); }
		if (key == "bhop") { cvar.bhopEnabled = atoi(pValue); }

		if (key == "aimteam") { cvar.aimteam = atoi(pValue); }
		if (key == "aimlock") { cvar.aimlock = atoi(pValue); }
		if (key == "fixpunch") { cvar.fixPunch = atoi(pValue); }
		if (key == "bone") { cvar.bone = atoi(pValue); }

		if (key == "maxdistance") { cvar.maxDistance = atof(pValue); }
		if (key == "fov") { cvar.fov = atof(pValue); }
		if (key == "smooth") { cvar.smooth = atof(pValue); }


		if (key == "matrix") { cvar.matrix = atoi(pValue); }

	}

	cout << "[+] Settings loaded!\n\n";

	//ini.SetValue("settings", "fov", std::to_string(1997).c_str());
}



void SetCvar() {
	while (1) {

		cout << "\ncmd > ";
		std::string var, val, cmd;
		std::getline(std::cin, cmd);

		//system("cls");

		std::istringstream iss(cmd);

		iss >> var >> val;

		vector<string> cmdlist{ "?", "q","bye","clear","matrix","aim","aimteam","aimlock","trigger","radar","bhop","fixpunch","maxdistance","fov","bone","smooth" };


		bool found = (std::find(cmdlist.begin(), cmdlist.end(), var) != cmdlist.end());

		if (var.empty())
			return;

		if (found) {
			if (var == "?") { for (std::vector<string>::const_iterator i = cmdlist.begin(); i != cmdlist.end(); ++i) { std::cout << *i << "\n"; } }
			if (var == "clear") { system("cls"); }
			if (var == "q") { break; }
			if (var == "bye") { exit(0); }
			if (var == "matrix") {
				cvar.matrix = !cvar.matrix; ini.SetValue("settings", "matrix", std::to_string(cvar.matrix).c_str());
			}
			///////////////
			///////////////

			SI_Error set;

			if (var == "aim") {
				cvar.aimEnabled = !cvar.aimEnabled; cout << var << " = " << cvar.aimEnabled << "\n\n";
				set = ini.SetValue("settings", "aim", std::to_string(cvar.aimEnabled).c_str());
			}
			if (var == "aimteam") {
				cvar.aimteam = !cvar.aimteam; cout << var << " = " << cvar.aimteam << "\n\n";
				set = ini.SetValue("settings", "aimteam", std::to_string(cvar.aimteam).c_str());
			}
			if (var == "aimlock") {
				cvar.aimlock = !cvar.aimlock; cout << var << " = " << cvar.aimlock << "\n\n";
				set = ini.SetValue("settings", "aimlock", std::to_string(cvar.aimlock).c_str());
			}

			if (var == "trigger") {
				cvar.triggerEnabled = !cvar.triggerEnabled;
				cout << var << " = " << cvar.triggerEnabled << "\n\n";
				set = ini.SetValue("settings", "trigger", std::to_string(cvar.triggerEnabled).c_str());
			}
			if (var == "radar") {
				cvar.radarEnabled = !cvar.radarEnabled;
				cout << var << " = " << cvar.radarEnabled << "\n\n";
				set = ini.SetValue("settings", "radar", std::to_string(cvar.radarEnabled).c_str());
			}
			if (var == "bhop") {
				cvar.bhopEnabled = !cvar.bhopEnabled;
				cout << var << " = " << cvar.bhopEnabled << "\n\n";
				set = ini.SetValue("settings", "bhop", std::to_string(cvar.bhopEnabled).c_str());
			}

			if (var == "fixpunch") {
				cvar.fixPunch = !cvar.fixPunch; cout << var << " = " << cvar.fixPunch << "\n\n";
				set = ini.SetValue("settings", "fixpunch", std::to_string(cvar.fixPunch).c_str());
			}

			if (var == "maxdistance") {
				cvar.maxDistance = atof(val.c_str());
				cout << var << " = " << cvar.maxDistance << "\n\n";
				set = ini.SetValue("settings", "maxdistance", std::to_string(cvar.maxDistance).c_str());
			}
			if (var == "fov") {
				cvar.fov = atof(val.c_str()); cout << var << " = " << cvar.fov << "\n\n";
				set = ini.SetValue("settings", "fov", std::to_string(cvar.fov).c_str());
			}
			if (var == "smooth") {
				cvar.smooth = atof(val.c_str()); cout << var << " = " << cvar.smooth << "\n\n";
				set = ini.SetValue("settings", "smooth", std::to_string(cvar.smooth).c_str());
			}
			if (var == "bone") {
				cvar.bone = atoi(val.c_str()); cout << var << " = " << cvar.bone << "\n\n";
				set = ini.SetValue("settings", "bone", std::to_string(cvar.bone).c_str());
			}


			cout << "\n";
			cout << "aim = " << cvar.aimEnabled << "\n";
			cout << "trigger = " << cvar.triggerEnabled << "\n";
			cout << "radar = " << cvar.radarEnabled << "\n";
			cout << "bhop = " << cvar.bhopEnabled << "\n";
			cout << "fixpunch = " << cvar.fixPunch << "\n";
			cout << "maxdistance = " << cvar.maxDistance << "\n";
			cout << "fov = " << cvar.fov << "\n";
			cout << "smooth = " << cvar.smooth << "\n";
			cout << "bone = " << cvar.bone << "\n";
			cout << "aimteam = " << cvar.aimteam << "\n";
			cout << "aimlock = " << cvar.aimlock << "\n";

			cout << "\n";



			if (set < 0 && var != "?") { cout << "\n[-] Error updating " << var << "\n\n"; }

			SI_Error bSave = ini.SaveFile("settings.ini");
			if (bSave < 0)
				cout << "\n[-] Error saving settings.ini\n\n";

		}
		else {
			cout << "\n[-] Command not found\n\n";
		}


	}
}




//
//  Utilz
//

DWORD FindProcessId(char* processName)
{
	// strip path

	char* p = strrchr(processName, '\\');
	if (p)
		processName = p + 1;

	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!strcmp(processName, processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!strcmp(processName, processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

void Matrix() {

	char caRow[80]; int j = 7; int k = 2; int l = 5; int m = 1;

	while (!GetAsyncKeyState(VK_F11) && !GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_MENU) && !GetAsyncKeyState(VK_END) && !GetAsyncKeyState(VK_MBUTTON)) {


		// Output a random row of characters
		for (int i = 0; i < 80; ++i)
		{
			if (!GetAsyncKeyState(VK_F11) && GetAsyncKeyState(VK_LBUTTON) || GetAsyncKeyState(VK_MENU) || GetAsyncKeyState(VK_END) || GetAsyncKeyState(VK_MBUTTON))
				return;

			if (caRow[i] != ' ') {

				caRow[i] = (33 + ((j + i*i) - ((j + i*i) / 30) * 30));
				//if (((i*i + k) % 71) == 0) {
				if (((i*i + k) % 41) == 0) {
					SetConsoleTextAttribute(hConsole, 7);
				}
				else {
					SetConsoleTextAttribute(hConsole, 2);
				}
			}
			std::cout << caRow[i];
			SetConsoleTextAttribute(hConsole, 2);
		}
		j = (j + 31);
		k = (k + 17);
		l = (l + 47);
		m = (m + 67);
		caRow[(j - ((j / 80) * 80))] = '-';
		caRow[(k - ((k / 80) * 80))] = ' ';
		caRow[(l - ((l / 80) * 80))] = '-';
		caRow[(m - ((m / 80) * 80))] = ' ';
		// Delay
		sleep(31);
	} //end while
}

//extern char **environ;
bool Auth(const char * steamid, const char * profile) {

	CStringArray m_strArray;
	EnvVarValArray arrEnvVarVal;

	//arrEnvVarVal.clear();

	std::wstring windowName;
	windowName = L"Counter-Strike: Global Offensive";

	HWND windowHandle = FindWindowW(NULL, windowName.c_str());
	DWORD* processID = new DWORD;
	GetWindowThreadProcessId(windowHandle, processID);

	// Get the proces ID set forthe item
	DWORD dwProcessID = *processID;


	// Open the process for further operations
	HANDLE hProcess = CProcessEnvReader::OpenProcessToRead(dwProcessID);
	if (hProcess)
	{
		_ENVSTRING_t stEnvData;
		// Read the process environment block
		if (!CProcessEnvReader::ReadEnvironmentBlock(hProcess, stEnvData))
		{
			//MessageBox(NULL, "error reading process env block", "env block error", NULL);
			cout << "[-] Error reading process Env block\n\n";
			//return false;
		}

		// Parse the retrieved data
		CProcessEnvReader::ParseEnvironmentStrings(stEnvData.pData,
			stEnvData.nSize / 2,
			m_strArray);


		// Seperate values and variables
		CProcessEnvReader::SeparateVariablesAndValues(m_strArray, arrEnvVarVal);

		// UpdateProcessMiscInfo( hProcess, pNMItemActivate->iItem);
		CProcessEnvReader::ReleaseHandle(hProcess);

		int nIdx = 0;
		int nListIdx = 0;

		EnvVarValArray::const_iterator Iter = arrEnvVarVal.begin();

		for (int nListIdx = 0; Iter != arrEnvVarVal.end(); ++Iter, ++nListIdx)
		{
			CString env = (*Iter).first;
			CString var = (*Iter).second;
			if ((env == "STEAMID") && (
				(var == "X")  //raimit0
				))
			{ //steamid
			  //MessageBox(NULL, (*Iter).first + "=" + (*Iter).second, "arraylist", NULL);
				return true;
			}

		}

	}
	else {
		//MessageBox(NULL, "erro ao abrir processo", "csgo", NULL);
		cout << "[-] Error reading process Env\n\n";
	}


	return false;
}
