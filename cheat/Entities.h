
#pragma once

#include "MiscDefinitions.h"
#include "ClientRecvProps.h"
#include "offsets.h"
#include "Vector.h"

#define TEAM_CS_T 2
#define TEAM_CS_CT 3

#define BONE_USED_BY_HITBOX			0x00000100

class IClientRenderable;
class IClientNetworkable;
class IClientUnknown;
class IClientThinkable;
class IClientEntity;
class CSWeaponInfo;

struct CHudTexture
{
	char	szShortName[64];	//0x0000
	char	szTextureFile[64];	//0x0040
	bool	bRenderUsingFont;	//0x0080
	bool	bPrecached;			//0x0081
	char	cCharacterInFont;	//0x0082
	BYTE	pad_0x0083;			//0x0083
	int		hFont;				//0x0084
	int		iTextureId;			//0x0088
	float	afTexCoords[4];		//0x008C
	int		iPosX[4];			//0x009C
}; //Size=0x00AC


class CSWeaponInfo
{
public:
	char _0x0000[2048];
	__int32 m_iAmmoType; //0x07B0 
	char _0x0804[52];
	__int32 m_iWeaponType; //0x0838 
	__int32 m_iTeam; //0x083C 
	__int32 m_iWeaponId; //0x0840 
	BYTE m_IsFullAuto; //0x0844
	char _0x0845[3];
	float m_flHeatPerShot; //0x0848 
	__int32 m_iWeaponPrice; //0x084C 
	float m_flArmorRatio; //0x0850 
	float m_flMaxPlayerSpeed; //0x0854 
	float m_flMaxPlayerSpeedAlt; //0x0858 
	__int32 m_iCrosshairMinDistance; //0x085C 
	__int32 m_iCrosshairDeltaDistance; //0x0860 
	float m_flPenetration; //0x0864 
	__int32 m_iDamage; //0x0868 
	float m_flRange; //0x086C 
	float m_flRangeModifier; //0x0870 
	__int32 m_iBullets; //0x0874 
	float m_flCycleTime; //0x0878 
	float m_flCycleTimeAlt; //0x087C 
	char _0x0880[416];
	float m_flSpread; //0x0A20 
	float m_flSpreadAlt; //0x0A24 
	float m_flInaccuracyCrouch; //0x09D8 
	float m_flInaccuracyCrouchAlt; //0x09DC 
	float m_flInaccuracyStand; //0x09E0 
	float m_flInaccuracyStandAlt; //0x09E4 
	float m_flInaccuracyJump; //0x09E8 
	float m_flInaccuracyJumpAlt; //0x09EC 
	float m_flInaccuracyLand; //0x09F0 
	float m_flInaccuracyLandAlt; //0x09F4 
	float m_flInaccuracyLadder; //0x09F8 
	float m_flInaccuracyLadderAlt; //0x09FC 
	float m_flInaccuracyFire; //0x0A00 
	float m_flInaccuracyFireAlt; //0x0A04 
	float m_flInaccuracyMove; //0x0A08 
	float m_flInaccuracyMoveAlt; //0x0A0C 
	float m_flRecoveryTimeStand; //0x0A10 
	float m_flRecoveryTimeCrouch; //0x0A14 
	float m_flInaccuracyReload; //0x0A18 
	float m_flInaccuracyAltSwitch; //0x0A1C 
	float m_flRecoilAngle; //0x0A20 
	float m_flRecoilAngleAlt; //0x0A24 
	float m_flRecoilAngleVariance; //0x0A28 
	float m_flRecoilAngleVarianceAlt; //0x0A2C 
	float m_flRecoilMagnitude; //0x0A30 
	float m_flRecoilMagnitudeAlt; //0x0A34 
	float m_flRecoilMagnitudeVariance; //0x0A38 
	float m_flRecoilMagnitudeVarianceAlt; //0x0A3C 
	__int32 m_iRecoilSeed; //0x0A40 
	float m_flFlinchVelocityModifierLarge; //0x0A44 
	float m_flFlinchVelocityModifierSmall; //0x0A48 
	float m_flTimeToIdle; //0x0A4C 
	float m_flIdleInterval; //0x0A50 
};

