//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef GAMEUI_UTIL_H
#define GAMEUI_UTIL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/convar.h"
#include "vgui_controls/Panel.h"

extern ConVar gameui_proportional_scaling;

void GameUI_MakeSafeName( const char *oldName, char *newName, int newNameBufSize );

#endif // GAMEUI_UTIL_H
