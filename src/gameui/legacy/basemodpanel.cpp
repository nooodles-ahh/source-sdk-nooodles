//========= Copyright  1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "basemodpanel.h"
#include "./GameUI/IGameUI.h"
#include "ienginevgui.h"
#include "engine/IEngineSound.h"
#include "engineinterface.h"
#include "tier0/dbg.h"
#include "gameui_interface.h"
#include "game/client/IGameClientExports.h"
#include "GameUI/IGameConsole.h"
#include "inputsystem/iinputsystem.h"
#include "filesystem.h"
#include "tier2/renderutils.h"
#include "gameconsole.h"
#include "vgui/ISystem.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "vgui/IInput.h"
#include "vgui_controls/AnimationController.h"
#include "gameui_util.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/materialsystem_config.h"
#include "materialsystem/imesh.h"
#include "tier0/icommandline.h"
#include "fmtstr.h"
#include "smartptr.h"
//#include "nb_header_footer.h"
#include "utlbuffer.h"
#include "UtlSortVector.h"
#include "steam/steam_api.h"
#include "language.h"
#include "vguisystemmoduleloader.h"

#include "loadingdialog.h"
#include "engineinterface.h"
#include "dialogs/quitdialog.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


using namespace BaseModUI;
using namespace vgui;

extern const char *COM_GetModDirectory( void );

void CC_gamemenucommand( const CCommand &args )
{
	if ( args.ArgC() < 2 )
	{
		Msg( "Format: gamemenucommand <command>\n" );
		return;
	}
	
	CBaseModPanel::GetSingleton().OnCommand( args[1] );
}

//=============================================================================
CBaseModPanel* CBaseModPanel::m_CFactoryBasePanel = 0;

#ifndef _CERT
ConVar ui_gameui_debug( "ui_gameui_debug", "0", FCVAR_NONE );
int UI_IsDebug()
{
	return (*(int *)(&ui_gameui_debug)) ? ui_gameui_debug.GetInt() : 0;
}
#endif

//=============================================================================
CBaseModPanel::CBaseModPanel(): BaseClass(0, "CBaseModPanel")
{
	static ConCommand gamemenucommand( "gamemenucommand", CC_gamemenucommand, "TODO\n", FCVAR_NONE );

	//MakePopup( false );

	Assert(m_CFactoryBasePanel == 0);
	m_CFactoryBasePanel = this;

	m_LevelLoading = false;
	
	// delay 3 frames before doing activation on initialization
	// needed to allow engine to exec startup commands (background map signal is 1 frame behind) 
	m_DelayActivation = 3;
	
	m_UIScheme = vgui::scheme()->LoadSchemeFromFileEx( 0, "resource/sourcescheme.res", "SourceScheme" );
	SetScheme( m_UIScheme );

	int wide, tall;
	vgui::surface()->GetScreenSize( wide, tall );
	SetSize( wide, tall );

	SetProportional( gameui_proportional_scaling.GetBool() );
	m_iBackgroundImageID = -1;

	m_bInGame = false;

	m_pGameMenu = nullptr;
	CreateGameMenu();

	m_bNeedsDarkening = false;
	m_bHasDarkened = false;
}

//=============================================================================
CBaseModPanel::~CBaseModPanel()
{
	Assert(m_CFactoryBasePanel == this);
	m_CFactoryBasePanel = 0;

	surface()->DestroyTextureID( m_iBackgroundImageID );
}

//=============================================================================
CBaseModPanel& CBaseModPanel::GetSingleton()
{
	Assert(m_CFactoryBasePanel != 0);
	return *m_CFactoryBasePanel;
}

//=============================================================================
CBaseModPanel* CBaseModPanel::GetSingletonPtr()
{
	return m_CFactoryBasePanel;
}

//=============================================================================
void CBaseModPanel::ReloadScheme()
{
}

void CBaseModPanel::CreateMenus( void )
{
	// create the main game menu
	CreateGameMenu();
}

