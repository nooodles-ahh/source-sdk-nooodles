#include "serverbrowserdialog.h"
#include <KeyValues.h>
#include "vgui/ILocalize.h"
#include <vgui/IInput.h>
#include "tier0/valve_minmax_off.h"
#include <functional>
#include "tier0/valve_minmax_on.h"
#include "serverlistpage.h"

using namespace vgui;

//-----------------------------------------------------------------------------
CServerBrowserDialog::CServerBrowserDialog( vgui::Panel* parent ) : BaseClass( parent, "CServerBrowserDialog")
{
	//SetProportional( true );
	SetSize(
		PROPORTIONAL_VALUE( 624 ),
		PROPORTIONAL_VALUE( 384 )
	);

	g_pVGuiLocalize->AddFile( "servers/serverbrowser_%language%.txt", "PLATFORM", true );
	SetSizeable( true );
	SetDeleteSelfOnClose( true );
	MoveToCenterOfScreen();

	AddPage( new CInternetGames( this ), "#ServerBrowser_InternetTab" );
}

CServerBrowserDialog::~CServerBrowserDialog()
{
}

void CServerBrowserDialog::Activate()
{
	BaseClass::Activate();
	vgui::input()->SetMouseCapture( GetVPanel() );
}

void CServerBrowserDialog::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CServerBrowserDialog::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	LoadControlSettings( "servers/dialogserverbrowser.res" );
}

void CServerBrowserDialog::OnCommand( const char* command )
{
	if ( !Q_stricmp( command, "Close" ) )
	{
		Close();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void CServerBrowserDialog::OnKeyCodeTyped( vgui::KeyCode code )
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