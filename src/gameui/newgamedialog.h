#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H
#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/KeyRepeat.h>
#include <utlvector.h>

using EScrollDirection = int;

class CNewGameDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CNewGameDialog, vgui::Frame);
public:
	CNewGameDialog(vgui::Panel* parent);
	~CNewGameDialog();

	virtual void Activate();
	virtual void ApplySettings(KeyValues* inResourceData);
	virtual void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme);
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void OnKeyCodeReleased(vgui::KeyCode code);
	virtual void OnThink();

	void AnimateSelectionPanels();
	void ContinueScrolling();
	void PostScroll(EScrollDirection dir);
	void ScrollSelectionPanels(EScrollDirection dir);
	void SetSelectedChapter(const char* name);
	void SetSelectedChapterIndex(int index);
	void ShiftPanelIndices(int i);
	void StartGame();
	void UpdateBonusSelection();
	void UpdateMenuComponents(EScrollDirection dir);

private:
	EScrollDirection m_eScrollDir1;
	int m_iDesiredChapter;
	CUtlVector<vgui::Panel*> m_vecChapterPanels;
	vgui::Button* m_pPlayButton;
	vgui::Button* m_pNextButton;
	vgui::Button* m_pPrevButton;
	vgui::Panel* m_pBGPanel;
	vgui::Label* m_pChapterTitleLabel;
	vgui::Label* m_pChapterDescriptionLabel;
	vgui::Label* m_pBonusChapterLabel;

	bool m_bIsCommentary;

	float m_flPanelXPos[5];
	float m_flPanelYPos[5];
	float m_flPanelAlpha[5];
	int m_iPanelSomething[5];

	float m_flScrollSpeed;
	EScrollDirection m_eScrollDir2;
	bool m_bSomething0;
	bool m_bSomething1;
	bool m_bSomething2;

	int m_iVar01;
	bool m_bVar01;
	int m_iVar03;
	vgui::CKeyRepeatHandler* m_pKeyRepeatHandler;

	//vgui::DHANDLE<CCommentaryExplanationDialog> m_hCommentaryDialog;
	vgui::DHANDLE<vgui::Panel> m_hCommentaryDialog;
};
#endif