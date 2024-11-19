#ifndef QUITDIALOG_H
#define QUITDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/QueryBox.h>

// A self important query box that restricts drawing to itself and prevents ESC from closing GameUI
class EgoQueryBox : public vgui::QueryBox
{
	DECLARE_CLASS_SIMPLE(EgoQueryBox, vgui::QueryBox );
	EgoQueryBox( const char* title, const char* queryText, vgui::Panel* parent = nullptr );
	virtual void DoModal( Frame* pFrameOver = NULL );
	virtual void OnClose( void );
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
	virtual void OnKeyCodePressed(vgui::KeyCode code);
};
#endif
