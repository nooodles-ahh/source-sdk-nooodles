//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Defines the interface that the GameUI dll exports
//
// $NoKeywords: $
//=============================================================================//

#ifndef GAMEUI_INTERFACE_H
#define GAMEUI_INTERFACE_H
#pragma once

#include "GameUI/IGameUI.h"

#include "vgui_controls/Panel.h"
#include "vgui_controls/PHandle.h"
#include "convar.h"

class IGameClientExports;
class CCommand;

//-----------------------------------------------------------------------------
// Purpose: Implementation of GameUI's exposed interface 
//-----------------------------------------------------------------------------
class CGameUI : public IGameUI
{
public:
	CGameUI();
	~CGameUI();

	virtual void Initialize( CreateInterfaceFn appFactory );
	virtual void PostInit();

	virtual void Connect( CreateInterfaceFn gameFactory );

	virtual void Start();
	virtual void Shutdown();
	virtual void RunFrame();

	// notifications
	virtual void OnGameUIActivated();
	virtual void OnGameUIHidden();

	// OLD: use OnConnectToServer2
	virtual void OLD_OnConnectToServer( const char *game, int IP, int port );
	virtual void OnDisconnectFromServer_OLD( uint8 eSteamLoginFailure, const char *username ) { OnDisconnectFromServer( eSteamLoginFailure ); }

	virtual void OnLevelLoadingStarted( bool bShowProgressDialog );
	virtual void OnLevelLoadingFinished( bool bError, const char *failureReason, const char *extendedReason );

	// progress
	virtual bool UpdateProgressBar(float progress, const char* statusText);
	// Shows progress desc, returns previous setting... (used with custom progress bars )
	virtual bool SetShowProgressText(bool show);

	// !!!!!!!!!members added after "GameUI011" initial release!!!!!!!!!!!!!!!!!!!
	virtual void ShowNewGameDialog( int chapter );

	// Xbox 360
	virtual void SessionNotification( const int notification, const int param = 0 );
	virtual void SystemNotification( const int notification );
	virtual void ShowMessageDialog( const uint nType, vgui::Panel *pOwner );
	virtual void UpdatePlayerInfo( uint64 nPlayerId, const char *pName, int nTeam, byte cVoiceState, int nPlayersNeeded, bool bHost );
	virtual void SessionSearchResult( int searchIdx, void *pHostData, XSESSION_SEARCHRESULT *pResult, int ping );
	virtual void OnCreditsFinished( void );

	virtual void SetLoadingBackgroundDialog( vgui::VPANEL panel );

	virtual void BonusMapUnlock( const char *pchFileName = NULL, const char *pchMapName = NULL );
	virtual void BonusMapComplete( const char *pchFileName = NULL, const char *pchMapName = NULL );
	virtual void BonusMapChallengeUpdate( const char *pchFileName, const char *pchMapName, const char *pchChallengeName, int iBest );
	virtual void BonusMapChallengeNames( char *pchFileName, char *pchMapName, char *pchChallengeName );
	virtual void BonusMapChallengeObjectives( int &iBronze, int &iSilver, int &iGold );
	virtual void BonusMapDatabaseSave( void );
	virtual int BonusMapNumAdvancedCompleted( void );
	virtual void BonusMapNumMedals( int piNumMedals[3] );

	virtual void OnConnectToServer2( const char *game, int IP, int connectionPort, int queryPort );

	virtual bool ValidateStorageDevice( int *pStorageDeviceValidated );

	virtual void SetProgressOnStart();
	virtual void OnDisconnectFromServer(uint8 eSteamLoginFailure);

	virtual void OnConfirmQuit( void );

	virtual bool IsMainMenuVisible( void );

	// Client DLL is providing us with a panel that it wants to replace the main menu with
	virtual void SetMainMenuOverride( vgui::VPANEL panel );
	// Client DLL is telling us that a main menu command was issued, probably from its custom main menu panel
	virtual void SendMainMenuCommand( const char *pszCommand );
 
#if defined( _DEMO )
	virtual void OnDemoTimeout();
#endif

	// plays the startup mp3 when GameUI starts
	void PlayGameStartupSound();

	// Engine wrappers for activating / hiding the gameUI
	void ActivateGameUI();
	void HideGameUI();

	// Toggle allowing the engine to hide the game UI with the escape key
	void PreventEngineHideGameUI();
	void AllowEngineHideGameUI();

 	// state
 	bool IsInLevel();
 	bool IsInBackgroundLevel();
 	bool IsInMultiplayer();
 	bool IsConsoleUI();
 	bool HasSavedThisMenuSession();
 	void SetSavedThisMenuSession( bool bState );
 
 	void ShowLoadingBackgroundDialog();
	void HideLoadingBackgroundDialog();
	bool HasLoadingBackgroundDialog();

private:
	void StartProgressBar();
	void StopProgressBar(bool bError, const char* failureReason, const char* extendedReason = NULL);
	void SendConnectedToGameMessage();

	bool FindPlatformDirectory(char *platformDir, int bufferSize);
	void GetUpdateVersion( char *pszProd, char *pszVer);
	void ValidateCDKey();

	CreateInterfaceFn m_GameFactory;

	bool m_bTryingToLoadFriends : 1;
	bool m_bActivatedUI : 1;
	bool m_bIsConsoleUI : 1;
	bool m_bHasSavedThisMenuSession : 1;
	bool m_bOpenProgressOnStart : 1;

	int m_iGameIP;
	int m_iGameConnectionPort;
	int m_iGameQueryPort;
	
	int m_iFriendsLoadPauseFrames;
	int m_iPlayGameStartupSound;

	int m_nOldWide;
	int m_nOldTall;

	char m_szPreviousStatusText[128];
	char m_szPlatformDir[MAX_PATH];

	vgui::DHANDLE<class CCDKeyEntryDialog> m_hCDKeyEntryDialog;

	int m_nChangeFrameCount;
};

// Purpose: singleton accessor
extern CGameUI &GameUI();

// expose client interface
extern IGameClientExports *GameClientExports();

#endif // GAMEUI_INTERFACE_H