void CBaseModPanel::CreateGameMenu( void )
{
	m_pGameMenu = new CGameMenu( this, "GameMenu" );

	// read resource/gamemenu.res
	KeyValuesAD pKVGameMenu("GameMenu");
	if ( !pKVGameMenu->LoadFromFile( g_pFullFileSystem, "resource/gamemenu.res", "GAME" ) )
	{
		Warning( "Could not read file resource/gamemenu.res\n" );
		return;
	}

	// for each key in gamemenu.res add a button
	for ( KeyValues* pKV = pKVGameMenu->GetFirstSubKey(); pKV != NULL; pKV = pKV->GetNextKey() )
	{
		const char* buttonCommand = pKV->GetString( "command" );
		const char* buttonLabel = pKV->GetString( "label", "unknown" );
		// include user data
		m_pGameMenu->AddMenuItem( buttonLabel, buttonLabel, buttonCommand, this, pKV );
	}
}

bool CBaseModPanel::IsLevelLoading()
{
	return m_LevelLoading;
}

//=============================================================================
void CBaseModPanel::OnGameUIActivated()
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnGameUIActivated( delay = %d )\n", m_DelayActivation );
	}

	if ( m_DelayActivation )
	{
		return;
	}

	COM_TimestampedLog( "CBaseModPanel::OnGameUIActivated()" );

	bool bIsInLevel = GameUI().IsInLevel();
	bool bIsMultiplayer = false;
	bool bIsInReplay = false; // TODO: GameUI().IsInReplay();
	bool bIsVREnabled = false; // TODO
	bool bIsVRActive = false;

	if( bIsInLevel && engine->GetMaxClients() > 1 )
		bIsMultiplayer = true;
	
	if( m_pGameMenu )
		m_pGameMenu->UpdateVisibleItems(bIsInLevel, bIsMultiplayer, bIsInReplay, bIsVREnabled, bIsVRActive);

	SetVisible( true );
}

//=============================================================================
void CBaseModPanel::OnGameUIHidden()
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnGameUIHidden()\n" );
	}

	SetVisible(false);
	
	// Notify the options dialog that game UI is closing
	//if ( m_hOptionsDialog.Get() )
	//{
	//	PostMessage( m_hOptionsDialog.Get(), new KeyValues( "GameUIHidden" ) );
	//}
}

//=============================================================================
void CBaseModPanel::RunFrame()
{
	GetAnimationController()->UpdateAnimations( Plat_FloatTime() );

	if ( m_DelayActivation )
	{
		m_DelayActivation--;
		if ( !m_LevelLoading && !m_DelayActivation )
		{
			if ( UI_IsDebug() )
			{
				Msg( "[GAMEUI] Executing delayed UI activation\n");
			}
			OnGameUIActivated();
		}
	}

	if ( m_bInGame != GameUI().IsInLevel() )
	{
		m_bInGame = GameUI().IsInLevel();
	}
}


//=============================================================================
void CBaseModPanel::OnLevelLoadingStarted( char const *levelName, bool bShowProgressDialog )
{
	Assert( !m_LevelLoading );

	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] OnLevelLoadingStarted - opening loading progress (%s)...\n",
			levelName ? levelName : "<< no level specified >>" );
	}

	if (m_pGameMenu)
	{
		VPANEL overriddenMainMenu = m_pGameMenu->GetMainMenuOverride();
		if(overriddenMainMenu)
			PostMessage(overriddenMainMenu, new KeyValues("LoadingStarted"));
	}

	if ( bShowProgressDialog )
	{
		CLoadingDialog *pLoadingDialog = dynamic_cast<CLoadingDialog *>(m_hLoadingDialog.Get());
		if ( !pLoadingDialog )
		{
			pLoadingDialog = new CLoadingDialog( this );
			m_hLoadingDialog = pLoadingDialog;
		}
		
		pLoadingDialog->SetProgressPoint( 0.0f );
		pLoadingDialog->Open();
	}

	m_LevelLoading = true;
}

void CBaseModPanel::OnEngineLevelLoadingSession( KeyValues *pEvent )
{
}

//=============================================================================
void CBaseModPanel::OnLevelLoadingFinished( bool bError, const char* failureReason, const char* extendedReason )
{
	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] CBaseModPanel::OnLevelLoadingFinished( %s, %s )\n", bError ? "Had Error" : "No Error", failureReason );
	}

	if (m_pGameMenu)
	{
		VPANEL overriddenMainMenu = m_pGameMenu->GetMainMenuOverride();
		if (overriddenMainMenu)
			PostMessage(overriddenMainMenu, new KeyValues("LoadingFinished", "error", bError));
	}

	CLoadingDialog* pLoadingDialog = dynamic_cast< CLoadingDialog* >( m_hLoadingDialog.Get() );
	if ( pLoadingDialog )
	{
		if ( bError )
		{
			// turn the dialog to error display mode
			pLoadingDialog->DisplayGenericError( failureReason, extendedReason );
		}
		else
		{
			// close loading dialog
			pLoadingDialog->Close();
			pLoadingDialog = nullptr;
		}
	}

	InvalidateLayout( true );

	m_LevelLoading = false;

	if ( bError )
	{
		Warning( "%s\n", failureReason );

	}
}

