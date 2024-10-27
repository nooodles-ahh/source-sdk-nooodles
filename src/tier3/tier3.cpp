#include "tier3/tier3.h"
#include <istudiorender.h>
#include <VGuiMatSurface/IMatSystemSurface.h>
#include <vgui/IInput.h>
#include <vgui/IVGui.h>
#include <vgui/IPanel.h>
#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/ISystem.h>
#include <datacache/idatacache.h>
#include <video/ivideoservices.h>
#include <vphysics_interface.h>
#include <SoundEmitterSystem/isoundemittersystembase.h>
#include <ivtex.h>

IStudioRender* g_pStudioRender = nullptr;
IStudioRender* studiorender = nullptr;
IMatSystemSurface* g_pMatSystemSurface = nullptr;
vgui::ISurface* g_pVGuiSurface = nullptr;
vgui::IInput* g_pVGuiInput = nullptr;
vgui::IVGui* g_pVGui = nullptr;
vgui::IPanel* g_pVGuiPanel = nullptr;
vgui::ILocalize* g_pVGuiLocalize = nullptr;
vgui::ISchemeManager* g_pVGuiSchemeManager = nullptr;
vgui::ISystem* g_pVGuiSystem = nullptr;
IDataCache* g_pDataCache = nullptr;
IMDLCache* g_pMDLCache = nullptr;
IMDLCache* mdlcache = nullptr;
IVideoServices* g_pVideo = nullptr;
IDmeMakefileUtils* g_pDmeMakefileUtils = nullptr;
IPhysicsCollision* g_pPhysicsCollision = nullptr;
ISoundEmitterSystemBase* g_pSoundEmitterSystem = nullptr;
IVTex* g_pVTex = nullptr;

static bool s_bConnected = false;

void ConnectTier3Libraries(CreateInterfaceFn* pFactoryList, int nFactoryCount)
{
	if (s_bConnected)
		return;

	for (int i = 0; i < nFactoryCount; ++i)
	{
		if (!g_pStudioRender)
			g_pStudioRender = studiorender = (IStudioRender*)pFactoryList[i](STUDIO_RENDER_INTERFACE_VERSION, NULL);
		if (!g_pMatSystemSurface)
			g_pMatSystemSurface = (IMatSystemSurface*)pFactoryList[i](MAT_SYSTEM_SURFACE_INTERFACE_VERSION, NULL);
		if (!g_pVGuiSurface)
			g_pVGuiSurface = (vgui::ISurface*)pFactoryList[i](VGUI_SURFACE_INTERFACE_VERSION, NULL);
		if (!g_pVGuiInput)
			g_pVGuiInput = (vgui::IInput*)pFactoryList[i](VGUI_INPUT_INTERFACE_VERSION, NULL);
		if (!g_pVGui)
			g_pVGui = (vgui::IVGui*)pFactoryList[i](VGUI_IVGUI_INTERFACE_VERSION, NULL);
		if (!g_pVGuiPanel)
			g_pVGuiPanel = (vgui::IPanel*)pFactoryList[i](VGUI_PANEL_INTERFACE_VERSION, NULL);
		if (!g_pVGuiLocalize)
			g_pVGuiLocalize = (vgui::ILocalize*)pFactoryList[i](VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
		if (!g_pVGuiSchemeManager)
			g_pVGuiSchemeManager = (vgui::ISchemeManager*)pFactoryList[i](VGUI_SCHEME_INTERFACE_VERSION, NULL);
		if (!g_pVGuiSystem)
			g_pVGuiSystem = (vgui::ISystem*)pFactoryList[i](VGUI_SYSTEM_INTERFACE_VERSION, NULL);
		if (!g_pDataCache)
			g_pDataCache = (IDataCache*)pFactoryList[i](DATACACHE_INTERFACE_VERSION, NULL);
		if (!g_pMDLCache)
			g_pMDLCache = mdlcache = (IMDLCache*)pFactoryList[i](MDLCACHE_INTERFACE_VERSION, NULL);
		if (!g_pVideo)
			g_pVideo = (IVideoServices*)pFactoryList[i](VIDEO_SERVICES_INTERFACE_VERSION, NULL);
		//if (!g_pDmeMakefileUtils)
		//	g_pDmeMakefileUtils = (IDmeMakefileUtils*)pFactoryList[i](???, NULL);
		if (!g_pPhysicsCollision)
			g_pPhysicsCollision = (IPhysicsCollision*)pFactoryList[i](VPHYSICS_COLLISION_INTERFACE_VERSION, NULL);
		if (!g_pSoundEmitterSystem)
			g_pSoundEmitterSystem = (ISoundEmitterSystemBase*)pFactoryList[i](SOUNDEMITTERSYSTEM_INTERFACE_VERSION, NULL);
		if (!g_pVTex)
			g_pVTex = (IVTex*)pFactoryList[i](IVTEX_VERSION_STRING, NULL);
	}
}

void DisconnectTier3Libraries()
{

}