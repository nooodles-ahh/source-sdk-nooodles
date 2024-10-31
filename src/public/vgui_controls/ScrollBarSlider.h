//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef SCROLLBARSLIDER_H
#define SCROLLBARSLIDER_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/Panel.h>

namespace vgui
{

class IBorder;

//-----------------------------------------------------------------------------
// Purpose: ScrollBarSlider bar, as used in ScrollBar's
//-----------------------------------------------------------------------------
class ScrollBarSlider : public Panel
{
	DECLARE_CLASS_SIMPLE( ScrollBarSlider, Panel );

public:
	ScrollBarSlider(Panel *parent, const char *panelName, bool vertical);

	// Set the ScrollBarSlider value of the nob.
	virtual void SetValue(int value);
	virtual int  GetValue();
#ifdef VGUI_ENHANCEMENTS
	virtual int GetDesiredValue();
	void SetSmoothScrolling(bool bSmoothScrolling)
	{
		_bSmoothScrolling = bSmoothScrolling;
	}
#endif

	// Check whether the scroll bar is vertical or not
	virtual bool IsVertical();

	// Set max and min range of lines to display
    virtual void SetRange(int min, int max);	
	
	virtual void GetRange(int &min, int &max);

	// Set number of rows that can be displayed in window
	virtual void SetRangeWindow(int rangeWindow); 

	// Get number of rows that can be displayed in window
	virtual int GetRangeWindow(); 

	// Set the size of the ScrollBarSlider nob
	virtual void SetSize(int wide, int tall);

	// Get current ScrollBarSlider bounds
	virtual void GetNobPos(int &min, int &max);	

	virtual bool HasFullRange();
	virtual void SetButtonOffset(int buttonOffset);
	virtual void OnCursorMoved(int x, int y);
	virtual void OnMousePressed(MouseCode code);
	virtual void OnMouseDoublePressed(MouseCode code);
	virtual void OnMouseReleased(MouseCode code);

	// Return true if this slider is actually drawing itself
	virtual bool IsSliderVisible( void );

	virtual void ApplySettings( KeyValues *pInResourceData );

protected:
	virtual void Paint();
	virtual void PaintBackground();
	virtual void PerformLayout();
	virtual void ApplySchemeSettings(IScheme *pScheme);
#ifdef VGUI_ENHANCEMENTS
	virtual void OnThink();
	virtual void PaintBorder();
#endif

private:
	virtual void RecomputeNobPosFromValue();
	virtual void RecomputeValueFromNobPos();
	virtual void SendScrollBarSliderMovedMessage();

	bool _vertical;
	bool _dragging;
	int _nobPos[2];
	int _nobDragStartPos[2];
	int _dragStartPos[2];
	int _range[2];
#ifndef VGUI_ENHANCEMENTS
	int _value;		// the position of the ScrollBarSlider, in coordinates as specified by SetRange/SetRangeWindow
#endif
	int _rangeWindow;
	int _buttonOffset;
#ifndef VGUI_ENHANCEMENTS
	IBorder *_ScrollBarSliderBorder;
#else // VGUI_ENHANCEMENTS
	int _desiredValue;
	CPanelAnimationVar( float, _value, "_value", "0" );
	bool	_bSmoothScrolling;
#endif
};

} // namespace vgui

#endif // SCROLLBARSLIDER_H
