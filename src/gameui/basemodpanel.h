//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef _BASEMODFACTORYBASEPANEL_H__
#define _BASEMODFACTORYBASEPANEL_H__

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/Button.h"
#include "tier1/utllinkedlist.h"
#include "materialsystem/imaterial.h"
#include "vgui_controls/Label.h"
#include "vgui/ISurface.h"
#include "tier1/utldict.h"
#include "gamemenu.h"

class CLogoPanel;

// must supply some non-trivial time to let the movie startup smoothly
// the attract screen also uses this so it doesn't pop in either
#define TRANSITION_TO_MOVIE_DELAY_TIME	0.5f	// how long to wait before starting the fade
#define TRANSITION_TO_MOVIE_FADE_TIME	1.2f	// how fast to fade

class IVTFTexture;

using namespace vgui;

namespace BaseModUI 
{
	//=============================================================================
	//
	//=============================================================================
	class CBaseModPanel : public vgui::Panel
	{
		DECLARE_CLASS_SIMPLE( CBaseModPanel, vgui::Panel );

	public:
		CBaseModPanel();
		~CBaseModPanel();

	public:
		static CBaseModPanel& GetSingleton();
		static CBaseModPanel* GetSingletonPtr();

		void ReloadScheme();
		bool IsLevelLoading();

		void OnGameUIActivated();
		void OnGameUIHidden();
		void RunFrame();
		void OnLevelLoadingStarted( char const *levelName, bool bShowProgressDialog );
		void OnLevelLoadingFinished( bool bError, const char* failureReason, const char* extendedReason );
		bool UpdateProgressBar(float progress, const char *statusText);

		bool IsReadyToWriteConfig( void );
	
		virtual void OnCommand( const char *command );

		virtual void OnKeyCodePressed(KeyCode code);

		virtual Color GetBgColor()
		{
			return Color( 0, 0, 0, 160 );
		}

		void SetMainMenuOverride( vgui::VPANEL panel );

	protected:
		CBaseModPanel(const CBaseModPanel&);
		CBaseModPanel& operator=(const CBaseModPanel&);

		virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
		virtual void PerformLayout();
		bool ShouldDarkenPanel(vgui::VPANEL panel);
		virtual void PaintBackground();
		virtual void OnSetFocus();

		MESSAGE_FUNC( OnMovedPopupToFront, "OnMovedPopupToFront" );

	private:
		void OnEngineLevelLoadingSession( KeyValues *pEvent );
		
		void CreateMenus(void);
		void CreateGameMenu( void );

		static CBaseModPanel* m_CFactoryBasePanel;
		
		bool m_LevelLoading;
		vgui::HScheme m_UIScheme;

		int	m_iBackgroundImageID;
		int m_DelayActivation;

		bool m_bInGame;

		CGameMenu *m_pGameMenu;
		vgui::DHANDLE<vgui::Frame> m_hLoadingDialog;

		bool m_bNeedsDarkening;
		bool m_bHasDarkened;
		CPanelAnimationVar(float, m_flBackgroundFade, "BackgroundFade", "0");

		int m_iMainMenuX;
		int m_iMainMenuY;
		int m_iMainMenuBottomMargin;
		CUtlVector<Vector2D> m_vecMenuTitlePos;

	};
};

#endif
