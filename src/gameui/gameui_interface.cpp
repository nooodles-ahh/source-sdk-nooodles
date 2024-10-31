//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Implements all the functions exported by the GameUI dll
//
// $NoKeywords: $
//===========================================================================//

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <tier0/dbg.h>
																
#include "filesystem.h"
#include "gameui_interface.h"
#include "sys_utils.h"
#include "string.h"
#include "tier0/icommandline.h"

// interface to engine
#include "engineinterface.h"
#include "bitmap/tgaloader.h"

#include "gameconsole.h"
#include "loadingdialog.h"
#include "modinfo.h"
#include "game/client/IGameClientExports.h"
#include "materialsystem/imaterialsystem.h"
#include "ixboxsystem.h"
#include "iachievementmgr.h"
#include "IGameUIFuncs.h"
#include "ienginevgui.h"
#include "vguisystemmoduleloader.h"
#include "threadtools.h"

// vgui2 interface
// note that GameUI project uses ..\vgui2\include, not ..\utils\vgui\include
#include "vgui/Cursor.h"
#include "tier1/KeyValues.h"
#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/IVGui.h"
#include "vgui/ISystem.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/PHandle.h"
#include "tier3/tier3.h"
#include "matsys_controls/matsyscontrols.h"
#include "steam/steam_api.h"

#include <time.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include "basemodpanel.h"
#include "basemodui.h"
typedef BaseModUI::CBaseModPanel UI_BASEMOD_PANEL_CLASS;
inline UI_BASEMOD_PANEL_CLASS &GetUiBaseModPanelClass() { return UI_BASEMOD_PANEL_CLASS::GetSingleton(); }
inline UI_BASEMOD_PANEL_CLASS &ConstructUiBaseModPanelClass() { return *new UI_BASEMOD_PANEL_CLASS(); }

#include "tier0/dbg.h"
#include "engine/IEngineSound.h"
#include "gameui_util.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IEngineVGui* enginevguifuncs = nullptr;
vgui::ISurface* enginesurfacefuncs = nullptr;
IAchievementMgr *achievementmgr = nullptr;

IVEngineClient *engine = nullptr;
IEngineSound *enginesound = nullptr;
IGameUIFuncs *gameuifuncs = nullptr;

#ifndef GAMEUI_EMBEDDED
static CSteamAPIContext s_SteamAPIContext;	
CSteamAPIContext *steamapicontext = &s_SteamAPIContext;
#endif

class CGameUI;
CGameUI *g_pGameUI = NULL;
static WHANDLE g_hMutex = NULL;
static WHANDLE g_hWaitMutex = NULL;

class CLoadingDialog;
vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;
vgui::VPANEL g_hLoadingBackgroundDialog = NULL;

static CGameUI g_GameUI;

static IGameClientExports *g_pGameClientExports = NULL;
IGameClientExports *GameClientExports()
{
	return g_pGameClientExports;
}

//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
CGameUI &GameUI()
{
	return g_GameUI;
}

