//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#include <stdarg.h>
#include "gameui_util.h"
#include "strtools.h"
#include "engineinterface.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar gameui_proportional_scaling( "gameui_proportional_scaling", "0", FCVAR_NONE, "Scale the menu proportionally with the screen resolution" );

//-----------------------------------------------------------------------------
// Purpose: Scans player names
//Passes the player name to be checked in a KeyValues pointer
//with the keyname "name"
// - replaces '&' with '&&' so they will draw in the scoreboard
// - replaces '#' at the start of the name with '*'
//-----------------------------------------------------------------------------

void GameUI_MakeSafeName( const char *oldName, char *newName, int newNameBufSize )
{
	if ( !oldName )
	{
		newName[0] = 0;
		return;
	}

	int newpos = 0;

	for( const char *p=oldName; *p != 0 && newpos < newNameBufSize-1; p++ )
	{
		//check for a '#' char at the beginning

		/*
		if( p == oldName && *p == '#' )
				{
					newName[newpos] = '*';
					newpos++;
				}
				else */

		if( *p == '%' )
		{
			// remove % chars
			newName[newpos] = '*';
			newpos++;
		}
		else if( *p == '&' )
		{
			//insert another & after this one
			if ( newpos+2 < newNameBufSize )
			{
				newName[newpos] = '&';
				newName[newpos+1] = '&';
				newpos+=2;
			}
		}
		else
		{
			newName[newpos] = *p;
			newpos++;
		}
	}
	newName[newpos] = 0;
}

#ifndef GAMEUI_EMBEDDED
char *VarArgs( const char *format, ... )
{
	va_list		argptr;
	static char		string[1024];
	
	va_start (argptr, format);
	Q_vsnprintf (string, sizeof( string ), format,argptr);
	va_end (argptr);

	return string;	
}

const char *COM_GetModDirectory()
{
	static char modDir[MAX_PATH];
	if ( Q_strlen( modDir ) == 0 )
	{
		const char *gamedir = CommandLine()->ParmValue("-game", CommandLine()->ParmValue( "-defaultgamedir", "hl2" ) );
		Q_strncpy( modDir, gamedir, sizeof(modDir) );
		if ( strchr( modDir, '/' ) || strchr( modDir, '\\' ) )
		{
			Q_StripLastDir( modDir, sizeof(modDir) );
			int dirlen = Q_strlen( modDir );
			Q_strncpy( modDir, gamedir + dirlen, sizeof(modDir) - dirlen );
		}
	}

	return modDir;
}
#endif


CDialogFactoryHelper* CDialogFactoryHelper::m_sHelpers = NULL;

CDialogFactoryHelper::CDialogFactoryHelper(char const* className, DIALOGCREATEFUNC func)
{
	// Make this fatal
	if (HasFactory(className))
	{
		Error("CBuildFactoryHelper:  Factory for '%s' already exists!!!!\n", className);
	}

	//List is empty, or element belongs at front, insert here
	m_pNext = m_sHelpers;
	m_sHelpers = this;

	Assert(func);
	m_CreateFunc = func;
	Assert(className);
	m_pClassName = className;
}

CDialogFactoryHelper* CDialogFactoryHelper::GetNext(void)
{
	return m_pNext;
}

char const* CDialogFactoryHelper::GetClassName() const
{
	return m_pClassName;
}

vgui::Frame* CDialogFactoryHelper::CreatePanel(vgui::Panel* pParent)
{
	if (!m_CreateFunc)
		return NULL;

	return (*m_CreateFunc)(pParent);
}

// private static meethod
bool CDialogFactoryHelper::HasFactory(char const* className)
{
	CDialogFactoryHelper* p = m_sHelpers;
	while (p)
	{
		if (!Q_stricmp(className, p->GetClassName()))
			return true;

		p = p->GetNext();
	}
	return false;
}

// static method
vgui::Frame* CDialogFactoryHelper::InstancePanel(char const* className, vgui::Panel* pParent)
{
	CDialogFactoryHelper* p = m_sHelpers;
	while (p)
	{
		if (!Q_stricmp(className, p->GetClassName()))
			return p->CreatePanel(pParent);

		p = p->GetNext();
	}
	return NULL;
}

// static method
void CDialogFactoryHelper::GetFactoryNames(CUtlVector< char const* >& list)
{
	list.RemoveAll();

	CDialogFactoryHelper* p = m_sHelpers;
	while (p)
	{
		list.AddToTail(p->GetClassName());
		p = p->GetNext();
	}
}

