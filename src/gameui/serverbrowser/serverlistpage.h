#ifndef SERVERLISTPAGE_H
#define SERVERLISTPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/PanelListPanel.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>
#include "steam/steam_api.h"

class CServerListPage : public vgui::PropertyPage, public ISteamMatchmakingServerListResponse
{
	DECLARE_CLASS_SIMPLE( CServerListPage, vgui::PropertyPage );
public:
	CServerListPage( vgui::Panel* parent, const char *name );
	~CServerListPage();
	void RefreshQuery();
	void StartQuery();
	void StopQuery();

	virtual void ApplySchemeSettings( vgui::IScheme* pScheme );

	MESSAGE_FUNC( OnPageShow, "PageShow" );

	virtual void MakeRequest() = 0;
	virtual void OnServerListResponse( gameserveritem_t info ) = 0;
	virtual void OnServerListFailed( gameserveritem_t info ) = 0;
	virtual void OnRefreshComplete() = 0;

	// ISteamMatchmakingServerListResponse overrides
	virtual void ServerResponded( HServerListRequest hRequest, int iServer );
	virtual void ServerFailedToRespond( HServerListRequest hRequest, int iServer );
	virtual void RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response );

protected:
	bool                    m_requestingServers;
	HServerListRequest      m_requestHandle;
	MatchMakingKeyValuePair_t *m_filters;
	int m_iFilterCount;

	vgui::ListPanel* m_pServerList;
	vgui::PanelListPanel* m_pQuickList;
	vgui::CheckButton* m_pQuickListCheck;
	vgui::ToggleButton* m_pFilterToggle;
	vgui::CheckButton* m_pPasswordCheck;
	vgui::CheckButton* m_pServerFullCheck;
	vgui::CheckButton* m_pServerEmptyCheck;
	vgui::ComboBox* m_pSecureFilterCombo;
	vgui::ComboBox* m_pPingFilterCombo;
	vgui::TextEntry* m_pMaxPlayerText;
	vgui::TextEntry* m_pMapFilterText;
	vgui::ComboBox* m_pGameFilterCombo;
	vgui::ComboBox* m_pLocationFilterCombo;
	vgui::CheckButton* m_pReplayCheck;
};


class CInternetGames : public CServerListPage
{
	DECLARE_CLASS_SIMPLE( CInternetGames, CServerListPage );
public:
	CInternetGames(vgui::Panel *parent);

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	virtual void MakeRequest();
	virtual void OnServerListResponse( gameserveritem_t info );
	virtual void OnServerListFailed( gameserveritem_t info );
	virtual void OnRefreshComplete();


};
#endif