#ifndef LOADGAMEDIALOG_H
#define LOADGAMEDIALOG_H
#pragma once

#include "basesavegamedialog.h"
#include "vgui_controls/Frame.h"

class CLoadGameDialog : public CBaseSaveGameDialog
{
	DECLARE_CLASS_SIMPLE(CLoadGameDialog, CBaseSaveGameDialog);
public:
	CLoadGameDialog(vgui::Panel* parent);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnCommand(const char* command);
};
#endif