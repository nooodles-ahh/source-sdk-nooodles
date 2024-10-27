#include "tier2/meshutils.h"
#include "basetypes.h"

void GenerateSequentialIndexBuffer(unsigned short* pIndexMemory, int nIndexCount, int nFirstVertex)
{
    int idx;

	if (pIndexMemory && (0 < nIndexCount)) {
		idx = 0;
		do {
			pIndexMemory[idx] = (uint16)nFirstVertex;
			idx++;
			nFirstVertex++;
		} while (idx != nIndexCount);
	}
	return;
}

void GenerateQuadIndexBuffer(unsigned short* pIndexMemory, int nIndexCount, int nFirstVertex)
{
    int idx;

	if (pIndexMemory && (0 < nIndexCount / 6)) 
	{
		idx = 0;
        do {
			pIndexMemory[0] = (uint16)nFirstVertex;
			pIndexMemory[1] = (uint16)nFirstVertex + 1;
			pIndexMemory[2] = (uint16)nFirstVertex + 2;
			pIndexMemory[3] = (uint16)nFirstVertex;
			pIndexMemory[4] = (uint16)nFirstVertex + 2;
			pIndexMemory[5] = (uint16)nFirstVertex + 3;
			pIndexMemory += 6;

			idx++;
			nFirstVertex += 4;
        } while (idx != nIndexCount / 6);
    }
    return;
}

void GeneratePolygonIndexBuffer(unsigned short* pIndexMemory, int nIndexCount, int nFirstVertex)
{
	uint16 uVar1;
	int idx;

	if (pIndexMemory && (0 < nIndexCount / 3)) {
		uVar1 = (uint16)nFirstVertex + 1;
		idx = 0;
		do {
			*pIndexMemory = (uint16)nFirstVertex;
			pIndexMemory[1] = uVar1;
			uVar1 = uVar1 + 1;
			pIndexMemory[2] = uVar1;
			pIndexMemory = pIndexMemory + 3;
			idx = idx + 1;
		} while (idx != nIndexCount / 3);
	}
	return;
}

void GenerateLineStripIndexBuffer(unsigned short* pIndexMemory, int nIndexCount, int nFirstVertex)
{
	int idx;

	if (pIndexMemory && (0 < nIndexCount / 2)) {
		idx = 0;
		do {
			*pIndexMemory = (uint16)nFirstVertex;
			nFirstVertex = (uint16)nFirstVertex + 1;
			pIndexMemory[1] = (uint16)nFirstVertex;
			pIndexMemory = pIndexMemory + 2;
			idx = idx + 1;
		} while (idx != nIndexCount / 2);
	}
	return;
}

void GenerateLineLoopIndexBuffer(unsigned short* pIndexMemory, int nIndexCount, int nFirstVertex)
{
	int iVar1;
	int iVar2;

	if (pIndexMemory) {
		iVar2 = nIndexCount / 2;
		*pIndexMemory = ((uint16)nFirstVertex - 1) + (short)iVar2;
		pIndexMemory[1] = (uint16)nFirstVertex;
		if (1 < iVar2) {
			iVar1 = 1;
			do {
				pIndexMemory[iVar1 * 2] = (uint16)nFirstVertex;
				pIndexMemory[iVar1 * 2 + 1] = (uint16)nFirstVertex + 1;
				iVar1 = iVar1 + 1;
				nFirstVertex = (uint16)nFirstVertex + 1;
			} while (iVar1 != iVar2);
		}
	}
	return;
}
