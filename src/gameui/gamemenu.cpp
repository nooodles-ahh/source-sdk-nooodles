#include "vgui_controls/Menu.h"
#include "vgui_controls/MenuItem.h"
#include "KeyValues.h"
#include "gamemenu.h"
#include <vgui/ISurface.h>

using namespace vgui;

class CGameMenuItem : public MenuItem
{
	DECLARE_CLASS_SIMPLE(CGameMenuItem, MenuItem);
public:
    CGameMenuItem( Menu* parent, const char* panelName ) : BaseClass( parent, panelName, "GameMenuItem", nullptr, false )
    {
	}

    virtual void ApplySchemeSettings( IScheme* pScheme )
    {
		BaseClass::ApplySchemeSettings(pScheme);

		SetContentAlignment(Label::a_west);
		SetArmedSound("UI/buttonrollover.wav");
		SetDepressedSound("UI/buttonclick.wav");
		SetReleasedSound("UI/buttonclickrelease.wav");

        SetDefaultColor( pScheme->GetColor( "MainMenu.TextColor", Color( 0, 0, 0, 0 ) ), Color( 0, 0, 0, 0 ) );
        SetArmedColor( pScheme->GetColor( "MainMenu.ArmedTextColor", Color( 0, 0, 0, 0 ) ), Color( 0, 0, 0, 0 ) );
        SetDepressedColor( pScheme->GetColor( "MainMenu.DepressedTextColor", Color( 0, 0, 0, 0 ) ), Color( 0, 0, 0, 0 ) );
        SetSelectedColor(pScheme->GetColor("MainMenu.TextColor", Color(0, 0, 0, 0) ), Color(0, 0, 0, 0) );

        HFont hFont = pScheme->GetFont( "MainMenuFont", IsProportional() );
        if ( hFont != INVALID_FONT )
        {
			SetFont( hFont );
		}
        else
        {
            SetFont( pScheme->GetFont( "MenuLarge", IsProportional() ) );
        }
	}
};

CGameMenu::CGameMenu(Panel *parent, const char *panelName) : BaseClass(parent, panelName)
{
    SetPaintBackgroundEnabled(false);
    m_hMainMenuOverride = NULL;
}

void CGameMenu::SetVisible( bool state )
{
    BaseClass::SetVisible( true );

    if ( !state )
    {
        g_pVGuiPanel->MoveToBack( GetVPanel() );
    }

    if ( m_hMainMenuOverride )
    {
        g_pVGuiPanel->SetVisible( m_hMainMenuOverride, true );
        if ( !state )
        {
            g_pVGuiPanel->MoveToBack( m_hMainMenuOverride );
        }
    }
}

void CGameMenu::UpdateVisibleItems(bool inLevel, bool isMultiplayer, bool isInReplay, bool isVREnabled, bool isVRActive)
{
    int childCount = GetChildCount();
    for(int i = 0; i < childCount; ++i)
    {
        MenuItem *item = dynamic_cast<MenuItem *>(GetChild(i));
        if(!item)
            continue;

        KeyValues *userData = item->GetUserData();
        if(!userData)
            continue;

        bool bIsVisible = true;
        if( !inLevel && userData->GetBool("OnlyInGame") )
        {
            bIsVisible = false;
        }
        if( !isInReplay && userData->GetBool("OnlyInReplay") )
        {
            bIsVisible = false;
        }
        else if( !isVREnabled && userData->GetBool("OnlyWhenVREnabled") )
        {
            bIsVisible = false;
        }
        else if( !isVRActive && userData->GetBool("OnlyWhenVRActive") )
        {
            bIsVisible = false;
        }
        else if( isVRActive && userData->GetBool("OnlyWhenVRInactive") )
        {
            bIsVisible = false;
        }
        else if( isMultiplayer && userData->GetBool("notmulti") )
        {
            bIsVisible = false;
        }
        else if( (!isMultiplayer && inLevel) && userData->GetBool("notsingle") )
        {
            bIsVisible = false;
        }
        else if (userData->GetBool("ConsoleOnly"))
        {
            bIsVisible = false;
        }

        item->SetVisible(bIsVisible);
        item->SetPaintBackgroundEnabled(false);
    }

    // TODO "InGameOrder"

    InvalidateLayout();
}

int CGameMenu::AddMenuItem( const char* itemName, const char* itemText, const char* command, Panel* target, const KeyValues* userData )
{
    MenuItem* item = new CGameMenuItem( this, itemName );
    item->SetText( itemText );
    return AddMenuItemCharCommand( item, command, target, userData );
}

void CGameMenu::OnKillFocus()
{
	BaseClass::OnKillFocus();
}

void CGameMenu::OnSetFocus()
{
    if ( m_hMainMenuOverride )
    {
        Panel* pMainMenu = g_pVGuiPanel->GetPanel( m_hMainMenuOverride, "ClientDLL" );
        if ( pMainMenu )
        {
            pMainMenu->PerformLayout(); // 0x218
        }
    }


    BaseClass::OnSetFocus();
}

void CGameMenu::OnKeyCodePressed( vgui::KeyCode code )
{
    if ( m_hMainMenuOverride )
    {
        PostMessage( m_hMainMenuOverride, new KeyValues( "KeyCodePressed", "code", code ) );
	}
    else
    {
		BaseClass::OnKeyCodePressed( code );
	}
}

void CGameMenu::SetMainMenuOverride( vgui::VPANEL panel )
{
    m_hMainMenuOverride = panel;
}