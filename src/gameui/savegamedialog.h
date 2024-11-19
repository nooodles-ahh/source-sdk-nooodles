#ifndef SAVEGAMEDIALOG_H
#define SAVEGAMEDIALOG_H
#pragma once

#include "basesavegamedialog.h"
#include "vgui_controls/Frame.h"

class CSaveGameDialog : public CBaseSaveGameDialog
{
	DECLARE_CLASS_SIMPLE(CSaveGameDialog, CBaseSaveGameDialog);
public:
	CSaveGameDialog(vgui::Panel* parent);

	virtual void Activate();
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnCommand(const char* command);

	static void FindSaveSlot(OUT_Z_CAP(bufsize) char* buffer, int bufsize);
	virtual void OnScanningSaveGames();
};
#endif