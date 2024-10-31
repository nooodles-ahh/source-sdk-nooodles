#include "serverlistpage.h"
#include <KeyValues.h>
#include "engineinterface.h"
#include "steam/steam_api.h"

extern const char* COM_GetModDirectory();

using namespace vgui;

CServerListPage::CServerListPage( vgui::Panel* parent, const char *name ) : BaseClass(parent, name )
{
	SetProportional( true );
	m_requestingServers = false;
	m_requestHandle = NULL;

	m_pQuickList = new vgui::PanelListPanel( this, "quicklist" );
	m_pQuickList->SetFirstColumnWidth( 0 );

	m_pServerList = new vgui::ListPanel( this, "gamelist" );
	m_pServerList->SetMouseInputEnabled( true );
	m_pServerList->SetAllowUserModificationOfColumns( true );
	m_pServerList->AddColumnHeader( 0, "Password", "#ServerBrowser_Password", 16, 0 );
	m_pServerList->AddColumnHeader( 1, "Secure", "#ServerBrowser_Secure", 16, 0 );
	m_pServerList->AddColumnHeader( 2, "Name", "#ServerBrowser_Servers", 50, 0 );
	m_pServerList->AddColumnHeader( 3, "IPAddr", "#ServerBrowser_IPAddress", 64, 0 );
	m_pServerList->AddColumnHeader( 4, "Players", "#ServerBrowser_Players", 55, 0 );
	m_pServerList->AddColumnHeader( 5, "Bots", "#ServerBrowser_Bots", 40, 0 );
	m_pServerList->AddColumnHeader( 6, "Map", "#ServerBrowser_Map", 90, 90, 300, 0 );
	m_pServerList->AddColumnHeader( 7, "Ping", "#ServerBrowser_Latency", 55, 0 );
	m_pServerList->SetEmptyListText( "#ServerBrowser_NoInternetGames" );

	m_pQuickListCheck = new vgui::CheckButton(this, "quicklistcheck", "#ServerBrowser_QuickListCheck" );
	m_pFilterToggle = new vgui::ToggleButton( this, "Filter", "#ServerBrowser_Filters" );
	m_pGameFilterCombo = new vgui::ComboBox( this, "GameFilter", 2, false );
	m_pLocationFilterCombo = new vgui::ComboBox( this, "LocationFilter", 2, false );
	m_pMapFilterText = new vgui::TextEntry( this, "MapFilter" );
	m_pMaxPlayerText = new vgui::TextEntry( this, "MaxPlayerFilter" );
	m_pPingFilterCombo = new vgui::ComboBox( this, "PingFilter", 2, false );
	m_pSecureFilterCombo = new vgui::ComboBox( this, "SecureFilter", 2, false );
	m_pServerEmptyCheck = new vgui::CheckButton( this, "ServerEmptyFilterCheck", "#ServerBrowser_HasUsersPlaying" );
	m_pServerFullCheck = new vgui::CheckButton( this, "ServerFullFilterCheck", "#ServerBrowser_ServerNotFull" );
	m_pPasswordCheck = new vgui::CheckButton( this, "NoPasswordFilterCheck", "#ServerBrowser_IsNotPasswordProtected" );
	m_pReplayCheck = new vgui::CheckButton(this, "ReplayFilterCheck", "#ServerBrowser_SupportsReplays" );

	m_filters = new MatchMakingKeyValuePair_t;
	V_strncpy( m_filters->m_szKey, "gamedir", sizeof( m_filters->m_szKey ) );
	V_strncpy( m_filters->m_szValue, COM_GetModDirectory(), sizeof( m_filters->m_szValue ) );
	
	m_iFilterCount = 1;
}

CServerListPage::~CServerListPage()
{
	StopQuery();
	m_pServerList->DeleteAllItems();

	if ( m_requestHandle )
	{
		m_requestingServers = false;
		steamapicontext->SteamMatchmakingServers()->ReleaseRequest( m_requestHandle );
		m_requestHandle = NULL;
	}
}

void CServerListPage::OnPageShow()
{
	StartQuery();
}

void CServerListPage::RefreshQuery()
{
	StopQuery();
	m_pServerList->DeleteAllItems();
	StartQuery();
}

void CServerListPage::StartQuery()
{
	if ( m_requestingServers )
	{
		return;
	}

	m_requestingServers = true;

	MakeRequest();
}

void CServerListPage::StopQuery()
{
	if ( !m_requestingServers )
		return;

	m_requestingServers = false;

	steamapicontext->SteamMatchmakingServers()->ReleaseRequest( m_requestHandle );
	m_requestHandle = NULL;
}

void CServerListPage::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_pQuickList->SetVisible( false );
}

void CServerListPage::ServerResponded( HServerListRequest hRequest, int iServer )
{
	gameserveritem_t* info = steamapicontext->SteamMatchmakingServers()->GetServerDetails( hRequest, iServer );
	OnServerListResponse( *info );
}

void CServerListPage::ServerFailedToRespond( HServerListRequest hRequest, int iServer )
{
	gameserveritem_t *info = steamapicontext->SteamMatchmakingServers()->GetServerDetails( hRequest, iServer );
	OnServerListFailed( *info );
}

void CServerListPage::RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response )
{
	m_requestingServers = false;
	steamapicontext->SteamMatchmakingServers()->ReleaseRequest(hRequest);
	m_requestHandle = NULL;
	OnRefreshComplete();
}

// Internet games page
CInternetGames::CInternetGames( vgui::Panel* parent ) : BaseClass( parent, "InternetGames")
{
}

void CInternetGames::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	LoadControlSettings( "servers/internetgamespage_filters.res" );
}

void CInternetGames::MakeRequest()
{
	m_requestHandle = steamapicontext->SteamMatchmakingServers()->RequestInternetServerList(
		steamapicontext->SteamUtils()->GetAppID(), &m_filters, m_iFilterCount, this
	);
}
void CInternetGames::OnServerListResponse( gameserveritem_t info )
{
	KeyValues* data = new KeyValues( info.m_NetAdr.GetConnectionAddressString() );
	data->SetString( "Password", info.m_bPassword ? "Yes" : "No" );
	data->SetString( "Secure", info.m_bSecure ? "Yes" : "No" );
	data->SetString( "Name", info.GetName() );
	data->SetString( "IPAddr", info.m_NetAdr.GetConnectionAddressString() );
	data->SetInt( "Players", info.m_nPlayers );
	data->SetInt( "Bots", info.m_nBotPlayers );
	data->SetString( "Map", info.m_szMap );
	data->SetInt( "Ping", info.m_nPing );

	m_pServerList->AddItem( data, 0, false, false );
}

void CInternetGames::OnServerListFailed( gameserveritem_t info )
{
}

void CInternetGames::OnRefreshComplete()
{
}


// Favourites page
// TODO

// History page
// TODO

// Spectate page
// TODO

// Friends page
// TODO

// LAN page
// TODO

// Blacklist page
// TODO