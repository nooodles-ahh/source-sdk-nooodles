#ifndef LOGOPANEL_H
#define LOGOPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/EditablePanel.h"

class CLogoPanel : public vgui::EditablePanel
{
private:
	DECLARE_CLASS_SIMPLE( CLogoPanel, vgui::EditablePanel );

public:
	CLogoPanel( vgui::Panel *parent );
	~CLogoPanel();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void ApplySettings(KeyValues* inResourceData);

private:
	int m_iOffsetX;
	int m_iOffsetY;
};

#endif