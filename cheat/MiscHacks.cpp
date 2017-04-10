
#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "DLLMain.h"
#include <time.h>

#define charenc( s ) ( s )

Vector AutoStrafeView;

void CMiscHacks::Init()
{
	
	// Any init
}

void CMiscHacks::Draw()
{
	// Any drawing	
}
void CMiscHacks::OpenConsole(CUserCmd *pCmd)
{
	//static bool rekt;
	//if (rekt)
	//{
	//	Interfaces::Engine->ClientCmd_Unrestricted("openconsole");
	//	Interfaces::Engine->ClientCmd_Unrestricted("echo ayyware Loaded");
	//	Interfaces::Engine->ClientCmd_Unrestricted("echo Build Compiled At: " __DATE__);
	//	rekt = !rekt;
	//}
}



void CMiscHacks::Move(CUserCmd *pCmd, bool& bSendPacket)
{
	// Any Move Stuff
	OpenConsole(pCmd);
	// Bhop
	if (Menu::Window.MiscTab.OtherAutoJump.GetState())
		AutoJump(pCmd);

	// Strafe
	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	if (Menu::Window.MiscTab.OtherAutoStrafe.GetState())
		AutoStrafe(pCmd);

	int AirStuckKey = Menu::Window.MiscTab.OtherAirStuck.GetKey();
	if (AirStuckKey > 0 && GUI.GetKeyState(AirStuckKey))
	{
		//pCmd->forwardmove = NULL;
		//pCmd->sidemove = NULL;
		pCmd->tick_count = INT_MAX;
		//pCmd->sidemove = 0;
		//pCmd->forwardmove = 0;
		//pCmd->aimdirection += 10;
		//Interfaces::CVar->FindVar("+right")->SetValue("1");
		
	}

	// Spams
	switch (Menu::Window.MiscTab.OtherChatSpam.GetIndex())
	{
	case 0:
		// No Chat Spam
		break;
	case 1:
		// Round Say
		break;
	case 2:
		// Regular
		ChatSpamRegular();
		break;
	case 3:
		// Report Spam
		ChatSpamReports();
		break;
	}

}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 150)
		{


				//pCmd->forwardmove = 450.f;

			}

		}
	}

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}



inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}


inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

void CMiscHacks::AutoStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	static bool bDirection = true;

	if (!(pLocal->GetFlags() & FL_ONGROUND)) {
		if (pCmd->mousedx < 0)
			pCmd->sidemove = -450.0f;
		else if
			(pCmd->mousedx > 0)
			pCmd->sidemove = 450.0f;
	}
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}

void CMiscHacks::ChatSpamReports()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.5)
		return;

	// Loop through all active entitys
	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() != entity->GetTeamNum())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	int randomIndex = rand() % Names.size();
	char buffer[128];
	static unsigned long int meme = 3091961887844204720;
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str(), meme);
	meme += 1;
	SayInChat(buffer);
	start_t = clock();
}

void CMiscHacks::ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 1.5)
		return;

	SayInChat("insert chatspam here//Paste tutorial by Harcuz");

	start_t = clock();
}
