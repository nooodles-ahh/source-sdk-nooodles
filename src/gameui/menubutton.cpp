#include "menubutton.h"
#include "tier1/KeyValues.h"
#include "vgui/ISurface.h"

using namespace vgui;

DECLARE_BUILD_FACTORY( CMenuButton );

#define COLOR_WHITE Color(255, 255, 255, 255)

CMenuButton::CMenuButton( Panel *parent, const char *name ) : Button( parent, name, "Freeman!" )
{
	SetParent( parent );
	SetButtonBorderEnabled( false );
	
	SetArmedSound( "ui/buttonrollover.wav" );
	SetDepressedSound( "ui/buttonclick.wav" );
	SetReleasedSound( "ui/buttonclickrelease.wav" );
	SetSize( 100, 50 );
	m_bIsHovered = false;
	m_HighlightSide = SIDE_LEFT;

	SetContentAlignment( Label::a_west );
}

void CMenuButton::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetProportional( true );

	SetFont( pScheme->GetFont( "MenuButton.Text.Font", true ) );

	const char *resourceString = pScheme->GetResourceString( "MenuButton.Highlight.Size.Max" );
	if ( resourceString )
	{
		int highlightSize = atoi( resourceString );
		highlightSize = vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), highlightSize );
		m_highlightSizeMax = highlightSize;
	}

	resourceString = pScheme->GetResourceString( "MenuButton.Highlight.Size.Min" );
	if ( resourceString )
	{
		int highlightSize = atoi( resourceString );
		highlightSize = vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), highlightSize );
		m_highlightSizeMin = highlightSize;
	}

	// get size
	resourceString = pScheme->GetResourceString( "MenuButton.Wide" );
	if ( resourceString )
	{
		int wide = atoi( resourceString );
		wide = vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), wide );
		SetWide( wide );
	}

	resourceString = pScheme->GetResourceString( "MenuButton.Tall" );
	if ( resourceString )
	{
		int tall = atoi( resourceString );
		tall = vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), tall );
		SetTall( tall );
	}

	resourceString = pScheme->GetResourceString( "MenuButton.Inset.X" );
	if ( resourceString )
	{
		int xinset = atoi( resourceString );
		xinset = vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), xinset );

		int x, y;
		GetTextInset( &x, &y );
		SetTextInset( xinset, y );
	}

	m_highlightSize = m_highlightSizeMin;

	SetDefaultColor( pScheme->GetColor("MenuButton.FgColor", COLOR_WHITE ), 
		pScheme->GetColor( "MenuButton.BgColor", Color() ) );
	SetArmedColor( pScheme->GetColor( "MenuButton.FgColor.Armed", COLOR_WHITE ),
		pScheme->GetColor( "MenuButton.BgColor.Armed", Color() ) );
	SetDepressedColor( pScheme->GetColor( "MenuButton.FgColor.Depressed", COLOR_WHITE ), 
		pScheme->GetColor( "MenuButton.BgColor.Depressed", Color() ) );
	SetSelectedColor( pScheme->GetColor( "MenuButton.FgColor.Selected", COLOR_WHITE ),
		pScheme->GetColor( "MenuButton.BgColor.Selected", Color() ) );

	m_colBgColor = GetButtonDefaultBgColor();

	m_hightlightColor = pScheme->GetColor( "MenuButton.Highlight.Color", COLOR_WHITE );
}

void CMenuButton::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );
	m_HighlightSide = (sides_e)inResourceData->GetInt( "highlight_side", 0 );
}


void CMenuButton::SetEnabled( bool state )
{
	BaseClass::SetEnabled( state );

	if ( !state )
	{
		m_bIsHovered = false;
	}
}

void CMenuButton::FireActionSignal()
{
	BaseClass::FireActionSignal();

	m_bIsHovered = false;
}

void CMenuButton::Paint()
{
	// draw side highlight
	switch ( m_HighlightSide )
	{
	default:
	case SIDE_LEFT:
		surface()->DrawSetColor( m_hightlightColor );
		surface()->DrawFilledRect( 0, 0, m_highlightSize, GetTall() );
		break;
	case SIDE_BOTTOM:
		surface()->DrawSetColor( m_hightlightColor );
		surface()->DrawFilledRect( 0, GetTall() - m_highlightSize, GetWide(), GetTall() );
		break;
	}
	

	if ( IsHovered() )
	{
		// HACK(wheatley): there's a frame-perfect glitch in VGUI where you rapidly move mouse over
		// the button and clicking the LMB, which causes OnCursorEntered to fire after the mouse have
		// already left the panel
		if ( !IsCursorOver() )
		{
			OnCursorExited();
			return;
		}
	}
	BaseClass::Paint();
}

void CMenuButton::PaintBackground()
{
	if( m_HighlightSide == SIDE_LEFT)
		DrawBoxFade( 0, 0, GetWide(), GetTall(), GetBgColor(), 1.0f, 255, 0, true );
	else
		DrawBoxFade( 0, 0, GetWide(), GetTall(), GetBgColor(), 1.0f, 0, 255, false );
}

//-----------------------------------------------------------------------------
// Purpose: Perform graphical layout of button.
//-----------------------------------------------------------------------------
void CMenuButton::PerformLayout()
{
	BaseClass::PerformLayout();
	
	GetAnimationController()->CancelAnimationsForPanel( this );
	if ( IsDepressed() )
	{
		GetAnimationController()->RunAnimationCommand( this, "BackgroundColor",
			GetButtonDepressedBgColor(),
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
	}
	else if ( IsArmed() )
	{
		GetAnimationController()->RunAnimationCommand( this, "HighlightSize",
			m_highlightSizeMax,
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
		GetAnimationController()->RunAnimationCommand( this, "BackgroundColor",
			GetButtonArmedBgColor(),
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
	}
	else if ( IsSelected() )
	{
		GetAnimationController()->RunAnimationCommand( this, "HighlightSize",
			m_highlightSizeMax,
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
		GetAnimationController()->RunAnimationCommand( this, "BackgroundColor",
			GetButtonSelectedBgColor(),
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
	}
	else
	{
		GetAnimationController()->RunAnimationCommand( this, "HighlightSize",
			m_highlightSizeMin,
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
		GetAnimationController()->RunAnimationCommand( this, "BackgroundColor",
			GetButtonDefaultBgColor(),
			0.0f, 0.1f, AnimationController::INTERPOLATOR_LINEAR );
	}

}

void CMenuButton::OnKeyCodePressed( KeyCode code )
{
	BaseClass::OnKeyCodePressed( code );
}

void CMenuButton::OnMousePressed( MouseCode code )
{
	BaseClass::OnMousePressed( code );
	ForceDepressed( true );
}

void CMenuButton::OnMouseReleased( MouseCode code )
{
	BaseClass::OnMouseReleased( code );
	ForceDepressed( false );
}