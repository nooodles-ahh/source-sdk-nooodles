#ifndef QUITDIALOG_H
#define QUITDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/QueryBox.h>

class CQuitDialog : public vgui::QueryBox
{
	DECLARE_CLASS_SIMPLE( CQuitDialog, vgui::QueryBox );
	CQuitDialog( const char* title, const char* queryText, vgui::Panel* parent = nullptr );
	virtual void DoModal( Frame* pFrameOver = NULL );
	virtual void OnClose( void );
	virtual void OnKeyCodeTyped( vgui::KeyCode code );
	virtual void OnKeyCodePressed( vgui::KeyCode code );
};

#endif