enum class CSGOClassID
{
	CTestTraceline = 192,
	CTEWorldDecal = 193,
	CTESpriteSpray = 190,
	CTESprite = 189,
	CTESparks = 188,
	CTESmoke = 187,
	CTEShowLine = 185,
	CTEProjectedDecal = 182,
	CTEPlayerDecal = 181,
	CTEPhysicsProp = 178,
	CTEParticleSystem = 177,
	CTEMuzzleFlash = 176,
	CTELargeFunnel = 174,
	CTEKillPlayerAttachments = 173,
	CTEImpact = 172,
	CTEGlowSprite = 171,
	CTEShatterSurface = 184,
	CTEFootprintDecal = 168,
	CTEFizz = 167,
	CTEExplosion = 165,
	CTEEnergySplash = 164,
	CTEEffectDispatch = 163,
	CTEDynamicLight = 162,
	CTEDecal = 160,
	CTEClientProjectile = 159,
	CTEBubbleTrail = 158,
	CTEBubbles = 157,
	CTEBSPDecal = 156,
	CTEBreakModel = 155,
	CTEBloodStream = 154,
	CTEBloodSprite = 153,
	CTEBeamSpline = 152,
	CTEBeamRingPoint = 151,
	CTEBeamRing = 150,
	CTEBeamPoints = 149,
	CTEBeamLaser = 148,
	CTEBeamFollow = 147,
	CTEBeamEnts = 146,
	CTEBeamEntPoint = 145,
	CTEBaseBeam = 144,
	CTEArmorRicochet = 143,
	CTEMetalSparks = 175,
	CSteamJet = 138,
	CSmokeStack = 131,
	DustTrail = 242,
	CFireTrail = 62,
	SporeTrail = 248,
	SporeExplosion = 247,
	RocketTrail = 245,
	SmokeTrail = 246,
	CPropVehicleDriveable = 118,
	ParticleSmokeGrenade = 244,
	CParticleFire = 97,
	MovieExplosion = 243,
	CTEGaussExplosion = 170,
	CEnvQuadraticBeam = 55,
	CEmbers = 45,
	CEnvWind = 59,
	CPrecipitation = 112,
	CPrecipitationBlocker = 113,
	CBaseTempEntity = 18,
	NextBotCombatCharacter = 0,
	CBaseAttributableItem = 4,
	CEconEntity = 44,
	CWeaponXM1014 = 240,
	CWeaponTaser = 235,
	CSmokeGrenade = 129,
	CWeaponSG552 = 232,
	CSensorGrenade = 125,
	CWeaponSawedoff = 228,
	CWeaponNOVA = 224,
	CIncendiaryGrenade = 85,
	CMolotovGrenade = 94,
	CWeaponM3 = 216,
	CKnifeGG = 91,
	CKnife = 90,
	CHEGrenade = 82,
	CFlashbang = 64,
	CWeaponElite = 207,
	CDecoyGrenade = 40,
	CDEagle = 39,
	CWeaponUSP = 239,
	CWeaponM249 = 215,
	CWeaponUMP45 = 238,
	CWeaponTMP = 237,
	CWeaponTec9 = 236,
	CWeaponSSG08 = 234,
	CWeaponSG556 = 233,
	CWeaponSG550 = 231,
	CWeaponScout = 230,
	CWeaponSCAR20 = 229,
	CSCAR17 = 123,
	CWeaponP90 = 227,
	CWeaponP250 = 226,
	CWeaponP228 = 225,
	CWeaponNegev = 223,
	CWeaponMP9 = 222,
	CWeaponMP7 = 221,
	CWeaponMP5Navy = 220,
	CWeaponMag7 = 219,
	CWeaponMAC10 = 218,
	CWeaponM4A1 = 217,
	CWeaponHKP2000 = 214,
	CWeaponGlock = 213,
	CWeaponGalilAR = 212,
	CWeaponGalil = 211,
	CWeaponG3SG1 = 210,
	CWeaponFiveSeven = 209,
	CWeaponFamas = 208,
	CWeaponBizon = 203,
	CWeaponAWP = 201,
	CWeaponAug = 200,
	CAK47 = 1,
	CWeaponCSBaseGun = 205,
	CWeaponCSBase = 204,
	CC4 = 29,
	CWeaponBaseItem = 202,
	CBaseCSGrenade = 8,
	CSmokeGrenadeProjectile = 130,
	CSensorGrenadeProjectile = 126,
	CMolotovProjectile = 95,
	CItem_Healthshot = 89,
	CDecoyProjectile = 41,
	CFireCrackerBlast = 60,
	CInferno = 86,
	CChicken = 31,
	CFootstepControl = 66,
	CCSGameRulesProxy = 34,
	CWeaponCubemap = 0,
	CWeaponCycler = 206,
	CTEPlantBomb = 179,
	CTEFireBullets = 166,
	CTERadioIcon = 183,
	CPlantedC4 = 105,
	CCSTeam = 38,
	CCSPlayerResource = 36,
	CCSPlayer = 35,
	CCSRagdoll = 37,
	CTEPlayerAnimEvent = 180,
	CHostage = 83,
	CHostageCarriableProp = 84,
	CBaseCSGrenadeProjectile = 9,
	CHandleTest = 81,
	CTeamplayRoundBasedRulesProxy = 142,
	CSpriteTrail = 136,
	CSpriteOriented = 135,
	CSprite = 134,
	CRagdollPropAttached = 121,
	CRagdollProp = 120,
	CPredictedViewModel = 114,
	CPoseController = 110,
	CGameRulesProxy = 80,
	CInfoLadderDismount = 87,
	CFuncLadder = 72,
	CTEFoundryHelpers = 169,
	CEnvDetailController = 51,
	CWorld = 241,
	CWaterLODControl = 199,
	CWaterBullet = 198,
	CVoteController = 197,
	CVGuiScreen = 196,
	CPropJeep = 117,
	CPropVehicleChoreoGeneric = 0,
	CTriggerSoundOperator = 195,
	CBaseVPhysicsTrigger = 22,
	CTriggerPlayerMovement = 194,
	CBaseTrigger = 20,
	CTest_ProxyToggle_Networkable = 191,
	CTesla = 186,
	CBaseTeamObjectiveResource = 17,
	CTeam = 141,
	CSunlightShadowControl = 140,
	CSun = 139,
	CParticlePerformanceMonitor = 98,
	CSpotlightEnd = 133,
	CSpatialEntity = 132,
	CSlideshowDisplay = 128,
	CShadowControl = 127,
	CSceneEntity = 124,
	CRopeKeyframe = 122,
	CRagdollManager = 119,
	CPhysicsPropMultiplayer = 103,
	CPhysBoxMultiplayer = 101,
	CPropDoorRotating = 116,
	CBasePropDoor = 16,
	CDynamicProp = 43,
	CProp_Hallucination = 115,
	CPostProcessController = 111,
	CPointCommentaryNode = 109,
	CPointCamera = 108,
	CPlayerResource = 107,
	CPlasma = 106,
	CPhysMagnet = 104,
	CPhysicsProp = 102,
	CStatueProp = 137,
	CPhysBox = 100,
	CParticleSystem = 99,
	CMovieDisplay = 96,
	CMaterialModifyControl = 93,
	CLightGlow = 92,
	CInfoOverlayAccessor = 88,
	CFuncTrackTrain = 79,
	CFuncSmokeVolume = 78,
	CFuncRotating = 77,
	CFuncReflectiveGlass = 76,
	CFuncOccluder = 75,
	CFuncMoveLinear = 74,
	CFuncMonitor = 73,
	CFunc_LOD = 68,
	CTEDust = 161,
	CFunc_Dust = 67,
	CFuncConveyor = 71,
	CFuncBrush = 70,
	CBreakableSurface = 28,
	CFuncAreaPortalWindow = 69,
	CFish = 63,
	CFireSmoke = 61,
	CEnvTonemapController = 58,
	CEnvScreenEffect = 56,
	CEnvScreenOverlay = 57,
	CEnvProjectedTexture = 54,
	CEnvParticleScript = 53,
	CFogController = 65,
	CEnvDOFController = 52,
	CCascadeLight = 30,
	CEnvAmbientLight = 50,
	CEntityParticleTrail = 49,
	CEntityFreezing = 48,
	CEntityFlame = 47,
	CEntityDissolve = 46,
	CDynamicLight = 42,
	CColorCorrectionVolume = 33,
	CColorCorrection = 32,
	CBreakableProp = 27,
	CBeamSpotlight = 25,
	CBaseButton = 5,
	CBaseToggle = 19,
	CBasePlayer = 15,
	CBaseFlex = 12,
	CBaseEntity = 11,
	CBaseDoor = 10,
	CBaseCombatCharacter = 6,
	CBaseAnimatingOverlay = 3,
	CBoneFollower = 26,
	CBaseAnimating = 2,
	CAI_BaseNPC = 0,
	CBeam = 24,
	CBaseViewModel = 21,
	CBaseParticleEntity = 14,
	CBaseGrenade = 13,
	CBaseCombatWeapon = 7,
	CBaseWeaponWorldModel = 23
};

