/*
Syn's AyyWare Framework 2015
*/

#include "RageBot.h"
#include "RenderManager.h"
#include "Autowall.h"
#include <iostream>

void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

void CRageBot::Move(CUserCmd *pCmd, bool& bSendPacket)
{
	// Master switch
	if (!Menu::Window.RageBotTab.Active.GetState())
		return;

	// Aimbot
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	// Recoil
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	// No Spread
	if (Menu::Window.RageBotTab.AccuracySpread.GetState())
		DoNoSpread(pCmd);

	// Anti-Aim
	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
		DoAntiAim(pCmd, bSendPacket);

	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);

		}
	}

	LastAngle = pCmd->viewangles;
}

bool Ready2Silent(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	float serverTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
	if (pWeapon->GetNextPrimaryAttack() > serverTime)
		return false;

	return true;
}

bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

void CRageBot::NormalizeVector(Vector& vec)
{
	for (int i = 0; i < 3; ++i)
	{
		while (vec[i] > 180.f)
			vec[i] -= 360.f;

		while (vec[i] < -180.f)
			vec[i] += 360.f;
	}

	vec[2] = 0.f;
}

void CRageBot::ClampAngles(Vector& vecAngles)
{
	if (vecAngles[0] > 89.f)
		vecAngles[0] = 89.f;
	if (vecAngles[0] < -89.f)
		vecAngles[0] = -89.f;
	if (vecAngles[1] > 180.f)
		vecAngles[1] = 180.f;
	if (vecAngles[1] < -180.f)
		vecAngles[1] = -180.f;

	vecAngles[2] = 0.f;
}

// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd, bool& bSendPacket)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	bool FindNewTarget = true;
	//IsLocked = false;

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	bool bReady2Silent = Ready2Silent(pLocal, pWeapon);

	if (pWeapon)
	{
		if (!bReady2Silent && bSendPacket == false)
			bSendPacket = true;

		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View; Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					FindNewTarget = false;
			}
		}
	}

	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		// Target selection type
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		}

		// Memesj
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		HitBox = HitScan(pTarget);

		// Key
		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		Vector AimPoint = GetHitboxPosition(pTarget, HitBox);

		if (AimAtPoint(pLocal, AimPoint, pCmd))
		{
			if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
			{
				pCmd->buttons |= IN_ATTACK;
			}

			if (Menu::Window.RageBotTab.AimbotPSilent.GetState())
			{
				Vector point;
				IClientEntity* pLocal = hackManager.pLocal();
				CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
				float qOldViewAnglesY = pCmd->viewangles.y;
				Vector vecOldMovement(pCmd->sidemove, pCmd->forwardmove, 0.f);
				Vector vOldView = pCmd->viewangles;

				float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

				bool bBulletTime = true;
				if (pWeapon->GetNextPrimaryAttack() > flServerTime)
					bBulletTime = false;

				// Do your stuff here
				static int iChokedPackets = 0;
				iChokedPackets++;

				if (iChokedPackets < 14)
				{
					if (IsAbleToShoot(pLocal))
						bSendPacket = false;
					else bSendPacket = true;
				}
				else
				{
					bSendPacket = true;
					iChokedPackets = 0;
				}

				if (bSendPacket)
				{
					pCmd->viewangles.x = 89;
					pCmd->viewangles.y -= 180;
					pCmd->sidemove = vecOldMovement.x;
					pCmd->forwardmove = vecOldMovement.y;
				}
			}

			if (Menu::Window.RageBotTab.AimbotAutoFire.GetState())
				pCmd->buttons |= IN_ATTACK;
		}

		if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
		{
			pCmd->forwardmove = 0.f;
			pCmd->sidemove = 0.f;
		}
		
		if (Menu::Window.RageBotTab.AccuracyAutoCrouch.GetState())
		{
			pCmd->buttons |= IN_DUCK;
		}

	}

	if (Menu::Window.MiscTab.OtherFakeLag.GetState()) {
		int iChoke = 14;
		static int iFakeLag = 0;

		if (iFakeLag < iChoke)
			bSendPacket = false;

		if (iFakeLag > iChoke + 3)
			iFakeLag = 0;

		iFakeLag++;

	}

	// Auto Pistol
	static bool WasFiring = false;
	CSWeaponInfo* WeaponInfo = pWeapon->GetCSWpnData();
	if (!WeaponInfo->m_IsFullAuto && Menu::Window.RageBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}

		WasFiring = pCmd->buttons & IN_ATTACK ? true : false;
	}

	if (!bReady2Silent && !(pCmd->buttons & IN_ATTACK))
		bSendPacket = true;

}

bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	// Target selection
	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

class QAngleByValue;
class QAngle
{
public:
	float x, y, z;

	QAngle(void);
	QAngle(float X, float Y, float Z);

	operator QAngleByValue &() { return *((QAngleByValue *)(this)); }
	operator const QAngleByValue &() const { return *((const QAngleByValue *)(this)); }

	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	void Random(float minVal, float maxVal);

	bool IsValid() const;
	void Invalidate();

	bool IsZero()
	{
		CHECK_VALID(*this);
		if (this->x == 0.f && this->y == 0.f && this->z == 0.f)
			return true;

		return false;
	}

	float operator[](int i) const;
	float& operator[](int i);

	float* Base();
	float const* Base() const;

	bool operator==(const QAngle& v) const;
	bool operator!=(const QAngle& v) const;

	QAngle& operator+=(const QAngle &v);
	QAngle& operator-=(const QAngle &v);
	QAngle& operator*=(float s);
	QAngle& operator/=(float s);

	float   Length() const;
	float   LengthSqr() const;

	QAngle& operator=(const QAngle& src);

	QAngle  operator-(void) const;

	QAngle  operator+(const QAngle& v) const;
	QAngle  operator-(const QAngle& v) const;
	QAngle  operator*(float fl) const;
	QAngle  operator/(float fl) const;

	QAngle Clamp();
	QAngle Mod(float N);

	inline QAngle QAngle::Normalize()
	{
		QAngle vector;
		float length = this->Length();

		if (length != 0) {
			vector.x = x / length;
			vector.y = y / length;
			vector.z = z / length;
		}
		else
			vector.x = vector.y = 0.0f; vector.z = 1.0f;

		return vector;
	}

	QAngle Normalized()
	{
		if (this->x != this->x)
			this->x = 0;
		if (this->y != this->y)
			this->y = 0;
		if (this->z != this->z)
			this->z = 0;

		if (this->x > 89.f)
			this->x = 89.f;
		if (this->x < -89.f)
			this->x = -89.f;

		while (this->y > 180)
			this->y -= 360;
		while (this->y <= -180)
			this->y += 360;

		if (this->y > 180.f)
			this->y = 180.f;
		if (this->y < -180.f)
			this->y = -180.f;

		this->z = 0;

		return *this;
	}
};

int CRageBot::GetTargetHealth()
{
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::HitScan(IClientEntity* pEntity)
{ 
	std::vector<int> HitBoxesToScan;
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();

	// Get the hitboxes to scan
#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	if (HitScanMode == 0)
	{
		// No Hitscan, just a single hitbox	
		switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
		{
		case 0:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			break;
		case 2:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			break;
		}
	}
	else
	{
		switch (HitScanMode)
		{
		case 1:
			// head/body
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			break;
		case 2:
			// basic +(arms, thighs)
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
		case 3:
			// heaps ++(just all the random shit)
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
		}
	}
#pragma endregion Get the list of shit to scan

	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				c = Color(0, 255, 0, 255);
				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{
					return HitBoxID;
				}
			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::DoNoSpread(CUserCmd *pCmd)
{

}


void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}


bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd)
{
	bool ReturnValue = false;
	// Get the full angles
	if (point.Length() == 0) return ReturnValue;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	/*
	WTF IS THIS STUPID CHECK? OMG

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
	return ReturnValue;
	}
	*/

	IsLocked = true;
	//-----------------------------------------------

	// Aim Step Calcs
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; // Don't just use the viewangs because you need to consider aa

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}

	if (Menu::Window.RageBotTab.AimbotSilentAim.GetState() || Menu::Window.RageBotTab.AimbotPSilent.GetState())
	{
		pCmd->viewangles = angles;
	}

	if (!Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPSilent.GetState())
		Interfaces::Engine->SetViewAngles(angles);

	return ReturnValue;
}

