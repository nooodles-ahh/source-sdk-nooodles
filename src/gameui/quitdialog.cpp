#include "quitdialog.h"
#include "gameui_interface.h"
#include <vgui/ISurface.h>
#include <vgui/IInput.h>

CQuitDialog::CQuitDialog( const char* title, const char* queryText, vgui::Panel* parent ) : 
	vgui::QueryBox( title, queryText, parent )
{
}

void CQuitDialog::DoModal( Frame* pFrameOver )
{
	BaseClass::DoModal( pFrameOver );
	g_pVGuiSurface->RestrictPaintToSinglePanel( GetVPanel() );
	GameUI().PreventEngineHideGameUI();
	//vgui::input()->SetMouseCapture(GetVPanel());
}

void CQuitDialog::OnClose( void )
{
	BaseClass::OnClose();
	g_pVGuiSurface->RestrictPaintToSinglePanel( NULL );
	GameUI().AllowEngineHideGameUI();
}

void CQuitDialog::OnKeyCodeTyped( vgui::KeyCode code )
{
	if ( code == KEY_ESCAPE )
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodeTyped( code );
	}
}

void CQuitDialog::OnKeyCodePressed( vgui::KeyCode code )
{
	if ( code == KEY_XBUTTON_B /*|| code == 172 */ )
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodePressed( code );
	}
}