enum class CSGOHitboxID
{
	Head = 0,
	Neck,
	NeckLower,
	Pelvis,
	Stomach,
	LowerChest,
	Chest,
	UpperChest,
	RightThigh,
	LeftThigh,
	RightShin,
	LeftShin,
	RightFoot,
	LeftFoot,
	RightHand,
	LeftHand,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm
};

class ScriptCreatedItem
{
public:
	CPNETVAR_FUNC(int*, ItemDefinitionIndex, 0xE67AB3B8); //m_iItemDefinitionIndex
	CPNETVAR_FUNC(int*, ItemIDHigh, 0x714778A); //m_iItemIDHigh
	CPNETVAR_FUNC(int*, ItemIDLow, 0x3A3DFC74); //m_iItemIDLow
	CPNETVAR_FUNC(char**, szCustomName, 0x13285ad9); //m_szCustomName
};

class AttributeContainer
{
public:
	CPNETVAR_FUNC(ScriptCreatedItem*, m_Item, 0x7E029CE5);
};


class CBaseCombatWeapon
{
public:
	CNETVAR_FUNC(float, GetNextPrimaryAttack, 0xDB7B106E); //m_flNextPrimaryAttack
	CNETVAR_FUNC(int, GetAmmoInClip, 0x97B6F70C); //m_iClip1
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin

