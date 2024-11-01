#ifndef SERVERBROWSERDIALOG_H
#define SERVERBROWSERDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/ListPanel.h>
#include "steam/steam_api.h"

class CSteamSessionsRequest;
//-----------------------------------------------------------------------------
// Purpose: dialog for launching a listenserver
//-----------------------------------------------------------------------------
class CServerBrowserDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE( CServerBrowserDialog, vgui::PropertyDialog );

public:
	CServerBrowserDialog( vgui::Panel* parent );
	~CServerBrowserDialog();

	virtual void Activate();

	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	virtual void OnCommand( const char *command );
	virtual void OnKeyCodeTyped( vgui::KeyCode code );

};

#endif
