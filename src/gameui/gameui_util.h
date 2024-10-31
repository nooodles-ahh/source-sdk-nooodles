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
#ifndef GAMEUI_EMBEDDED
char	*VarArgs( PRINTF_FORMAT_STRING const char *format, ... );
const char *COM_GetModDirectory();
#endif

using DIALOGCREATEFUNC = vgui::Frame* (*)(vgui::Panel* parent);

class CDialogFactoryHelper
{
public:
	// Static list of helpers
	static CDialogFactoryHelper* m_sHelpers;

public:
	CDialogFactoryHelper(char const* className, DIALOGCREATEFUNC func);
	CDialogFactoryHelper* GetNext(void);

	static vgui::Frame* InstancePanel(char const* className, vgui::Panel* pParent);
	static void GetFactoryNames(CUtlVector< char const* >& list);

	char const* GetClassName() const;
	vgui::Frame* CreatePanel(vgui::Panel* pParent);

private:
	static bool HasFactory(char const* className);

	CDialogFactoryHelper* m_pNext;

	int					m_Type;
	DIALOGCREATEFUNC		m_CreateFunc;
	char const* m_pClassName;
};

#define DECLARE_GAMEUI_DIALOG_FACTORY( command, className )			\
	static vgui::Frame *CON_##command( vgui::Panel* parent )		\
		{															\
			static vgui::DHANDLE<vgui::Frame> panel; 					\
			if(!panel.Get()) panel = new className( parent, NULL );	\
			return panel;											\
		};															\
		static CDialogFactoryHelper g_##className##_DialogHelper( #command, CON_##command );\
	className *g_##className##DialogLinkerHack = NULL;


#endif // GAMEUI_UTIL_H
