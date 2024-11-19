//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
// This class is a message box that has two buttons, ok and cancel instead of
// just the ok button of a message box. We use a message box class for the ok button
// and implement another button here.
//
// $NoKeywords: $
//=============================================================================//

#include <vgui/KeyCode.h>

#include <vgui_controls/QueryBox.h>
#include <vgui_controls/TextImage.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
QueryBox::QueryBox(const char *title, const char *queryText, vgui::Panel *parent) : MessageBox(title, queryText,parent)
{
	Init();
}

QueryBox::QueryBox(const wchar_t *wszTitle, const wchar_t *wszQueryText,vgui::Panel *parent) : MessageBox(wszTitle, wszQueryText,parent)
{
	Init();
}

#ifdef VGUI_ENHANCEMENTS
void QueryBox::Init()
{
	SetDeleteSelfOnClose(true);
	m_pCancelButton->SetText("#QueryBox_Cancel");
	m_pNoButton = nullptr;

	m_pCancelCommand = nullptr;
	m_pOkCommand = nullptr;
	m_pNoCommand = nullptr;

	m_pOkButton->SetTabPosition(1);
	m_pCancelButton->SetTabPosition(2);
	SetCancelButtonVisible(true);
}
#endif

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
QueryBox::~QueryBox()
{
	delete m_pCancelButton;

	if ( m_pOkCommand )
	{
		m_pOkCommand->deleteThis();
	}
	if ( m_pCancelCommand )
	{
		m_pCancelCommand->deleteThis();
	}
#ifdef VGUI_ENHANCEMENTS
	if (m_pNoCommand)
	{
		m_pNoCommand->deleteThis();
	}
	if (m_pNoButton)
	{
		delete m_pNoButton;
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Layout the window for drawing 
//-----------------------------------------------------------------------------
void QueryBox::PerformLayout()
{
	BaseClass::PerformLayout();

#if 1
	int boxWidth, boxTall;
	GetSize(boxWidth, boxTall);

	int x, y, wide, tall;
	GetClientArea(x, y, wide, tall);
	wide += x;
	tall += y;

	int oldWide, oldTall;
	m_pCancelButton->GetSize(oldWide, oldTall);
	
	int btnWide, btnTall;
	int padding = PROPORTIONAL_VALUE( 10 );
	m_pCancelButton->GetContentSize(btnWide, btnTall);
	btnWide = max(oldWide, btnWide + padding );
	btnTall = max(oldTall, btnTall + padding );
	m_pCancelButton->SetSize(btnWide, btnTall);

#ifdef VGUI_ENHANCEMENTS
	m_pOkButton->GetContentSize( btnWide, btnTall );
	btnWide = max( oldWide, btnWide + padding );
	btnTall = max( oldTall, btnTall + padding );
	m_pOkButton->SetSize( btnWide, btnTall );


	if (m_pNoButton)
	{
		m_pNoButton->GetContentSize(btnWide, btnTall);
		btnWide = max(oldWide, btnWide + padding);
		btnTall = max(oldTall, btnTall + padding);
		m_pNoButton->SetSize(btnWide, btnTall);
	}

#endif

	GetSize(boxWidth, boxTall);

	padding = PROPORTIONAL_VALUE( 15 );

	if (m_pNoButton)
	{
		int totalButtonWidth = m_pOkButton->GetWide() + m_pNoButton->GetWide() + m_pCancelButton->GetWide() + padding * 2;

		float start = (boxWidth - totalButtonWidth )/ 2.f;

		// distribute the buttons evenly
		m_pOkButton->SetPos(start, tall - m_pOkButton->GetTall() - padding);
		m_pNoButton->SetPos(m_pOkButton->GetXPos() + m_pOkButton->GetWide() + padding, tall - m_pNoButton->GetTall() - padding);
		m_pCancelButton->SetPos(m_pNoButton->GetXPos() + m_pNoButton->GetWide() + padding, tall - m_pCancelButton->GetTall() - padding);

	}
	else
	{
		m_pOkButton->SetPos((wide / 2) - (m_pOkButton->GetWide()) - 1 + x, tall - m_pOkButton->GetTall() - padding);
		m_pCancelButton->SetPos((wide / 2) + x + PROPORTIONAL_VALUE(16), tall - m_pCancelButton->GetTall() - padding);
	}


#endif

}

//-----------------------------------------------------------------------------
// Purpose: Handles command text from the buttons
//			Deletes self when closed
//-----------------------------------------------------------------------------
void QueryBox::OnCommand(const char *command)
{
	if (!stricmp(command, "OK"))
	{
		OnCommand("Close");

		if ( m_pOkCommand )
		{
			PostActionSignal(m_pOkCommand->MakeCopy());
		}
	}
	else if (!stricmp(command, "No"))
	{
		OnCommand("Close");

		if (m_pNoCommand)
		{
			PostActionSignal(m_pNoCommand->MakeCopy());
		}
	}
	else if (!stricmp(command, "Cancel"))
	{
		OnCommand("Close");	

		if (m_pCancelCommand)
		{
			PostActionSignal(m_pCancelCommand->MakeCopy());
		}
	}
	
	BaseClass::OnCommand(command);
	
}

//-----------------------------------------------------------------------------
// Purpose: Set the keyvalues to send when ok button is hit
//-----------------------------------------------------------------------------
void QueryBox::SetOKCommand(KeyValues *keyValues)
{
	if ( m_pOkCommand )
	{
		m_pOkCommand->deleteThis();
	}

	m_pOkCommand = keyValues;
}

//-----------------------------------------------------------------------------
// Purpose: Set a value of the ok command
//-----------------------------------------------------------------------------
void QueryBox::SetOKCommandValue(const char *keyName, int value)
{
	if ( !m_pOkCommand )
	{
		m_pOkCommand = new KeyValues("Command");
	}

	m_pOkCommand->SetInt(keyName, value);
}

#ifdef VGUI_ENHANCEMENTS
void QueryBox::SetYesNoCancel(bool state)
{
	if (state)
	{
		if (!m_pNoButton)
		{
			m_pNoButton = new Button(this, NULL, "#QueryBox_No");
			m_pNoButton->SetCommand("No");
			m_pNoButton->AddActionSignalTarget(this);
			m_pNoButton->SetTabPosition(3);
		}
	}
	else
	{
		if (m_pNoButton)
		{
			delete m_pNoButton;
			m_pNoButton = nullptr;
		}
	}
}

void QueryBox::SetNoButtonText(const char* buttonText)
{
	if (!m_pNoButton)
		return;
	m_pNoButton->SetText(buttonText);
	InvalidateLayout();
}

void QueryBox::SetNoButtonText(const wchar_t* wszButtonText)
{
	if (!m_pNoButton)
		return;
	m_pNoButton->SetText(wszButtonText);
	InvalidateLayout();
}

void QueryBox::SetNoCommand(KeyValues* keyValues)
{
	if (!m_pNoButton)
		return;

	if (m_pNoCommand)
	{
		m_pNoCommand->deleteThis();
	}
	m_pNoCommand = keyValues;
}

void QueryBox::SetNoCommandValue(const char* keyName, int value)
{
	if (!m_pNoButton)
		return;

	if (!m_pNoCommand)
	{
		m_pNoCommand = new KeyValues("Command");
	}

	m_pNoCommand->SetInt(keyName, value);
}
#endif

//-----------------------------------------------------------------------------
// Purpose: Set the keyvalues to send when the cancel button is hit
//-----------------------------------------------------------------------------
void QueryBox::SetCancelCommand(KeyValues *keyValues)
{
	if ( m_pCancelCommand )
	{
		m_pCancelCommand->deleteThis();
	}

	m_pCancelCommand = keyValues;
}

//-----------------------------------------------------------------------------
// Purpose: Sets the cancel button text
//-----------------------------------------------------------------------------
void QueryBox::SetCancelButtonText(const char* buttonText)
{
	m_pCancelButton->SetText(buttonText);
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the cancel button text
//-----------------------------------------------------------------------------
void QueryBox::SetCancelButtonText(const wchar_t* wszButtonText)
{
	m_pCancelButton->SetText(wszButtonText);
	InvalidateLayout();
}

void QueryBox::OnKeyCodeTyped( KeyCode code )
{
	if ( code == KEY_ESCAPE )
	{
		OnCommand("Cancel");
	}
	else
	{
		Frame::OnKeyCodeTyped(code);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void QueryBox::OnKeyCodePressed( KeyCode code )
{
	if ( code == KEY_XBUTTON_B )
	{
		OnCommand("Cancel");
	}
	else
	{
		Frame::OnKeyCodePressed(code);
	}
}



