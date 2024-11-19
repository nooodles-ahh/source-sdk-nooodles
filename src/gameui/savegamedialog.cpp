#include "savegamedialog.h"
#include "engineinterface.h"
#include "gameui_util.h"
#include "gameui_interface.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/QueryBox.h"
#include "filesystem.h"

using namespace vgui;

#define NEW_SAVE_GAME_TIMESTAMP	0xFFFFFFFF

DECLARE_GAMEUI_DIALOG_FACTORY(OpenSaveGameDialog, CSaveGameDialog)

CSaveGameDialog::CSaveGameDialog(vgui::Panel* parent) : CBaseSaveGameDialog(parent, "SaveGameDialog")
{
	SetScheme(parent->GetScheme());
	SetDeleteSelfOnClose(true);
	SetBounds(
		0,
		0,
		PROPORTIONAL_VALUE(512),
		PROPORTIONAL_VALUE(384)
	);
	SetMinimumSize(
		PROPORTIONAL_VALUE(256),
		PROPORTIONAL_VALUE(300)
	);

	SetSizeable(true);
	SetTitle("#GameUI_SaveGame", true);

	Button* pButton = new Button(this, "Cancel", "#GameUI_Cancel");
	pButton->SetCommand("Close");

}

void CSaveGameDialog::Activate()
{
	BaseClass::Activate();
	ScanSavedGames();
}

void CSaveGameDialog::ApplySchemeSettings(vgui::IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetBounds(
		0,
		0,
		PROPORTIONAL_VALUE(512),
		PROPORTIONAL_VALUE(384)
	);
	SetMinimumSize(
		PROPORTIONAL_VALUE(256),
		PROPORTIONAL_VALUE(300)
	);

	LoadControlSettings("Resource/SaveGameDialog.res");
}

void CSaveGameDialog::OnCommand(const char* command)
{
	if (!V_stricmp(command, "loadsave"))
	{
		int iSlot = GetSelectedItemSaveIndex();
		if (iSlot >= 0 && iSlot < m_SaveGames.Count())
		{
			if (m_SaveGames[iSlot].iTimestamp == NEW_SAVE_GAME_TIMESTAMP)
			{
				OnCommand("SaveOverwriteConfirmed");
			}
			else
			{
				QueryBox* pDeleteDialog = new QueryBox("#GameUI_ConfirmOverwriteSaveGame_Title", "#GameUI_ConfirmOverwriteSaveGame_Info", this);
				if (pDeleteDialog != NULL)
				{
					pDeleteDialog->SetOKCommand(new KeyValues("Command", "command", "SaveOverwriteConfirmed"));
					pDeleteDialog->SetOKButtonText("#GameUI_ConfirmOverwriteSaveGame_OK");
					pDeleteDialog->AddActionSignalTarget(this);
					pDeleteDialog->DoModal();
				}
			}
		}
	}
	else if (!V_stricmp(command, "SaveOverwriteConfirmed"))
	{
		int iSlot = GetSelectedItemSaveIndex();
		if (iSlot >= 0 && iSlot < m_SaveGames.Count())
		{
			// delete any existing save
			DeleteSaveGame(m_SaveGames[iSlot].szFileName);

			// save to a new name
			char saveName[128];
			FindSaveSlot(saveName, sizeof(saveName));
			if (saveName && saveName[0])
			{
				// Load the game, return to top and switch to engine
				char sz[256];
				Q_snprintf(sz, sizeof(sz), "save %s\n", saveName);

				engine->ClientCmd_Unrestricted(sz);
				Close();
				GameUI().SendMainMenuCommand("resumegame");
			}
		}
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CSaveGameDialog::FindSaveSlot(OUT_Z_CAP(bufsize) char* buffer, int bufsize)
{
	buffer[0] = 0;
	char szFileName[_MAX_PATH];
	for (int i = 0; i < 1000; i++)
	{
		Q_snprintf(szFileName, sizeof(szFileName), "save/half-life-%03i.sav", i);

		FileHandle_t fp = g_pFullFileSystem->Open(szFileName, "rb");
		if (!fp)
		{
			// clean up name
			Q_strncpy(buffer, szFileName + 5, bufsize);
			char* ext = strstr(buffer, ".sav");
			if (ext)
			{
				*ext = 0;
			}
			return;
		}
		g_pFullFileSystem->Close(fp);
	}

	AssertMsg(false, "Could not generate new save game file");
}

void CSaveGameDialog::OnScanningSaveGames()
{
	m_SaveGames.AddToTail(SaveGameDescription_t{ "NewSavedGame", "", "", "#GameUI_NewSaveGame", "", "", "Current", NEW_SAVE_GAME_TIMESTAMP });
}
