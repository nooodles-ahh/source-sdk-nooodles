#include "tier2/fileutils.h"
#include "filesystem.h"
#include "platform.h"

// Builds a directory which is a subdirectory of the current mod
void GetModSubdirectory(const char* pSubDir, char* pBuf, int nBufLen)
{
    int found = g_pFullFileSystem->GetSearchPath("MOD_WRITE", 0, pBuf, nBufLen);
    if (!found) {
        g_pFullFileSystem->GetSearchPath("MOD", 0, pBuf, nBufLen);
    }

    char* searchPathEnd = strchr(pBuf, ';');
    if (searchPathEnd) {
        *searchPathEnd = '\0';
    }

    V_StripTrailingSlash(pBuf);
	V_FixSlashes(pBuf);
    if (pSubDir) {
		int subDirLen = V_strlen(pSubDir);
        V_strncat(pBuf, "\\", nBufLen, 1);
        V_strncat(pBuf, pSubDir, nBufLen, subDirLen);
    }
    V_FixSlashes(pBuf);
    return;
}

// Builds a directory which is a subdirectory of the current mod's *content*
void GetModContentSubdirectory(const char* pSubDir, char* pBuf, int nBufLen)
{
}

// Generates a filename under the 'game' subdirectory given a subdirectory of 'content'
void ComputeModFilename(const char* pContentFileName, char* pBuf, size_t nBufLen)
{
}

// Generates a filename under the 'content' subdirectory given a subdirectory of 'game'
void ComputeModContentFilename(const char* pGameFileName, char* pBuf, size_t nBufLen)
{
}

// Builds a list of all files under a directory with a particular extension
void AddFilesToList(CUtlVector< CUtlString >& list, const char* pDirectory, const char* pPath, const char* pExtension)
{
}

// Returns the search path as a list of paths
void GetSearchPath(CUtlVector< CUtlString >& path, const char* pPathID)
{
}

// Given file name generate a full path using the following rules.
// 1. if its full path already return
// 2. if its a relative path try to find it under the path id
// 3. if find fails treat relative path as relative to the current dir
bool GenerateFullPath(const char* pFileName, char const* pPathID, char* pBuf, int nBufLen)
{
	return false;
}

const char* RestoreFilename(const char* pSourceName, char* pTargetName, int targetLen)
{
    char cVar1;
    size_t sourceLen;
    size_t sVar2;
    int iVar3;

    sourceLen = V_strlen(pSourceName);
    do {
        sVar2 = sourceLen;
        sourceLen = sVar2 - 1;
        if ((int)sourceLen < 1) {
            return pSourceName;
        }
        cVar1 = pSourceName[sVar2 - 1];
    } while ((1 < (byte)(cVar1 - '.')) && (cVar1 != '\\'));
    if ((3 < (int)sourceLen) &&
        ((cVar1 == '.' && (iVar3 = V_strncmp(pSourceName + (sVar2 - 5), ".360", 4), iVar3 == 0)))) {
        iVar3 = sVar2 - 4;
        if (targetLen < (int)(sVar2 - 4)) {
            iVar3 = targetLen;
        }
        V_strncpy(pTargetName, pSourceName, iVar3);
        V_strncat(pTargetName, pSourceName + sourceLen, targetLen, -1);
        pSourceName = pTargetName;
    }
    return pSourceName;
}


int UpdateOrCreate(const char* pSourceName, char* pTargetName, int targetLen, const char* pPathID, CreateCallback_t pfnCreate, bool bForce, void* pExtraData)
{
    char szFixedSourceName[260];
    if (pTargetName) 
    {
        const char* fileName = RestoreFilename(pSourceName, szFixedSourceName, 260);
        V_StripExtension(fileName, pTargetName, targetLen);
		int fileNameLen = V_strlen(pTargetName);
        V_snprintf(pTargetName, targetLen, "%s.360%s", pTargetName, fileName + fileNameLen);
    }
    return 0;
}