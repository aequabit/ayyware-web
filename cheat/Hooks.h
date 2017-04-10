/*
Syn's AyyWare Framework 2015
*/

#pragma once

// It's actually in DLLMain but w/e
extern bool DoUnload;

#include "Utilities.h"

namespace Hooks
{
	void Initialise();
	void UndoHooks();

	// VMT Managers
	extern Utilities::Memory::CVMTHookManager VMTPanel; // Hooking drawing functions
	extern Utilities::Memory::CVMTHookManager VMTClient; // Maybe CreateMove
	extern Utilities::Memory::CVMTHookManager VMTClientMode; // CreateMove for functionality
	extern Utilities::Memory::CVMTHookManager VMTModelRender; // DrawModelEx for chams
	extern Utilities::Memory::CVMTHookManager VMTPrediction; // InPrediction for no vis recoil
};