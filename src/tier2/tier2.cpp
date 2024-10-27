#include "tier2/tier2.h"
#include <filesystem.h>
#include <materialsystem/imaterialsystem.h>
#include <materialsystem/idebugtextureinfo.h>
#include <materialsystem/ivballoctracker.h>
#include <inputsystem/iinputsystem.h>
#include <p4lib/ip4.h>
#include <mdllib/mdllib.h>
#include <filesystem/IQueuedLoader.h>

IFileSystem* g_pFullFileSystem = nullptr;
IMaterialSystem* materials = nullptr;
IMaterialSystem* g_pMaterialSystem = nullptr;
IInputSystem* g_pInputSystem = nullptr;
INetworkSystem* g_pNetworkSystem = nullptr;
IMaterialSystemHardwareConfig* g_pMaterialSystemHardwareConfig = nullptr;
IDebugTextureInfo* g_pMaterialSystemDebugTextureInfo = nullptr;
IVBAllocTracker* g_VBAllocTracker = nullptr;
IColorCorrectionSystem* colorcorrection = nullptr;
IP4* p4 = nullptr;
IMdlLib* mdllib = nullptr;
IQueuedLoader* g_pQueuedLoader = nullptr;

static bool s_bConnected = false;

void ConnectTier2Libraries(CreateInterfaceFn* pFactoryList, int nFactoryCount)
{
	if (s_bConnected)
		return;

	for (int i = 0; i < nFactoryCount; ++i)
	{
		if (!g_pFullFileSystem)
			g_pFullFileSystem = (IFileSystem*)pFactoryList[i](FILESYSTEM_INTERFACE_VERSION, NULL);
		if (!materials)
			materials = g_pMaterialSystem = (IMaterialSystem*)pFactoryList[i](MATERIAL_SYSTEM_INTERFACE_VERSION, NULL);
		if (!g_pInputSystem)
			g_pInputSystem = (IInputSystem*)pFactoryList[i](INPUTSYSTEM_INTERFACE_VERSION, NULL);
		//if (!g_pNetworkSystem)
		//	g_pNetworkSystem = (INetworkSystem*)pFactoryList[i](???, NULL);
		if (!g_pMaterialSystemHardwareConfig)
			g_pMaterialSystemHardwareConfig = (IMaterialSystemHardwareConfig*)pFactoryList[i](MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION, NULL);
		if (!g_pMaterialSystemDebugTextureInfo)
			g_pMaterialSystemDebugTextureInfo = (IDebugTextureInfo*)pFactoryList[i](DEBUG_TEXTURE_INFO_VERSION, NULL);
		if (!g_VBAllocTracker)
			g_VBAllocTracker = (IVBAllocTracker*)pFactoryList[i](VB_ALLOC_TRACKER_INTERFACE_VERSION, NULL);
		if (!colorcorrection)
			colorcorrection = (IColorCorrectionSystem*)pFactoryList[i](COLORCORRECTION_INTERFACE_VERSION, NULL);
		if (!p4)
			p4 = (IP4*)pFactoryList[i](P4_INTERFACE_VERSION, NULL);
		if (!mdllib)
			mdllib = (IMdlLib*)pFactoryList[i](MDLLIB_INTERFACE_VERSION, NULL);
		if (!g_pQueuedLoader)
			g_pQueuedLoader = (IQueuedLoader*)pFactoryList[i](QUEUEDLOADER_INTERFACE_VERSION, NULL);
	}

	s_bConnected = true;
}


void DisconnectTier2Libraries()
{
	if (!s_bConnected)
		return;

	g_pFullFileSystem = nullptr;
	materials = nullptr;
	g_pMaterialSystem = nullptr;
	g_pInputSystem = nullptr;
	g_pNetworkSystem = nullptr;
	g_pMaterialSystemHardwareConfig = nullptr;
	g_pMaterialSystemDebugTextureInfo = nullptr;
	g_VBAllocTracker = nullptr;
	colorcorrection = nullptr;
	p4 = nullptr;
	mdllib = nullptr;
	g_pQueuedLoader = nullptr;

	s_bConnected = false;
}