	CPNETVAR_FUNC(int*, FallbackPaintKit, 0xADE4C870); // m_nFallbackPaintKit
	CPNETVAR_FUNC(int*, FallbackSeed, 0xC2D0683D); // m_nFallbackSeed
	CPNETVAR_FUNC(float*, FallbackWear, 0xA263576C); //m_flFallbackWear
	CPNETVAR_FUNC(int*, FallbackStatTrak, 0x1ED78768); //m_nFallbackStatTrak

	CPNETVAR_FUNC(AttributeContainer*, m_AttributeManager, 0xCFFCE089);

	float GetInaccuracy()
	{
		typedef float(__thiscall* oGetSpread)(PVOID);
		return call_vfunc< oGetSpread>(this, Offsets::VMT::Weapon_GetSpread)(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall* oGetInac)(PVOID);
		return call_vfunc< oGetInac>(this, Offsets::VMT::Weapon_GetSpread + 1)(this);
	}

	void UpdateAccuracyPenalty()
	{
		typedef void(__thiscall* oUpdateAccuracyPenalty)(PVOID);
		return call_vfunc< oUpdateAccuracyPenalty>(this, Offsets::VMT::Weapon_GetSpread + 2)(this);
	}

	CSWeaponInfo* GetCSWpnData()
	{
		static DWORD GetCSWpnDataAddr = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x57", "xxxxx????x????x");
		if (GetCSWpnDataAddr)
		{
			CSWeaponInfo* retData;
			__asm
			{
				mov ecx, this
				call GetCSWpnDataAddr
				mov retData, eax
			}
			return retData;
		}
		else
		{
			return nullptr;
		}
	}
};

class CCSBomb
{
public:
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(float, GetC4BlowTime, 0xB5E0CA1C); //m_flC4Blow
	CNETVAR_FUNC(float, GetC4DefuseCountDown, 0xB959B4A6); //m_flDefuseCountDown
};

class CLocalPlayerExclusive
{
public:
	CNETVAR_FUNC(Vector, GetViewPunchAngle, 0x68F014C0);//m_viewPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngle, 0xBF25C290);//m_aimPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngleVel, 0x8425E045);//m_aimPunchAngleVel
};

class CollisionProperty
{
public:
	CNETVAR_FUNC(Vector, GetMins, 0xF6F78BAB);//m_vecMins
	CNETVAR_FUNC(Vector, GetMaxs, 0xE47C6FC4);//m_vecMaxs
	CNETVAR_FUNC(unsigned char, GetSolidType, 0xB86722A1);//m_nSolidType
	CNETVAR_FUNC(unsigned short, GetSolidFlags, 0x63BB24C1);//m_usSolidFlags
	CNETVAR_FUNC(int, GetSurroundsType, 0xB677A0BB); //m_nSurroundType

	bool IsSolid()
	{
		return (GetSolidType() != SOLID_NONE) && ((GetSolidFlags() & FSOLID_NOT_SOLID) == 0);
	}
};