namespace AntiAims
{
	// Pitches
	void StaticPitch(CUserCmd *pCmd, bool up)
	{
		pCmd->viewangles.x = 89;
	}

	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up) pCmd->viewangles.x = -88;
		else pCmd->viewangles.x = 88;
		up = !up;
	}
	void FakeDown(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -179.9995;
	}
	void Jitter(CUserCmd *pCmd) 
	{

		static int jitterangle = 0;

		if (jitterangle <= 2)
		{
			pCmd->viewangles.x = 89;
			jitterangle += 1;
		}
		else if (jitterangle > 2 && jitterangle <= 4)
		{
			pCmd->viewangles.x = 70;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}

	}

	void SilentXSpin(CUserCmd* pCmd)
	{
		int iSpinx = 0;
		iSpinx++;

		if (iSpinx > 1) {
			iSpinx = 0;
		}

		if (iSpinx == 1) {
			pCmd->viewangles.x = 89;
		}
	}
	void TerrorDown(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 180000089;
	}
	void Sleep(CUserCmd *pCmd)
	{
		pCmd->viewangles.z = 30000030000130 + INT_MAX ;
	}
	void CorrectLisp(CUserCmd* pCmd) {
		if (GetAsyncKeyState('W'))
			pCmd->forwardmove = 450.f;
		if (GetAsyncKeyState('S'))
			pCmd->forwardmove = -450.f;
	}
	void CorrectFakeDown(CUserCmd* pCmd) {
		if (GetAsyncKeyState('W'))
			pCmd->forwardmove = -450.f;
		if (GetAsyncKeyState('S'))
			pCmd->forwardmove = 450.f;
	}
	void FakeBody(CUserCmd *pCmd)
	{

		static int iChoked = -1;
		iChoked++;
		static bool baim;
		if (baim)
		{
			//*sendpacket->bSendPacket = 0;
			pCmd->viewangles.y -= 92;
		}
		else
		{
			//*sendpacket->bSendPacket = 1;
			pCmd->viewangles.y += 92;
			iChoked = -1;
		}


	}
	// Yaws
	void Sideways2(CUserCmd *pCmd)
	{
		static bool flip22;
		if (flip22)
		{
			pCmd->viewangles.y -= 270.0f;

		}
		else
		{
			pCmd->viewangles.y -= 90.0f;

		}
	}

	void Sideways(CUserCmd *pCmd)
	{
		int temp = pCmd->viewangles.y;
		temp = pCmd->viewangles.y + 90;
		if (temp > 135 && temp < -135)
			temp = pCmd->viewangles.y - 90;
		pCmd->viewangles.y = temp + 323210000;
	}
	void TJitter(CUserCmd *pCmd, bool& bSendPacket)
	{

			static bool Flip;
			static bool Flip2;

			Flip = !Flip;
			Flip2 = !Flip2;
			if (Flip)
			{
				if (Flip2)
				{
					pCmd->viewangles.y += 270.0f;

				}
				else
				{
					bSendPacket = false;
					pCmd->viewangles.y -= 90.0f;

				}


			}
			else
			{
				pCmd->viewangles.y -= 180.0f;
				bSendPacket = true;

			}
		
	}



	void FastSpint(CUserCmd *pCmd)
	{
		pCmd->viewangles.y += 85.f;
		
	}

	void SlowSpin(CUserCmd *pCmd)
	{
		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 5;
		else
			current_y -= 5;

		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;

	}

	void BackJitter(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = INT_MAX + 1;
		pCmd->viewangles.y = INT_MAX + 1;
		pCmd->viewangles.z = INT_MAX + 1;
	}
	void MoveFix(CUserCmd *cmd, Vector &realvec)
	{
		Vector vMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float flSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y), flYaw;
		Vector vMove2;
		VectorAngles(vMove, vMove2);

		flYaw = DEG2RAD(cmd->viewangles.y - realvec.y + vMove2.y);
		cmd->forwardmove = cos(flYaw) * flSpeed;
		cmd->sidemove = sin(flYaw) * flSpeed;

		if (cmd->viewangles.x < -90.f || cmd->viewangles.x > 90.f)
			cmd->forwardmove = -cmd->forwardmove;
	}
	void Flip(CUserCmd *pCmd)
	{
		static bool back = false;
		back = !back;
		if (back)
			pCmd->viewangles.y -= 180;
	}
	void Lisp(CUserCmd *pCmd)
	{
		static bool ySwitch = false;

		if (ySwitch)
			pCmd->viewangles.y -= 60602000140;
		else
			pCmd->viewangles.y += 60602000140;

		ySwitch = !ySwitch;
	}
	void AntiLisp(CUserCmd *pCmd)
	{
		int veloc1 = hackManager.pLocal()->GetVelocity().Length();
		pCmd->viewangles.y += 600030020120*veloc1;
	}
	
	void Edge(CUserCmd *pCmd)
	{
		static bool swap = false;
		if (swap) 
		{
			pCmd->viewangles.y -= 360;
			
		}
		else
		{
			pCmd->viewangles.y -= 180;
		}

		swap = !swap;
	}
	void FakeHead(CUserCmd *pCmd)
	{
		static bool ySwitch = false;
		int veloc = hackManager.pLocal()->GetVelocity().Length();
		if (ySwitch)
			pCmd->viewangles.y -= 1003004000*veloc;
		else
			pCmd->viewangles.y += 1801188099*veloc;

		ySwitch = !ySwitch;
	}
	
	Vector localPlyViewPos;
	void aimAA(CUserCmd *pCmd) {

		float len, closest = 8192;
		Vector pos;
		for (int i = 1; i <= Interfaces::Globals->maxClients; i++) {
			IClientEntity *ent = Interfaces::EntList->GetClientEntity(i);


			pos = ent->GetHeadPos();
			if (len > closest)
				continue;

			closest = len;
		}

		pCmd->viewangles.y = pos.y;
	}

	bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* pCmd, float flWall, float flCornor)
	{
		Ray_t ray;
		trace_t tr;

		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocalBaseEntity;

		auto bRetVal = false;
		auto vecCurPos = pLocalBaseEntity->GetOrigin() + pLocalBaseEntity->GetViewOffset();

		for (float i = 0; i < 360; i++)
		{
			Vector vecDummy(10.f, pCmd->viewangles.y, 0.f);
			vecDummy.y += i;

			vecDummy.NormalizeInPlace();

			Vector vecForward;
			AngleVectors(vecDummy, &vecForward);

			auto flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
			vecForward *= flLength;

			ray.Init(vecCurPos, (vecCurPos + vecForward));
			Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector qAngles;
				auto vecNegate = tr.plane.normal;

				vecNegate *= -1.f;
				VectorAngles(vecNegate, qAngles);

				vecDummy.y = qAngles.y;

				vecDummy.NormalizeInPlace();
				trace_t leftTrace, rightTrace;

				Vector vecLeft;
				AngleVectors(vecDummy + Vector(0.f, 30.f, 0.f), &vecLeft);

				Vector vecRight;
				AngleVectors(vecDummy - Vector(0.f, 30.f, 0.f), &vecRight);

				vecLeft *= (flLength + (flLength * sin(DEG2RAD(30.f))));
				vecRight *= (flLength + (flLength * sin(DEG2RAD(30.f))));

				ray.Init(vecCurPos, (vecCurPos + vecLeft));
				Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &leftTrace);

				ray.Init(vecCurPos, (vecCurPos + vecRight));
				Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &rightTrace);

				if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
					vecDummy.y -= flCornor; // left
				else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
					vecDummy.y += flCornor; // right           

				pCmd->viewangles.y = vecDummy.y;
				pCmd->viewangles.y -= flWall;
				bRetVal = true;
			}
		}

		return bRetVal;
	}

	void EdgeOut(IClientEntity* pLocal, float &fYawOut)
	{
		Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		CTraceFilter filter;
		filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		for (int y = 0; y < 360; y++)
		{
			Vector qTmp(10.0f, fYawOut, 0.0f);
			qTmp.y += y;

			if (qTmp.y > 180.0)
				qTmp.y -= 360.0;
			else if (qTmp.y < -180.0)
				qTmp.y += 360.0;

			GameUtils::NormaliseViewAngle(qTmp);

			Vector vForward;

			VectorAngles(qTmp, vForward);

			float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
			vForward *= fLength;

			trace_t tr;

			Vector vTraceEnd = vEyePos + vForward;

			Ray_t ray;

			ray.Init(vEyePos, vTraceEnd);
			Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector angles;

				Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

				VectorAngles(vNegative, angles);

				GameUtils::NormaliseViewAngle(angles);

				qTmp.y = angles.y;

				GameUtils::NormaliseViewAngle(qTmp);

				trace_t trLeft, trRight;

				Vector vLeft, vRight;
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

				vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
				vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

				vTraceEnd = vEyePos + vLeft;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

				vTraceEnd = vEyePos + vRight;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);

				if ((trLeft.fraction == 1.0f) && (trRight.fraction != 1.0f))
					qTmp.y -= 90.f;
				else if ((trLeft.fraction != 1.0f) && (trRight.fraction == 1.0f))
					qTmp.y += 90.f;

				if (qTmp.y > 180.0)
					qTmp.y -= 360.0;
				else if (qTmp.y < -180.0)
					qTmp.y += 360.0;

				fYawOut = qTmp.y;

				if (fYawOut > 180.0)
					fYawOut -= 360.0;
				else if (fYawOut < -180.0)
					fYawOut += 360.0;

				return;
			}
		}

		fYawOut -= 180.0f;

		if (fYawOut > 180.0)
			fYawOut -= 360.0;
		else if (fYawOut < -180.0)
			fYawOut += 360.0;
	}

	
}




