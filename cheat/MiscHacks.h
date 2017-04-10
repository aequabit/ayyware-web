

#pragma once

#include "Hacks.h"

Vector GetAutostrafeView();

class CMiscHacks : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool& bSendPacket);
	void OpenConsole(CUserCmd *pCmd);
private:

	void AutoJump(CUserCmd *pCmd);
	void AutoStrafe(CUserCmd *pCmd);
	void ChatSpamRegular();
	void ChatSpamReports();
};