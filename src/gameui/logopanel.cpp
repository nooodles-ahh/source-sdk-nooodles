#include "logopanel.h"

// Logo
CLogoPanel::CLogoPanel( vgui::Panel *parent ) : BaseClass( parent, "GameLogo" )
{
	SetParent( parent );
	SetProportional( true );
	SetVisible( true );
	SetDragEnabled( false );
	SetShowDragHelper( false );
	SetPaintBackgroundEnabled( false );
}

CLogoPanel::~CLogoPanel()
{
}

//-----------------------------------------------------------------------------
// Purpose: Applies scheme settings
//-----------------------------------------------------------------------------
void CLogoPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	LoadControlSettings( "resource/gamelogo.res", NULL, NULL );
	SetProportional( true );
}