// Edge Anti Aim

// AntiAim
// AntiAim
void CRageBot::DoAntiAim(CUserCmd *pCmd, bool& bSendPacket)
{
	// If the aimbot is doing something don't do anything
	if (IsAimStepping || pCmd->buttons & IN_ATTACK)
		return;

	// Weapon shit
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		if (!GameUtils::IsBallisticWeapon(pWeapon))
			return;
	}

	// Don't do antiaim
	// if (DoExit) return;

	// Anti-Aim Pitch
	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex())
	{
	case 0:
		// No Pitch AA
		break;
	case 1:
		// Down
		AntiAims::StaticPitch(pCmd, false);
		break;
	case 2:
		// Angel Down
		pCmd->viewangles.x = 1800089;
		break;
	case 3:
		// Jitter
		static bool bSwitch = false;

		if (bSwitch) {
			pCmd->viewangles.x = 89;
			bSwitch = false;
		}
		else {
			pCmd->viewangles.x = -89;
			bSwitch = true;
		}
	}

	//Anti-Aim Yaw
	switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
	{
	case 0:
		// No Yaw AA
		break;
	case 1:
		// Fast Spin
	{
		float CalculatedCurTime_1 = (Interfaces::Globals->curtime * 5000.0);
		pCmd->viewangles.y = CalculatedCurTime_1;
	}
	break;
	case 2:
		// Slow Spin
	{
		float CalculatedCurTime = (Interfaces::Globals->curtime * 2000.0);
		pCmd->viewangles.y = CalculatedCurTime;
	}
	break;
	case 3:
		// Inverse
		pCmd->viewangles.y -= 180;
		break;
	case 4:
		// Angel Inverse
		pCmd->viewangles.y = 36000180.000000;
		break;
	case 5:
		// Jitter
	{
		int v36 = pCmd->command_number % 3;
		if (v36 == 1)
		{
			pCmd->viewangles.y = pCmd->viewangles.y - 160.0;
		}
		else if (v36 == 2)
		{
			pCmd->viewangles.y = pCmd->viewangles.y - 200.0;
		}
	}
	break;
	case 6:
		// Jitter synced
		if (pCmd->command_number % 3)
			pCmd->viewangles.y = pCmd->viewangles.y + 200.0;
		break;
	case 7:
		// Fake Sideways
		if (bSendPacket)
			pCmd->viewangles.y = 90;
		else
			pCmd->viewangles.y -= 90;
	case 8:
		// Fake Backward
		if (bSendPacket)
			pCmd->viewangles.y = 180;
		else
			pCmd->viewangles.y -= 180;
		break;
	}
}