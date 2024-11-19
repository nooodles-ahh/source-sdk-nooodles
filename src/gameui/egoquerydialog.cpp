#include "egoquerydialog.h"
#include "gameui_interface.h"
#include <vgui/ISurface.h>
#include <vgui/IInput.h>

EgoQueryBox::EgoQueryBox( const char* title, const char* queryText, vgui::Panel* parent ) :
	vgui::QueryBox( title, queryText, parent )
{
}

void EgoQueryBox::DoModal( Frame* pFrameOver )
{
	BaseClass::DoModal( pFrameOver );
	g_pVGuiSurface->RestrictPaintToSinglePanel(GetVPanel());
	GameUI().PreventEngineHideGameUI();
}

void EgoQueryBox::OnClose( void )
{
	BaseClass::OnClose();
	g_pVGuiSurface->RestrictPaintToSinglePanel(NULL);
	GameUI().AllowEngineHideGameUI();
}

void EgoQueryBox::OnKeyCodeTyped(vgui::KeyCode code)
{
	if (code == KEY_ESCAPE)
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodeTyped(code);
	}
}

void EgoQueryBox::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == KEY_XBUTTON_B /*|| code == 172 */)
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodePressed(code);
	}
}