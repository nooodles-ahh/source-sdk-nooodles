#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "vgui_controls/Menu.h"

class CGameMenu : public vgui::Menu
{
    DECLARE_CLASS_SIMPLE( CGameMenu, vgui::Menu );
public:
    CGameMenu(Panel *parent, const char *panelName);
    virtual void SetVisible(bool state);
    virtual void LayoutMenuBorder() {};
    void UpdateVisibleItems(bool inLevel, bool isMultiplayer, bool isInReplay, bool isVREnabled, bool isVRActive);
    virtual int AddMenuItem( const char* itemName, const char* itemText, const char* command, vgui::Panel* target, const KeyValues* userData );
    virtual void OnKillFocus();
    virtual void OnSetFocus();
    void SetMainMenuOverride( vgui::VPANEL panel );
    virtual void	OnKeyCodePressed( vgui::KeyCode code );

    vgui::VPANEL GetMainMenuOverride() { return m_hMainMenuOverride; }

private:
    vgui::VPANEL m_hMainMenuOverride;

};
#endif