//-----------------------------------------------------------------------------
// Purpose: hack function to give the module loader access to the main panel handle
//			only used in VguiSystemModuleLoader
//-----------------------------------------------------------------------------
vgui::VPANEL GetGameUIBasePanel()
{
	return GetUiBaseModPanelClass().GetVPanel();
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION, g_GameUI);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameUI::CGameUI()
{
	g_pGameUI = this;
	m_bTryingToLoadFriends = false;
	m_iFriendsLoadPauseFrames = 0;
	m_iGameIP = 0;
	m_iGameConnectionPort = 0;
	m_iGameQueryPort = 0;
	m_bActivatedUI = false;
	m_szPreviousStatusText[0] = 0;
	m_bIsConsoleUI = false;
	m_bHasSavedThisMenuSession = false;
	m_bOpenProgressOnStart = false;
	m_iPlayGameStartupSound = 0;
	m_nOldWide = -1;
	m_nOldTall = -1;
	m_nChangeFrameCount = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CGameUI::~CGameUI()
{
	g_pGameUI = NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Initialization
//-----------------------------------------------------------------------------
void CGameUI::Initialize( CreateInterfaceFn factory )
{
	MEM_ALLOC_CREDIT();
	ConnectTier1Libraries( &factory, 1 );
	ConnectTier2Libraries( &factory, 1 );
	ConVar_Register( FCVAR_CLIENTDLL );
	ConnectTier3Libraries( &factory, 1 );

	enginesound = ( IEngineSound* )factory( IENGINESOUND_CLIENT_INTERFACE_VERSION, NULL );
	engine = ( IVEngineClient* )factory( VENGINE_CLIENT_INTERFACE_VERSION, NULL );

	//SteamAPI_InitSafe();
	steamapicontext->Init();

	ConVarRef var( "gameui_xbox" );
	m_bIsConsoleUI = var.IsValid() && var.GetBool();

	vgui::VGui_InitInterfacesList( "GameUI", &factory, 1 );
	vgui::VGui_InitMatSysInterfacesList( "GameUI", &factory, 1 );

	// load localization file
	g_pVGuiLocalize->AddFile( "Resource/gameui_%language%.txt", "GAME", true );

	// load mod info
	ModInfo().LoadCurrentGameInfo();

	// load localization file for kb_act.lst
	g_pVGuiLocalize->AddFile( "Resource/valve_%language%.txt", "GAME", true );

	enginevguifuncs = (IEngineVGui *)factory( VENGINE_VGUI_VERSION, NULL );
	enginesurfacefuncs = ( vgui::ISurface* )factory( VGUI_SURFACE_INTERFACE_VERSION, NULL );
	gameuifuncs = ( IGameUIFuncs* )factory( VENGINE_GAMEUIFUNCS_VERSION, NULL );

	if ( !enginesurfacefuncs || !gameuifuncs || !enginevguifuncs )
	{
		Error( "CGameUI::Initialize() failed to get necessary interfaces\n" );
	}

	// setup base panel
	UI_BASEMOD_PANEL_CLASS& factoryBasePanel = ConstructUiBaseModPanelClass(); // explicit singleton instantiation

	factoryBasePanel.SetBounds( 0, 0, 640, 480 );
	factoryBasePanel.SetPaintBorderEnabled( false );
	factoryBasePanel.SetPaintBackgroundEnabled( true );
	factoryBasePanel.SetPaintEnabled( true );
	factoryBasePanel.SetVisible( true );

	factoryBasePanel.SetMouseInputEnabled( false );
	factoryBasePanel.SetKeyBoardInputEnabled( false );

	vgui::VPANEL rootpanel = enginevguifuncs->GetPanel( PANEL_GAMEUIDLL );
	factoryBasePanel.SetParent( rootpanel );
}

void CGameUI::PostInit()
{
	//vgui::surface()->GetScreenSize( m_nOldWide, m_nOldTall );
}

//-----------------------------------------------------------------------------
// Purpose: Sets the specified panel as the background panel for the loading
//		dialog.  If NULL, default background is used.  If you set a panel,
//		it should be full-screen with an opaque background, and must be a VGUI popup.
//-----------------------------------------------------------------------------
void CGameUI::SetLoadingBackgroundDialog( vgui::VPANEL panel )
{
	g_hLoadingBackgroundDialog = panel;
}

//-----------------------------------------------------------------------------
// Purpose: connects to client interfaces
//-----------------------------------------------------------------------------
void CGameUI::Connect( CreateInterfaceFn gameFactory )
{
	g_pGameClientExports = (IGameClientExports *)gameFactory(GAMECLIENTEXPORTS_INTERFACE_VERSION, NULL);

	achievementmgr = engine->GetAchievementMgr();

	if (!g_pGameClientExports)
	{
		Error("CGameUI::Initialize() failed to get necessary interfaces\n");
	}

	m_GameFactory = gameFactory;
}

//-----------------------------------------------------------------------------
// Purpose: Callback function; sends platform Shutdown message to specified window
//-----------------------------------------------------------------------------
int __stdcall SendShutdownMsgFunc(WHANDLE hwnd, int lparam)
{
	Sys_PostMessage(hwnd, Sys_RegisterWindowMessage("ShutdownValvePlatform"), 0, 1);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Searches for GameStartup*.mp3 files in the sound/ui folder and plays one
//-----------------------------------------------------------------------------
void CGameUI::PlayGameStartupSound()
{
	if ( CommandLine()->FindParm( "-nostartupsound" ) )
		return;

	FileFindHandle_t fh;

	CUtlVector<char *> fileNames;

	char path[ 512 ];
	Q_snprintf( path, sizeof( path ), "sound/ui/gamestartup*.mp3" );
	Q_FixSlashes( path );

	char const *fn = g_pFullFileSystem->FindFirstEx( path, "MOD", &fh );
	if ( fn )
	{
		do
		{
			char ext[ 10 ];
			Q_ExtractFileExtension( fn, ext, sizeof( ext ) );

			if ( !Q_stricmp( ext, "mp3" ) )
			{
				char temp[ 512 ];
				Q_snprintf( temp, sizeof( temp ), "ui/%s", fn );

				char *found = new char[ strlen( temp ) + 1 ];
				Q_strncpy( found, temp, strlen( temp ) + 1 );

				Q_FixSlashes( found );
				fileNames.AddToTail( found );
			}
	
			fn = g_pFullFileSystem->FindNext( fh );

		} while ( fn );

		g_pFullFileSystem->FindClose( fh );
	}

	// did we find any?
	if ( fileNames.Count() > 0 )
	{
		int index = Plat_MSTime() % fileNames.Count();

		if ( fileNames.IsValidIndex( index ) && fileNames[index] )
		{
			char found[ 512 ];

			// escape chars "*#" make it stream, and be affected by snd_musicvolume
			Q_snprintf( found, sizeof( found ), "play *#%s", fileNames[index] );

			engine->ClientCmd_Unrestricted( found );
		}

		fileNames.PurgeAndDeleteElements();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called to setup the game UI
//-----------------------------------------------------------------------------
void CGameUI::Start()
{
	// determine Steam location for configuration
	if ( !FindPlatformDirectory( m_szPlatformDir, sizeof( m_szPlatformDir ) ) )
		return;

	// setup config file directory
	char szConfigDir[512];
	Q_strncpy( szConfigDir, m_szPlatformDir, sizeof( szConfigDir ) );
	Q_strncat( szConfigDir, "config", sizeof( szConfigDir ), COPY_ALL_CHARACTERS );

	Msg( "Steam config directory: %s\n", szConfigDir );

	g_pFullFileSystem->AddSearchPath(szConfigDir, "CONFIG");
	g_pFullFileSystem->CreateDirHierarchy("", "CONFIG");

	// user dialog configuration
	vgui::system()->SetUserConfigFile("InGameDialogConfig.vdf", "CONFIG");

	g_pFullFileSystem->AddSearchPath( "platform", "PLATFORM" );

	// localization
	g_pVGuiLocalize->AddFile( "Resource/platform_%language%.txt");
	g_pVGuiLocalize->AddFile( "Resource/vgui_%language%.txt");

	Sys_SetLastError( SYS_NO_ERROR );

	g_hMutex = Sys_CreateMutex( "ValvePlatformUIMutex" );
	g_hWaitMutex = Sys_CreateMutex( "ValvePlatformWaitMutex" );
	if ( g_hMutex == NULL || g_hWaitMutex == NULL || Sys_GetLastError() == SYS_ERROR_INVALID_HANDLE )
	{
		// error, can't get handle to mutex
		if ( g_hMutex )
		{
			Sys_ReleaseMutex( g_hMutex );
		}
		if ( g_hWaitMutex )
		{
			Sys_ReleaseMutex( g_hWaitMutex );
		}
		g_hMutex = NULL;
		g_hWaitMutex = NULL;
		Error( "Steam Error: Could not access Steam, bad mutex\n" );
		return;
	}
	unsigned int waitResult = Sys_WaitForSingleObject( g_hMutex, 0 );
	if ( !( waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED ) )
	{
		// mutex locked, need to deactivate Steam (so we have the Friends/ServerBrowser data files)
		// get the wait mutex, so that Steam.exe knows that we're trying to acquire ValveTrackerMutex
		waitResult = Sys_WaitForSingleObject( g_hWaitMutex, 0 );
		if ( waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED )
		{
			Sys_EnumWindows( SendShutdownMsgFunc, 1 );
		}
	}

	// Delay playing the startup music until two frames
	// this allows cbuf commands that occur on the first frame that may start a map
	m_iPlayGameStartupSound = 2;

	// now we are set up to check every frame to see if we can friends/server browser
	m_bTryingToLoadFriends = true;
	m_iFriendsLoadPauseFrames = 1;
}

//-----------------------------------------------------------------------------
// Purpose: Validates the user has a cdkey in the registry
//-----------------------------------------------------------------------------
void CGameUI::ValidateCDKey()
{
}

//-----------------------------------------------------------------------------
// Purpose: Finds which directory the platform resides in
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CGameUI::FindPlatformDirectory(char *platformDir, int bufferSize)
{
	platformDir[0] = '\0';
	if ( platformDir[0] == '\0' )
	{
		if ( getcwd( platformDir, bufferSize ) )
		{
			Q_strncat(platformDir, "\\platform\\", bufferSize, COPY_ALL_CHARACTERS );
			return true;
		}

		Error( "Unable to determine platform directory\n" );
		return false;
	}

	return (platformDir[0] != 0);
}

//-----------------------------------------------------------------------------
// Purpose: Called to Shutdown the game UI system
//-----------------------------------------------------------------------------
void CGameUI::Shutdown()
{
	// notify all the modules of Shutdown
	g_VModuleLoader.ShutdownPlatformModules();

	// unload the modules them from memory
	g_VModuleLoader.UnloadPlatformModules();

	ModInfo().FreeModInfo();

	// release platform mutex
	// close the mutex
	if ( g_hMutex )
	{
		Sys_ReleaseMutex( g_hMutex );
	}
	if ( g_hWaitMutex )
	{
		Sys_ReleaseMutex( g_hWaitMutex );
	}

	steamapicontext->Clear();
	
	ConVar_Unregister();
	DisconnectTier3Libraries();
	DisconnectTier2Libraries();
	DisconnectTier1Libraries();
}

//-----------------------------------------------------------------------------
// Purpose: just wraps an engine call to activate the gameUI
//-----------------------------------------------------------------------------
void CGameUI::ActivateGameUI()
{
	engine->ExecuteClientCmd("gameui_activate");
}

//-----------------------------------------------------------------------------
// Purpose: just wraps an engine call to hide the gameUI
//-----------------------------------------------------------------------------
void CGameUI::HideGameUI()
{
	engine->ExecuteClientCmd("gameui_hide");
}

//-----------------------------------------------------------------------------
// Purpose: Toggle allowing the engine to hide the game UI with the escape key
//-----------------------------------------------------------------------------
void CGameUI::PreventEngineHideGameUI()
{
	engine->ExecuteClientCmd("gameui_preventescape");
}

//-----------------------------------------------------------------------------
// Purpose: Toggle allowing the engine to hide the game UI with the escape key
//-----------------------------------------------------------------------------
void CGameUI::AllowEngineHideGameUI()
{
	engine->ExecuteClientCmd("gameui_allowescape");
}

//-----------------------------------------------------------------------------
// Purpose: Activate the game UI
//-----------------------------------------------------------------------------
void CGameUI::OnGameUIActivated()
{
	m_bActivatedUI = true;

	// pause the server in case it is pausable
	engine->ClientCmd_Unrestricted( "setpause nomsg" );

	SetSavedThisMenuSession( false );

	UI_BASEMOD_PANEL_CLASS &ui = GetUiBaseModPanelClass();
	bool bNeedActivation = true;
	if ( ui.IsVisible() )
	{
		// Already visible, maybe don't need activation
		if ( !IsInLevel() && IsInBackgroundLevel() )
			bNeedActivation = false;
	}
	if ( bNeedActivation )
	{
		GetUiBaseModPanelClass().OnGameUIActivated();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Hides the game ui, in whatever state it's in
//-----------------------------------------------------------------------------
void CGameUI::OnGameUIHidden()
{
	m_bActivatedUI = false;

	// unpause the game when leaving the UI
	engine->ClientCmd_Unrestricted( "unpause nomsg" );

	GetUiBaseModPanelClass().OnGameUIHidden();
}

//-----------------------------------------------------------------------------
// Purpose: paints all the vgui elements
//-----------------------------------------------------------------------------
void CGameUI::RunFrame()
{
	if ( /*IsX360() &&*/ m_bOpenProgressOnStart )
	{
		StartProgressBar();
		m_bOpenProgressOnStart = false;
	}

	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);
	if ( m_nChangeFrameCount > 0)
	{
		m_nChangeFrameCount--;
		if( m_nChangeFrameCount == 0)
		{
			m_nOldWide = wide;
			m_nOldTall = tall;
			GetUiBaseModPanelClass().SetSize( wide, tall );
		}
	}
	else if( m_nOldWide != wide || m_nOldTall != tall )
	{
		m_nChangeFrameCount = 3;
		GetUiBaseModPanelClass().SetSize( wide, tall );
		// post a message so by the time it gets there the resolution should have changed
		GetUiBaseModPanelClass().PostMessage( GetUiBaseModPanelClass().GetVPanel(),
			new KeyValues( "OnScreenSizeChanged", "oldwide", wide, "oldtall", tall ), NULL );
	}

	// Run frames
	g_VModuleLoader.RunFrame();

	GetUiBaseModPanelClass().RunFrame();

	// Play the start-up music the first time we run frame
	if ( m_iPlayGameStartupSound > 0 )
	{
		m_iPlayGameStartupSound--;
		if ( !m_iPlayGameStartupSound )
		{
			PlayGameStartupSound();
		}
	}

	if ( m_bTryingToLoadFriends && m_iFriendsLoadPauseFrames-- < 1 && g_hMutex && g_hWaitMutex )
	{
		// try and load Steam platform files
		unsigned int waitResult = Sys_WaitForSingleObject( g_hMutex, 0 );
		if ( waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED )
		{
			// we got the mutex, so load Friends/Serverbrowser
			// clear the loading flag
			m_bTryingToLoadFriends = false;
			g_VModuleLoader.LoadPlatformModules( &m_GameFactory, 1, false );

			// release the wait mutex
			Sys_ReleaseMutex( g_hWaitMutex );

			// notify the game of our game name
			const char* fullGamePath = engine->GetGameDirectory();
			const char* pathSep = strrchr( fullGamePath, '/' );
			if ( !pathSep )
			{
				pathSep = strrchr( fullGamePath, '\\' );
			}
			if ( pathSep )
			{
				KeyValues* pKV = new KeyValues( "ActiveGameName" );
				pKV->SetString( "name", pathSep + 1 );
				pKV->SetInt( "appid", engine->GetAppID() );
				KeyValues* modinfo = new KeyValues( "ModInfo" );
				if ( modinfo->LoadFromFile( g_pFullFileSystem, "gameinfo.txt" ) )
				{
					pKV->SetString( "game", modinfo->GetString( "game", "" ) );
				}
				modinfo->deleteThis();

				g_VModuleLoader.PostMessageToAllModules( pKV );
			}

			// notify the ui of a game connect if we're already in a game
			if ( m_iGameIP )
			{
				SendConnectedToGameMessage();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when the game connects to a server
//-----------------------------------------------------------------------------
void CGameUI::OLD_OnConnectToServer(const char *game, int IP, int port)
{
	// Nobody should use this anymore because the query port and the connection port can be different.
	// Use OnConnectToServer2 instead.
	Assert( false );
	OnConnectToServer2( game, IP, port, port );
}

//-----------------------------------------------------------------------------
// Purpose: Called when the game connects to a server
//-----------------------------------------------------------------------------
void CGameUI::OnConnectToServer2(const char *game, int IP, int connectionPort, int queryPort)
{
	m_iGameIP = IP;
	m_iGameConnectionPort = connectionPort;
	m_iGameQueryPort = queryPort;

	SendConnectedToGameMessage();
}


void CGameUI::SendConnectedToGameMessage()
{
	MEM_ALLOC_CREDIT();
	KeyValues *kv = new KeyValues( "ConnectedToGame" );
	kv->SetInt( "ip", m_iGameIP );
	kv->SetInt( "connectionport", m_iGameConnectionPort );
	kv->SetInt( "queryport", m_iGameQueryPort );

	g_VModuleLoader.PostMessageToAllModules( kv );
}



//-----------------------------------------------------------------------------
// Purpose: Called when the game disconnects from a server
//-----------------------------------------------------------------------------
void CGameUI::OnDisconnectFromServer( uint8 eSteamLoginFailure )
{
	m_iGameIP = 0;
	m_iGameConnectionPort = 0;
	m_iGameQueryPort = 0;

	if ( g_hLoadingBackgroundDialog )
	{
		vgui::ivgui()->PostMessage( g_hLoadingBackgroundDialog, new KeyValues("DisconnectedFromGame"), NULL );
	}

	g_VModuleLoader.PostMessageToAllModules( new KeyValues( "DisconnectedFromGame" ) );

	if ( eSteamLoginFailure == STEAMLOGINFAILURE_NOSTEAMLOGIN )
	{
		if ( g_hLoadingDialog )
		{
			g_hLoadingDialog->DisplayNoSteamConnectionError();
		}
	}
	else if ( eSteamLoginFailure == STEAMLOGINFAILURE_VACBANNED )
	{
		if ( g_hLoadingDialog )
		{
			g_hLoadingDialog->DisplayVACBannedError();
		}
	}
	else if ( eSteamLoginFailure == STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE )
	{
		if ( g_hLoadingDialog )
		{
			g_hLoadingDialog->DisplayLoggedInElsewhereError();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: activates the loading dialog on level load start
//-----------------------------------------------------------------------------
void CGameUI::OnLevelLoadingStarted( bool bShowProgressDialog )
{
	g_VModuleLoader.PostMessageToAllModules( new KeyValues( "LoadingStarted" ) );


	GetUiBaseModPanelClass().OnLevelLoadingStarted( NULL, bShowProgressDialog );
	if ( bShowProgressDialog )
	{
		ShowLoadingBackgroundDialog();
		StartProgressBar();
	}

	// Don't play the start game sound if this happens before we get to the first frame
	m_iPlayGameStartupSound = 0;
}

//-----------------------------------------------------------------------------
// Purpose: closes any level load dialog
//-----------------------------------------------------------------------------
void CGameUI::OnLevelLoadingFinished(bool bError, const char *failureReason, const char *extendedReason)
{
	StopProgressBar( bError, failureReason, extendedReason );

	// notify all the modules
	g_VModuleLoader.PostMessageToAllModules( new KeyValues( "LoadingFinished" ) );

	HideLoadingBackgroundDialog();

	GetUiBaseModPanelClass().OnLevelLoadingFinished( bError, failureReason, extendedReason );

}

//-----------------------------------------------------------------------------
// Purpose: Updates progress bar
// Output : Returns true if screen should be redrawn
//-----------------------------------------------------------------------------
bool CGameUI::UpdateProgressBar(float progress, const char *statusText)
{
	return GetUiBaseModPanelClass().UpdateProgressBar(progress, statusText);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGameUI::StartProgressBar()
{
}

//-----------------------------------------------------------------------------
// Purpose: stops progress bar, displays error if necessary
//-----------------------------------------------------------------------------
void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	if (!g_hLoadingDialog.Get())
		return;

	if ( bError )
	{
		// turn the dialog to error display mode
		g_hLoadingDialog->DisplayGenericError(failureReason, extendedReason);
	}
	else
	{
		// close loading dialog
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}
	// should update the background to be in a transition here
}

//-----------------------------------------------------------------------------
// Purpose: Returns prev settings
//-----------------------------------------------------------------------------
bool CGameUI::SetShowProgressText( bool show )
{
	if (!g_hLoadingDialog.Get())
		return false;

	return g_hLoadingDialog->SetShowProgressText( show );
}


//-----------------------------------------------------------------------------
// Purpose: returns true if we're currently playing the game
//-----------------------------------------------------------------------------
bool CGameUI::IsInLevel()
{
	const char *levelName = engine->GetLevelName();
	if (levelName && levelName[0] && !engine->IsLevelMainMenuBackground())
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if we're at the main menu and a background level is loaded
//-----------------------------------------------------------------------------
bool CGameUI::IsInBackgroundLevel()
{
	const char *levelName = engine->GetLevelName();
	if (levelName && levelName[0] && engine->IsLevelMainMenuBackground())
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if we're in a multiplayer game
//-----------------------------------------------------------------------------
bool CGameUI::IsInMultiplayer()
{
	return (IsInLevel() && engine->GetMaxClients() > 1);
}

//-----------------------------------------------------------------------------
// Purpose: returns true if we're console ui
//-----------------------------------------------------------------------------
bool CGameUI::IsConsoleUI()
{
	return m_bIsConsoleUI;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if we've saved without closing the menu
//-----------------------------------------------------------------------------
bool CGameUI::HasSavedThisMenuSession()
{
	return m_bHasSavedThisMenuSession;
}

void CGameUI::SetSavedThisMenuSession( bool bState )
{
	m_bHasSavedThisMenuSession = bState;
}

//-----------------------------------------------------------------------------
// Purpose: Makes the loading background dialog visible, if one has been set
//-----------------------------------------------------------------------------
void CGameUI::ShowLoadingBackgroundDialog()
{
	if ( g_hLoadingBackgroundDialog )
	{
		vgui::VPANEL panel = GetUiBaseModPanelClass().GetVPanel();

		vgui::ipanel()->SetVisible( g_hLoadingBackgroundDialog, true );
		vgui::ipanel()->SetParent( g_hLoadingBackgroundDialog, panel );
		vgui::ipanel()->MoveToFront( g_hLoadingBackgroundDialog );
		vgui::ivgui()->PostMessage( g_hLoadingBackgroundDialog, new KeyValues( "activate" ), NULL );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Hides the loading background dialog, if one has been set
//-----------------------------------------------------------------------------
void CGameUI::HideLoadingBackgroundDialog()
{
	if ( g_hLoadingBackgroundDialog )
	{
		vgui::ipanel()->SetVisible( g_hLoadingBackgroundDialog, false );
		vgui::ipanel()->SetParent( g_hLoadingBackgroundDialog, NULL );
		vgui::ipanel()->MoveToBack( g_hLoadingBackgroundDialog );
		vgui::ivgui()->PostMessage( g_hLoadingBackgroundDialog, new KeyValues("deactivate"), NULL );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether a loading background dialog has been set
//-----------------------------------------------------------------------------
bool CGameUI::HasLoadingBackgroundDialog()
{
	return ( NULL != g_hLoadingBackgroundDialog );
}

//-----------------------------------------------------------------------------
void CGameUI::SetProgressOnStart()
{
	m_bOpenProgressOnStart = true;
}

#if defined( _DEMO )
void CGameUI::OnDemoTimeout()
{
	GetUiBaseModPanelClass().OnDemoTimeout();
}
#endif

void CGameUI::ShowNewGameDialog( int chapter )
{
	// TODO	
}

void CGameUI::SessionNotification( const int notification, const int param /*= 0*/ )
{
	// TODO
}

void CGameUI::SystemNotification( const int notification )
{
	// TODO
}

void CGameUI::ShowMessageDialog( const uint nType, vgui::Panel *pOwner )
{
	// TODO
}

void CGameUI::UpdatePlayerInfo( uint64 nPlayerId, const char *pName, int nTeam, byte cVoiceState, int nPlayersNeeded, bool bHost )
{
	// TODO
}

void CGameUI::SessionSearchResult( int searchIdx, void *pHostData, XSESSION_SEARCHRESULT *pResult, int ping )
{
	// TODO
}

void CGameUI::OnCreditsFinished( void )
{
	// TODO
}

void CGameUI::BonusMapUnlock( const char *pchFileName /*= NULL*/, const char *pchMapName /*= NULL*/ )
{
	// TODO
}

void CGameUI::BonusMapComplete( const char *pchFileName /*= NULL*/, const char *pchMapName /*= NULL*/ )
{
	// TODO
}

void CGameUI::BonusMapChallengeUpdate( const char *pchFileName, const char *pchMapName, const char *pchChallengeName, int iBest )
{
	// TODO
}

void CGameUI::BonusMapChallengeNames( char *pchFileName, char *pchMapName, char *pchChallengeName )
{
	// TODO
}

void CGameUI::BonusMapChallengeObjectives( int &iBronze, int &iSilver, int &iGold )
{
	// TODO
}

void CGameUI::BonusMapDatabaseSave( void )
{
	// TODO
}

int CGameUI::BonusMapNumAdvancedCompleted( void )
{
	// TODO
	return 0;
}

void CGameUI::BonusMapNumMedals( int piNumMedals[3] )
{
	// TODO
}

// X360 Storage device validation. Not required to implement
bool CGameUI::ValidateStorageDevice( int *pStorageDeviceValidated )
{
	return false;
}

void CGameUI::OnConfirmQuit( void )
{
	// TODO
}

bool CGameUI::IsMainMenuVisible( void )
{
	// TODO
	return true;
}

void CGameUI::SetMainMenuOverride( vgui::VPANEL panel )
{
	UI_BASEMOD_PANEL_CLASS& ui = GetUiBaseModPanelClass();
	ui.SetMainMenuOverride( panel );
}

void CGameUI::SendMainMenuCommand( const char *pszCommand )
{
	UI_BASEMOD_PANEL_CLASS& ui = GetUiBaseModPanelClass();
	ui.OnCommand( pszCommand );
}