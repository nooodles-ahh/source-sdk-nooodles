#include "engineinterface.h"
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

	m_iOffsetX = 0;
	m_iOffsetY = 0;
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
	KeyValuesAD pConditions("conditions");

	char szBackgroundName[MAX_PATH];
	engine->GetMainMenuBackgroundName(szBackgroundName, sizeof(szBackgroundName));
	pConditions->AddSubKey(new KeyValues(szBackgroundName));

	LoadControlSettings( "resource/gamelogo.res", NULL, NULL, pConditions );
}

void CLogoPanel::ApplySettings(KeyValues* inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_iOffsetX = inResourceData->GetInt("offset_x", 0);
	m_iOffsetY = inResourceData->GetInt("offset_y", 0);
}
