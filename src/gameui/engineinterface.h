//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Includes all the headers/declarations necessary to access the
//			engine interface
//
// $NoKeywords: $
//=============================================================================//

#ifndef ENGINEINTERFACE_H
#define ENGINEINTERFACE_H

#ifdef _WIN32
#pragma once
#endif

// these stupid set of includes are required to use the cdll_int interface
#include "mathlib/vector.h"
//#include "wrect.h"
#define IN_BUTTONS_H

// engine interface
#include "cdll_int.h"
#include "eiface.h"
#include "icvar.h"
#include "tier2/tier2.h"
#include "ienginevgui.h"
#ifdef GAMEUI_EMBEDDED
#include "cdll_client_int.h"
#else
#include "engine/IEngineSound.h"
#include "IGameUIFuncs.h"
#endif

// engine interface singleton accessors
extern IEngineVGui* enginevguifuncs;
extern class IAchievementMgr *achievementmgr; 
#ifndef GAMEUI_EMBEDDED
extern IVEngineClient *engine;
extern IEngineSound *enginesound;
extern IGameUIFuncs *gameuifuncs;

class CSteamAPIContext;
extern CSteamAPIContext *steamapicontext;
#endif

#endif // ENGINEINTERFACE_H