//=============================================================================
bool CBaseModPanel::UpdateProgressBar( float progress, const char *statusText )
{
	if ( !m_LevelLoading )
	{
		return false;
	}

	CLoadingDialog* pLoadingDialog = dynamic_cast< CLoadingDialog* >( m_hLoadingDialog.Get() );
	if ( pLoadingDialog )
	{
		// Even if the progress hasn't advanced, we want to go ahead and refresh if it has been more than 1/10 seconds since last refresh to keep the spinny thing going.
		static float s_LastEngineTime = -1.0f;
		// clock the anim at 10hz
		float flTime = Plat_FloatTime();
		float deltaTime = flTime - s_LastEngineTime;

		//bool bUpdated = false;

		if ( ( pLoadingDialog->GetProgressPoint() < progress ) || ( deltaTime > 0.06f ) )
		{
			// update progress
			pLoadingDialog->SetProgressPoint( progress );
			s_LastEngineTime = flTime;
			//bUpdated = true;
		}

		if ( statusText && statusText[ 0 ] )
		{
			pLoadingDialog->SetStatusText( statusText );
			//bUpdated = true;
		}

		return true;
	}

	// no update required*/
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: moves the game menu button to the right place on the taskbar
//-----------------------------------------------------------------------------
static void BaseUI_PositionDialog(vgui::PHandle dlg)
{
	if (!dlg.Get())
		return;

	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds( x, y, ww, wt );
	dlg->GetSize(wide, tall);

	// Center it, keeping requested size
	dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}

#ifndef PROPORTIONAL_VALUE
#define PROPORTIONAL_VALUE(x) \
	(IsProportional() ? vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), x ) : x)
#endif

//=============================================================================
void CBaseModPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetProportional( gameui_proportional_scaling.GetBool() );

	SetBgColor(pScheme->GetColor("Blank", Color(0, 0, 0, 0)));

	// load background image
	int screenWide, screenTall;
	surface()->GetScreenSize( screenWide, screenTall );
	m_iBackgroundImageID = surface()->CreateNewTextureID();
	// depending on the aspect ratio, choose a different startup image
	if ( ( float )screenWide / ( float )screenTall >= 1.6f )
		surface()->DrawSetTextureFile( m_iBackgroundImageID, "console/background01_widescreen", true, false);
	else
		surface()->DrawSetTextureFile( m_iBackgroundImageID, "console/background01", true, false );

	if ( m_pGameMenu )
	{
		m_pGameMenu->SetPos(
			PROPORTIONAL_VALUE( 50 ),
			PROPORTIONAL_VALUE( 200 )
		);
		m_pGameMenu->SetVisible( true );
		m_pGameMenu->SetAlpha( 255 );
	}
	MakeReadyForUse();
}

void CBaseModPanel::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CBaseModPanel::SetMainMenuOverride( vgui::VPANEL panel )
{
	if ( m_pGameMenu )
	{
		m_pGameMenu->SetMainMenuOverride( panel );
		m_pGameMenu->DeleteAllItems();
		g_pVGuiPanel->SetParent( panel, GetVPanel() );
	}
}

void CBaseModPanel::PaintBackground()
{
	if ( GameUI().IsInLevel() || GameUI().IsInBackgroundLevel() )
	{
		BaseClass::PaintBackground();
		return;
	}

	int wide, tall;
	GetSize( wide, tall );

	surface()->DrawSetColor( 255, 255, 255, 255 );
	surface()->DrawSetTexture( m_iBackgroundImageID );
	surface()->DrawTexturedRect( 0, 0, wide, tall );

	if ( GameUI().IsInLevel() || m_LevelLoading )
	{
		return;
	}

#if 1
	m_bNeedsDarkening = false;
	// check if children need darkening
	for( auto panel : GetChildren() )
	{
		bool bVisible = g_pVGuiPanel->IsVisible( panel );
		bool bGamepanel = panel == m_pGameMenu->GetVPanel();
		if ( bVisible && !bGamepanel )
		{
			m_bNeedsDarkening = true;
			break;
		}
	}

	// paint dark fade
	if ( m_bNeedsDarkening && !m_bHasDarkened )
	{
		m_bHasDarkened = true;
		vgui::GetAnimationController()->RunAnimationCommand( this, "BackgroundFade", 120.0f, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR );
	}

	if ( !m_bNeedsDarkening && m_bHasDarkened )
	{
		m_bHasDarkened = false;
		vgui::GetAnimationController()->RunAnimationCommand( this, "BackgroundFade", 0.f, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR );
	}

	surface()->DrawSetColor( 0, 0, 0, m_flBackgroundFade );
	surface()->DrawFilledRect( 0, 0, wide, tall );
#endif
}

