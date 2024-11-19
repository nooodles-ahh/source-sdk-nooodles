#include "loadgamedialog.h"
#include "vgui_controls/Button.h"
#include "engineinterface.h"
#include "gameui_util.h"
#include "vgui_controls/PanelListPanel.h"
#include "vgui_controls/QueryBox.h"

using namespace vgui;

DECLARE_GAMEUI_DIALOG_FACTORY(OpenLoadGameDialog, CLoadGameDialog)

CLoadGameDialog::CLoadGameDialog(vgui::Panel* parent) : CBaseSaveGameDialog(parent, "LoadGameDialog")
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
	SetTitle("#GameUI_LoadGame", true);

	Button* pButton = new Button(this, "Cancel", "#GameUI_Cancel");
	pButton->SetCommand("Close");

	SetControlEnabled("delete", false);
}

void CLoadGameDialog::ApplySchemeSettings(vgui::IScheme* pScheme)
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

	LoadControlSettings("Resource/LoadGameDialog.res");
	SetControlEnabled("delete", false);
}

void CLoadGameDialog::OnCommand(const char* command)
{
	if (!V_stricmp(command, "loadsave"))
	{
		int iSlot = GetSelectedItemSaveIndex();
		if (iSlot >= 0 && iSlot < m_SaveGames.Count())
		{
			const char* shortName = m_SaveGames[iSlot].szShortName;
			if (shortName && shortName[0])
			{
				// Load the game, return to top and switch to engine
				char szCommand[256];
				V_snprintf(szCommand, sizeof(szCommand), "progress_enable\nload %s\n", shortName);
				engine->ClientCmd_Unrestricted(szCommand);
				Close();
			}
		}
	}
	else if(!V_stricmp(command, "Delete"))
	{
		int iSlot = GetSelectedItemSaveIndex();
		if (iSlot >= 0 && iSlot < m_SaveGames.Count())
		{
			QueryBox* pDeleteDialog = new QueryBox("#GameUI_ConfirmDeleteSaveGame_Title", "#GameUI_ConfirmDeleteSaveGame_Info", this);
			if (pDeleteDialog != NULL)
			{
				pDeleteDialog->SetOKCommand(new KeyValues("Command", "command", "DeleteConfirmed"));
				pDeleteDialog->SetOKButtonText("#GameUI_ConfirmDeleteSaveGame_OK");
				pDeleteDialog->AddActionSignalTarget(this);
				pDeleteDialog->DoModal();
			}
		}
	}
	else if(!V_stricmp(command, "DeleteConfirmed"))
	{
		int iSlot = GetSelectedItemSaveIndex();
		if (iSlot >= 0 && iSlot < m_SaveGames.Count())
		{
			DeleteSaveGame(m_SaveGames[iSlot].szFileName);
			ScanSavedGames();
			m_pGameList->MoveScrollBarToTop();
		}
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}
