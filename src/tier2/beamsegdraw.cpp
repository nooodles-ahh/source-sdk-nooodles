#include "tier2/beamsegdraw.h"
#include "engine/ivmodelinfo.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imaterialvar.h"
#include "tier1/tier1.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar r_drawsprites;
extern ConVar r_DrawBeams;

static IMaterial* g_pBeamWireframeMaterial;

extern ConVar mat_wireframe;
extern const ConVar* sv_cheats;

static inline int WireFrameMode(void)
{
	if (!sv_cheats)
	{
		sv_cheats = cvar->FindVar("sv_cheats");
	}

	if (sv_cheats && sv_cheats->GetBool())
		return mat_wireframe.GetInt();
	else
		return 0;
}

static inline bool ShouldDrawInWireFrameMode(void)
{
	if (!sv_cheats)
	{
		sv_cheats = cvar->FindVar("sv_cheats");
	}

	if (sv_cheats && sv_cheats->GetBool())
		return (mat_wireframe.GetInt() != 0);
	else
		return false;
}

// ------------------------------------------------------------------------------------------ //
// CBeamSegDraw implementation.
// ------------------------------------------------------------------------------------------ //
void CBeamSegDraw::Start(IMatRenderContext* pRenderContext, int nSegs, IMaterial* pMaterial, CMeshBuilder* pMeshBuilder, int nMeshVertCount)
{
	Assert(nSegs >= 2);

	m_pRenderContext = pRenderContext;
	m_nSegsDrawn = 0;
	m_nTotalSegs = nSegs;

	if (pMeshBuilder)
	{
		m_pMeshBuilder = pMeshBuilder;
		m_nMeshVertCount = nMeshVertCount;
	}
	else
	{
		m_pMeshBuilder = NULL;
		m_nMeshVertCount = 0;

		if (ShouldDrawInWireFrameMode() || r_DrawBeams.GetInt() == 2)
		{
			if (!g_pBeamWireframeMaterial)
				g_pBeamWireframeMaterial = materials->FindMaterial("shadertest/wireframevertexcolor", TEXTURE_GROUP_OTHER);
			pMaterial = g_pBeamWireframeMaterial;
		}
		;
		IMesh* pMesh = pRenderContext->GetDynamicMesh(true, NULL, NULL, pMaterial);
		m_Mesh.Begin(pMesh, MATERIAL_TRIANGLE_STRIP, (nSegs - 1) * 2);
	}
}

inline void CBeamSegDraw::ComputeNormal(const Vector& vecCameraPos, const Vector& vStartPos, const Vector& vNextPos, Vector* pNormal)
{
	// vTangentY = line vector for beam
	Vector vTangentY;
	VectorSubtract(vStartPos, vNextPos, vTangentY);

	// vDirToBeam = vector from viewer origin to beam
	Vector vDirToBeam;
	VectorSubtract(vStartPos, vecCameraPos, vDirToBeam);

	// Get a vector that is perpendicular to us and perpendicular to the beam.
	// This is used to fatten the beam.
	CrossProduct(vTangentY, vDirToBeam, *pNormal);
	VectorNormalizeFast(*pNormal);
}