void CBaseModPanel::OnCommand(const char *command)
{
	vgui::Frame* fromFactory = CDialogFactoryHelper::InstancePanel(command, this);
	if (fromFactory)
	{
		fromFactory->Activate();
	}
	else if ( Q_stristr( command, "engine " ) )
	{
		engine->ClientCmd_Unrestricted( command + Q_strlen( "engine " ) );
	}
	else if ( !V_stricmp(command, "ResumeGame"))
	{
		GameUI().HideGameUI();
	}
	else if ( !V_stricmp(command, "DisconnectNoConfirm" ) )
	{
		engine->ClientCmd_Unrestricted( "disconnect\n" );
	}
	else if ( !V_stricmp(command, "Disconnect" ) )
	{
		// TODO - confirm dialog
		engine->ClientCmd_Unrestricted( "disconnect\n" );
	}
	else if (!V_stricmp(command, "Quit"))
	{
		if (GameUI().IsInLevel() && engine->GetMaxClients() == 1)
		{
			// TODO - save and quit dialog
			return;
		}
		CQuitDialog* pQuitDialog = new CQuitDialog("#GameUI_QuitConfirmationTitle", "#GameUI_QuitConfirmationText", this);
		pQuitDialog->SetOKButtonText("#GameUI_Quit");
		KeyValues* pKeys = new KeyValues("Command", "command", "QuitNoConfirm");
		pQuitDialog->SetOKCommand(pKeys);
		pKeys = new KeyValues("Command", "command", "ReleaseModalWindow");
		pQuitDialog->SetCancelCommand(pKeys);
		pQuitDialog->AddActionSignalTarget(this);
		pQuitDialog->DoModal();
	}
	else if ( !V_stricmp( command, "QuitNoConfirm" ) )
	{
		SetVisible(false);
		vgui::surface()->RestrictPaintToSinglePanel( GetVPanel() );
		engine->ClientCmd_Unrestricted( "quit\n" );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

bool CBaseModPanel::IsReadyToWriteConfig( void )
{
	// For cert we only want to write config files is it has been at least 3 seconds
	return false;
}

void CBaseModPanel::OnSetFocus()
{
	BaseClass::OnSetFocus();
	GameConsole().Hide();
}

void CBaseModPanel::OnMovedPopupToFront()
{
	GameConsole().Hide();
}

void CBaseModPanel::OnKeyCodePressed(KeyCode code)
{
	if( code == KEY_XBUTTON_START )
	{
		if( GameUI().IsInLevel())

		return;
	}
	BaseClass::OnKeyCodePressed( code );
}

CON_COMMAND_F( openserverbrowser, "Opens server browser", 0 )
{
	bool isSteam = IsPC() && steamapicontext->SteamFriends() && steamapicontext->SteamUtils();
	if ( isSteam )
	{
		// show the server browser
		g_VModuleLoader.ActivateModule( "Servers" );

		// if an argument was passed, that's the tab index to show, send a message to server browser to switch to that tab
		//if ( args.ArgC() > 1 )
		//{
		//	KeyValues* pKV = new KeyValues( "ShowServerBrowserPage" );
		//	pKV->SetInt( "page", atoi( args[ 1 ] ) );
		//	g_VModuleLoader.PostMessageToAllModules( pKV );
		//}

#ifdef INFESTED_DLL
		KeyValues* pSchemeKV = new KeyValues( "SetCustomScheme" );
		pSchemeKV->SetString( "SchemeName", "SwarmServerBrowserScheme" );
		g_VModuleLoader.PostMessageToAllModules( pSchemeKV );
#endif
	}
}
