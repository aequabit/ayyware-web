

// Credits to Valve and Shad0w
#include "Interfaces.h"
#include "Menu.h"
#include "AntiAntiAim.h"
// Shad0ws Yaw fix
// (FIX ME UP LATER)
void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;
	int enable = Menu::Window.RageBotTab.AccuracyAngleFix.GetState();
	if (enable > 1)
	{

		int rekaimware = 1;
		static bool rekt;
		if (rekt)
		{
			flYaw = 90.f;
			*(float*)(pOut) = flYaw;
			rekt = !rekt;
		}

		else
		{
			flYaw = -90.f;
			*(float*)(pOut) = flYaw;
			rekt = !rekt;
		}

	}
}

void FixX(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float flPitch = pData->m_Value.m_Float;

	if (flPitch > 180.0)
		flPitch -= 360.0;
	else if (flPitch < -180.0)
		flPitch += 360.0;

	//Fakedown autofix
	if (flPitch < -179.648438f || flPitch > 179.648438f)
	{
		flPitch = -10.0f;
	}

	if (flPitch <= -88.945313f && flPitch >= -179.648438f)
	{
		flPitch = -89.0f;
	}

	if (flPitch >= 88.945313f && flPitch <= 179.648438f)
	{
		flPitch = 89.0f;
	}


	if (flPitch > 89)
	{
		flPitch = 270;
	}
	if (flPitch < -89)
	{
		flPitch = 90;
	}

	if (flPitch > 36000088.000000)
	{
		flPitch = 89;
	}

	if (flPitch > 1800089)
	{
		flPitch = 89;
	}

	*(float*)pOut = flPitch;
}

RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	int iModel = pData->m_Value.m_Int;
	// Get the knife view model id's
	static int default_t = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	static int default_ct = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	static int bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	static int karambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	static int flip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	static int gut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	static int m9 = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	static int huntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	static int butterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	static int daggers = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
	static int falchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	static int bowie = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");

	// Get local player (just to stop replacing spectators knifes)
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Menu::Window.MiscTab.KnifeEnable.GetState() && pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (pData->m_Value.m_Int == default_t || pData->m_Value.m_Int == default_ct))
		{
			// Set whatever knife we want
			switch (Menu::Window.MiscTab.KnifeModel.GetIndex())
			{
			case 0:
				//none
				break;
				//bayonet karambit flip gut m9 hunts butterfly
			case 1:
				pData->m_Value.m_Int = bayonet; //bayonet
				break;
			case 2:
				pData->m_Value.m_Int = karambit; //karambit

				break;
			case 3:
				pData->m_Value.m_Int = flip; //flip
				break;
			case 4:
				pData->m_Value.m_Int = gut; //gut
				break;
			case 5:
				pData->m_Value.m_Int = m9; //m9bayo
				break;
			case 6:
				pData->m_Value.m_Int = huntsman; //hunts
				break;
			case 7:
				pData->m_Value.m_Int = butterfly; //butterfly
				break;
			case 8:
				pData->m_Value.m_Int = daggers; // shadow daggers
				break;
			case 9:
				pData->m_Value.m_Int = falchion; // falchion
				break;
			case 10:
				pData->m_Value.m_Int = bowie; // bowie knife
				break;
			}
		}
	}

	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}

void ApplyAAAHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Pitch Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{
					pProp->m_ProxyFn = FixX;
				}

				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{

					pProp->m_ProxyFn = FixY;
				}
			}
		}
		else if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}
