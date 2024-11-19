//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "optionsdialog.h"

#include "vgui_controls/Button.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/QueryBox.h"

#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"
#include "vgui/IInput.h"

#include "KeyValues.h"
#include "optionssubkeyboard.h"
#include "optionssubmouse.h"
#include "optionssubaudio.h"
#include "optionssubvideo.h"
#include "optionssubvoice.h"
#include "optionssubmultiplayer.h"
#include "optionssubdifficulty.h"
#include "modinfo.h"
#include "gameui_util.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_GAMEUI_DIALOG_FACTORY(OpenOptionsDialog, COptionsDialog)

//-----------------------------------------------------------------------------
// Purpose: Basic help dialog
//-----------------------------------------------------------------------------
COptionsDialog::COptionsDialog(vgui::Panel *parent) : PropertyDialog(parent, "OptionsDialog")
{
	SetScheme(parent->GetScheme());
	SetDeleteSelfOnClose( true );
	SetBounds(
		0,
		0,
		PROPORTIONAL_VALUE(512),
		PROPORTIONAL_VALUE(415) 
	);
	SetSizeable( false );

	SetTitle("#GameUI_Options", true);

	if ( ModInfo().IsSinglePlayerOnly() && !ModInfo().NoDifficulty() )
	{
		AddPage(new COptionsSubDifficulty(this), "#GameUI_Difficulty");
	}

	AddPage( new COptionsSubKeyboard( this ), "#GameUI_Keyboard");
	AddPage( new COptionsSubMouse( this ), "#GameUI_Mouse");

	m_pOptionsSubAudio = new COptionsSubAudio(this);
	AddPage(m_pOptionsSubAudio, "#GameUI_Audio");

	m_pOptionsSubVideo = new COptionsSubVideo(this);
	AddPage(m_pOptionsSubVideo, "#GameUI_Video");

	if ( !ModInfo().IsSinglePlayerOnly() ) 
	{
		AddPage(new COptionsSubVoice(this), "#GameUI_Voice");
	}

	// add the multiplay page last, if we're combo single/multi or just multi
	//if ( (ModInfo().IsMultiplayerOnly() && !ModInfo().IsSinglePlayerOnly()) ||
	//	 (!ModInfo().IsMultiplayerOnly() && !ModInfo().IsSinglePlayerOnly()) )
	//{
	//	AddPage( new COptionsSubMultiplayer( this ), "#GameUI_Multiplayer");
	//}

	SetApplyButtonVisible(true);
	GetPropertySheet()->SetTabWidth(84);
	MoveToCenterOfScreen();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
COptionsDialog::~COptionsDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: Brings the dialog to the fore
//-----------------------------------------------------------------------------
void COptionsDialog::Activate()
{
	BaseClass::Activate();
	EnableApplyButton(false);
}

//-----------------------------------------------------------------------------
// Purpose: Opens the dialog
//-----------------------------------------------------------------------------
void COptionsDialog::Run()
{
	SetTitle("#GameUI_Options", true);
	Activate();
}

//-----------------------------------------------------------------------------
// Purpose: Opens the gamma dialog directly
//-----------------------------------------------------------------------------
void COptionsDialog::OpenGammaDialog()
{
	m_pOptionsSubVideo->OpenGammaDialog();
}

void COptionsDialog::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetSize(
		PROPORTIONAL_VALUE( 512 ),
		PROPORTIONAL_VALUE( 415 ) 
	);
	SetSizeable( false );
	GetPropertySheet()->SetTabWidth( 84 );

	// last page is visible when the pages are invalidated
	for ( int i = 0; i < GetPropertySheet()->GetNumPages(); i++ )
	{
		Panel* page = GetPropertySheet()->GetPage( i );
		if ( page != GetPropertySheet()->GetActivePage() )
		{
			page->InvalidateLayout();
			page->SetVisible( false );
		}
	}
	MoveToCenterOfScreen();
}

void COptionsDialog::OnKeyCodeTyped( KeyCode code )
{
	// force ourselves to be closed if the escape key it pressed
	if ( code == KEY_ESCAPE )
	{
		Close();
	}
	else
	{
		BaseClass::OnKeyCodeTyped( code );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when the GameUI is hidden
//-----------------------------------------------------------------------------
void COptionsDialog::OnGameUIHidden()
{
	// tell our children about it
	for ( int i = 0 ; i < GetChildCount() ; i++ )
	{
		Panel *pChild = GetChild( i );
		if ( pChild )
		{
			PostMessage( pChild, new KeyValues( "GameUIHidden" ) );
		}
	}
}