inline void CBeamSegDraw::SpecifySeg(const Vector& vecCameraPos, const Vector& vNormal)
{
	// SUCKY: Need to do a fair amount more work to get the tangent owing to the averaged normal
	Vector vDirToBeam, vTangentY;
	VectorSubtract(m_Seg.m_vPos, vecCameraPos, vDirToBeam);
	CrossProduct(vDirToBeam, vNormal, vTangentY);
	VectorNormalizeFast(vTangentY);

	// Build the endpoints.
	Vector vPoint1, vPoint2;
	VectorMA(m_Seg.m_vPos, m_Seg.m_flWidth * 0.5f, vNormal, vPoint1);
	VectorMA(m_Seg.m_vPos, -m_Seg.m_flWidth * 0.5f, vNormal, vPoint2);

	if (m_pMeshBuilder)
	{
		// Specify the points.
		m_pMeshBuilder->Position3fv(vPoint1.Base());
		m_pMeshBuilder->Color4f(VectorExpand(m_Seg.m_vColor), m_Seg.m_flAlpha);
		m_pMeshBuilder->TexCoord2f(0, 0, m_Seg.m_flTexCoord);
		m_pMeshBuilder->TexCoord2f(1, 0, m_Seg.m_flTexCoord);
		m_pMeshBuilder->TangentS3fv(vNormal.Base());
		m_pMeshBuilder->TangentT3fv(vTangentY.Base());
		m_pMeshBuilder->AdvanceVertex();

		m_pMeshBuilder->Position3fv(vPoint2.Base());
		m_pMeshBuilder->Color4f(VectorExpand(m_Seg.m_vColor), m_Seg.m_flAlpha);
		m_pMeshBuilder->TexCoord2f(0, 1, m_Seg.m_flTexCoord);
		m_pMeshBuilder->TexCoord2f(1, 1, m_Seg.m_flTexCoord);
		m_pMeshBuilder->TangentS3fv(vNormal.Base());
		m_pMeshBuilder->TangentT3fv(vTangentY.Base());
		m_pMeshBuilder->AdvanceVertex();

		if (m_nSegsDrawn > 1)
		{
			int nBase = ((m_nSegsDrawn - 2) * 2) + m_nMeshVertCount;

			m_pMeshBuilder->FastIndex(nBase);
			m_pMeshBuilder->FastIndex(nBase + 1);
			m_pMeshBuilder->FastIndex(nBase + 2);
			m_pMeshBuilder->FastIndex(nBase + 1);
			m_pMeshBuilder->FastIndex(nBase + 3);
			m_pMeshBuilder->FastIndex(nBase + 2);
		}
	}
	else
	{
		// Specify the points.
		m_Mesh.Position3fv(vPoint1.Base());
		m_Mesh.Color4f(VectorExpand(m_Seg.m_vColor), m_Seg.m_flAlpha);
		m_Mesh.TexCoord2f(0, 0, m_Seg.m_flTexCoord);
		m_Mesh.TexCoord2f(1, 0, m_Seg.m_flTexCoord);
		m_Mesh.TangentS3fv(vNormal.Base());
		m_Mesh.TangentT3fv(vTangentY.Base());
		m_Mesh.AdvanceVertex();

		m_Mesh.Position3fv(vPoint2.Base());
		m_Mesh.Color4f(VectorExpand(m_Seg.m_vColor), m_Seg.m_flAlpha);
		m_Mesh.TexCoord2f(0, 1, m_Seg.m_flTexCoord);
		m_Mesh.TexCoord2f(1, 1, m_Seg.m_flTexCoord);
		m_Mesh.TangentS3fv(vNormal.Base());
		m_Mesh.TangentT3fv(vTangentY.Base());
		m_Mesh.AdvanceVertex();
	}
}

void CBeamSegDraw::NextSeg(BeamSeg_t* pSeg)
{
	Vector camPos;
	m_pRenderContext->GetWorldSpaceCameraPosition(&camPos);
	if (m_nSegsDrawn > 0)
	{
		// Get a vector that is perpendicular to us and perpendicular to the beam.
		// This is used to fatten the beam.
		Vector vNormal, vAveNormal;
		ComputeNormal(camPos, m_Seg.m_vPos, pSeg->m_vPos, &vNormal);

		if (m_nSegsDrawn > 1)
		{
			// Average this with the previous normal
			VectorAdd(vNormal, m_vNormalLast, vAveNormal);
			vAveNormal *= 0.5f;
			VectorNormalizeFast(vAveNormal);
		}
		else
		{
			vAveNormal = vNormal;
		}

		m_vNormalLast = vNormal;
		SpecifySeg(camPos, vAveNormal);
	}

	m_Seg = *pSeg;
	++m_nSegsDrawn;

	if (m_nSegsDrawn == m_nTotalSegs)
	{
		SpecifySeg(camPos, m_vNormalLast);
	}
}

void CBeamSegDraw::End()
{
	if (m_pMeshBuilder)
	{
		m_pMeshBuilder = NULL;
		return;
	}

	m_Mesh.End(false, true);
}