class IClientRenderable
{
public:
	//virtual void*					GetIClientUnknown() = 0;
	virtual Vector const&			GetRenderOrigin(void) = 0;
	virtual Vector const&			GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;
	virtual void					GetShadowHandle() const = 0;
	virtual void*					RenderHandle() = 0;
	virtual const model_t*				GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;
	virtual int						GetBody() = 0;
	virtual void					ComputeFxBlend() = 0;

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* oSetupBones)(PVOID, matrix3x4*, int, int, float);
		return call_vfunc< oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
};

class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;// FOR NETVARS FIND YOURSELF ClientClass* stuffs
	virtual void			NotifyShouldTransmit( /* ShouldTransmitState_t state*/) = 0;
	virtual void			OnPreDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			OnDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PreDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PostDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			unknown();
	virtual bool			IsDormant(void) = 0;
	virtual int				GetIndex(void) const = 0;
	virtual void			ReceiveMessage(int classID /*, bf_read &msg*/) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientUnknown
{
public:
	virtual void*		GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IClientEntity*		GetIClientEntity() = 0;
	virtual IClientEntity*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual void*				GetThinkHandle() = 0;
	virtual void				SetThinkHandle(void* hThink) = 0;
	virtual void				Release() = 0;
};

class __declspec (novtable)IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
public:
	virtual void			Release(void) = 0;
	virtual void			blahblahpad(void) = 0;
	virtual Vector&	GetAbsOrigin(void) const = 0;//in broken place use GetOrigin Below
	virtual const Vector&	GetAbsAngles(void) const = 0;

	//---                 NetVars                  ---//

	CPNETVAR_FUNC(CLocalPlayerExclusive*, localPlayerExclusive, 0x7177BC3E);// m_Local
	CPNETVAR_FUNC(CollisionProperty*, collisionProperty, 0xE477CBD0);//m_Collision

	CNETVAR_FUNC(int, GetFlags, 0xE456D580); //m_fFlags
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin
	CNETVAR_FUNC(Vector, GetRotation, 0x6BEA197A); //m_angRotation
	CNETVAR_FUNC(int, GetTeamNum, 0xC08B6C6E); //m_iTeamNum
	CNETVAR_FUNC(int, GetMaxHealth, 0xC52E1C28); //m_iMaxHealth
	CNETVAR_FUNC(int, GetHealth, 0xA93054E3); //m_iHealth
	CNETVAR_FUNC(unsigned char, GetLifeState, 0xD795CCFC); //m_lifeState
	CNETVAR_FUNC(HANDLE, GetActiveWeaponHandle, 0xB4FECDA3); //m_hActiveWeapon
	CNETVAR_FUNC(int, GetTickBase, 0xD472B079); //m_nTickBase
	CNETVAR_FUNC(Vector, GetViewOffset, 0xA9F74931); //m_vecViewOffset[0]
	CNETVAR_FUNC(Vector, GetVelocity, 0x40C1CA24); //m_vecVelocity[0]
	CNETVAR_FUNC(bool, HasGunGameImmunity, 0x6AD6FA0D); //m_bGunGameImmunity
	CNETVAR_FUNC(bool, IsDefusing, 0xA2C14106); //m_bIsDefusing
	CNETVAR_FUNC(int, ArmorValue, 0x3898634); //m_ArmorValue
	CNETVAR_FUNC(bool, HasHelmet, 0x7B97F18A); //m_bHasHelmet
	CNETVAR_FUNC(bool, IsScoped, 0x61B9C22C); //m_bIsScoped
	CNETVAR_FUNC(int, GetMoney, 0xF4B3E183); //m_iAccount
	CNETVAR_FUNC(HANDLE, GetObserverTargetHandle, 0x8660FD83); //m_hObserverTarget
	inline float getnextattack() { return nigvar(float, this, 0x2D60); }
	// ----------------------------------------------//

	bool IsAlive()
	{
		return (GetLifeState() == LIFE_ALIVE && GetHealth() > 0);
	}

	Vector GetBonePos(int i)
	{
		matrix3x4 boneMatrix[128];
		if (this->SetupBones(boneMatrix, 128, BONE_USED_BY_HITBOX, GetTickCount64()))
		{
			return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
		}
		return Vector(0, 0, 0);
	}

	Vector GetHeadPos()
	{
		return this->GetBonePos(6);
	}

};