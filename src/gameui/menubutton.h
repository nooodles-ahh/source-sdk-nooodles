#ifndef MODBUTTON_H
#define MODBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/Button.h"
#include "vgui_controls/AnimationController.h"

using namespace vgui;

class CMenuButton : public vgui::Button
{
	DECLARE_CLASS_SIMPLE( CMenuButton, vgui::Button );
public:
	CMenuButton( vgui::Panel *parent, const char *name );

	void			SetEnabled( bool state );
	virtual void	FireActionSignal();
	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void PerformLayout();

	virtual void OnKeyCodePressed( KeyCode code );
	virtual void OnMousePressed( MouseCode code );
	virtual void OnMouseReleased( MouseCode code );


protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Paint();
	virtual void PaintBackground();

	virtual Color GetBgColor() {
		return m_colBgColor;
	}

	virtual void OnCursorEntered()
	{
		m_bIsHovered = true;
		BaseClass::OnCursorEntered();
	}

	virtual void OnCursorExited()
	{
		m_bIsHovered = false;
		ForceDepressed( false );
		BaseClass::OnCursorExited();
	}


	bool IsHovered()
	{
		return m_bIsHovered;
	}

	enum sides_e
	{
		SIDE_LEFT = 0,
		SIDE_TOP = 1,
		SIDE_RIGHT = 2,
		SIDE_BOTTOM = 3
	};

private:
	int			m_iGradient;
	bool		m_bIsHovered;
	sides_e	m_HighlightSide;

	float m_highlightSizeMin;
	float m_highlightSizeMax;

	CPanelAnimationVar( Color, m_colBgColor, "BackgroundColor", "255 255 255 255" );
	CPanelAnimationVar( Color, m_hightlightColor, "HighlightColor", "255 255 255 255" );
	CPanelAnimationVar( float, m_highlightSize, "HighlightSize", "1" );

	// ==============================
	


};

#endif