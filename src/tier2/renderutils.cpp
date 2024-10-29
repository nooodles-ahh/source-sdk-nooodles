#include "tier2/renderutils.h"
#include "tier1/keyvalues.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imaterial.h"
#include <materialsystem/imesh.h>

bool s_bMaterialsInitialized = false;
IMaterial* s_pWireframe = nullptr;
IMaterial* s_pWireframeIgnoreZ = nullptr;
IMaterial* s_pVertexColor = nullptr;
IMaterial* s_pVertexColorIgnoreZ = nullptr;

void InitializeStandardMaterials(void)
{
    if (s_bMaterialsInitialized)
        return;

    s_bMaterialsInitialized = true;
	KeyValues* pKVMaterial = new KeyValues("wireframe");
    pKVMaterial->SetInt("$vertexcolor", 1);
    s_pWireframe = g_pMaterialSystem->CreateMaterial("__utilWireframe", pKVMaterial);
	s_pWireframe->IncrementReferenceCount();

	pKVMaterial = new KeyValues("wireframe");
    pKVMaterial->SetInt("$vertexcolor", 1);
    pKVMaterial->SetInt("$vertexalpha", 1);
    pKVMaterial->SetInt("$ignorez", 1);
    s_pWireframeIgnoreZ = g_pMaterialSystem->CreateMaterial("__utilWireframeIgnoreZ", pKVMaterial);\
	s_pWireframeIgnoreZ->IncrementReferenceCount();

    pKVMaterial = new KeyValues("unlitgeneric");
    pKVMaterial->SetInt("$vertexcolor", 1);
    pKVMaterial->SetInt("$vertexalpha", 1);
	s_pVertexColor = g_pMaterialSystem->CreateMaterial("__utilVertexColor", pKVMaterial);
	s_pVertexColor->IncrementReferenceCount();

    pKVMaterial = new KeyValues("unlitgeneric");
    pKVMaterial->SetInt("$vertexcolor", 1);
    pKVMaterial->SetInt("$vertexalpha", 1);
    pKVMaterial->SetInt("$ignorez", 1);
    s_pVertexColorIgnoreZ = g_pMaterialSystem->CreateMaterial("__utilVertexColorIgnoreZ", pKVMaterial);
	s_pVertexColorIgnoreZ->IncrementReferenceCount();
    return;
}

void ShutdownStandardMaterials(void)
{
	if (!s_bMaterialsInitialized)
		return;

	s_bMaterialsInitialized = false;
	s_pWireframe->DecrementReferenceCount();
	s_pWireframe = nullptr;
	s_pWireframeIgnoreZ->DecrementReferenceCount();
	s_pWireframeIgnoreZ = nullptr;
	s_pVertexColor->DecrementReferenceCount();
	s_pVertexColor = nullptr;
	s_pVertexColorIgnoreZ->DecrementReferenceCount();
	s_pVertexColorIgnoreZ = nullptr;
}

void DrawAxes(Vector* origin, Vector* pts, int idx, Color c, CMeshBuilder* meshBuilder)
{
	int index = idx;
	Vector _v1 = Vector(origin->x + pts[index].x, pts[index].y + origin->y, pts[index].z + origin->z);
	
	meshBuilder->Position3fv(_v1.Base());
	meshBuilder->Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder->AdvanceVertex();
	
	if ((idx & 1U) == 0)
		index = idx + 1;
	else
		index = idx - 1;

	meshBuilder->Position3f(origin->x + pts[index].x,
		pts[index].y + origin->y,
		pts[index].z + origin->z);
	meshBuilder->Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder->AdvanceVertex();

	meshBuilder->Position3fv(_v1.Base());
	meshBuilder->Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder->AdvanceVertex();

	if ((idx & 2U) == 0)
		index = idx + 2;
	else
		index = idx + -2;

	meshBuilder->Position3f(origin->x + pts[index].x,
		pts[index].y + origin->y,
		pts[index].z + origin->z);
	meshBuilder->Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder->AdvanceVertex();

	meshBuilder->Position3fv(_v1.Base());
	meshBuilder->Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder->AdvanceVertex();

	if ((idx & 4U) == 0)
		index = idx + 4;
	else
		index = idx + -4;

	meshBuilder->Position3f(origin->x + pts[index].x,
		pts[index].y + origin->y,
		pts[index].z + origin->z);
	meshBuilder->Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder->AdvanceVertex();
	return;
}


// Renders a wireframe sphere
void RenderWireframeSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer)
{

}

// Renders a sphere
void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, IMaterial* pMaterial)
{

}

void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer)
{
	IMaterial* pMaterial = bZBuffer ? s_pVertexColor : s_pVertexColorIgnoreZ;
	RenderSphere(vCenter, flRadius, nTheta, nPhi, c, pMaterial);
}

// Renders a wireframe box relative to an origin 
void RenderWireframeBox(const Vector& vOrigin, const QAngle& angles,
	const Vector& vMins, const Vector& vMaxs, Color c, bool bZBuffer)
{
	InitializeStandardMaterials();
	CMatRenderContextPtr pRenderContext(materials);

	pRenderContext->BeginRender();
	pRenderContext->Bind(bZBuffer ? s_pVertexColor : s_pVertexColorIgnoreZ);

	IMesh* pMesh = pRenderContext->GetDynamicMesh();

	CMeshBuilder meshBuilder;

	// The four sides
	meshBuilder.Begin(pMesh, MATERIAL_LINES, 4);
	for (int i = 0; i < 10; i++)
	{
		meshBuilder.Position3fv(v[i & 7].Base());
		meshBuilder.Color4fv(color);
		meshBuilder.AdvanceVertex();
	}
	meshBuilder.End();
	pMesh->Draw();

	// top and bottom
	meshBuilder.Begin(pMesh, MATERIAL_LINE_STRIP, 4);

	meshBuilder.Position3fv(v[6].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[0].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[2].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[4].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[6].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.End();
	pMesh->Draw();

	meshBuilder.Begin(pMesh, MATERIAL_LINE_STRIP, 4);

	meshBuilder.Position3fv(v[1].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[7].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[5].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[3].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[1].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.End();
	pMesh->Draw();
}

// Renders a swept wireframe box
void RenderWireframeSweptBox(const Vector& vStart, const Vector& vEnd, const QAngle& angles,
	const Vector& vMins, const Vector& vMaxs, Color c, bool bZBuffer)
{

}

// Renders a solid box 
void RenderBox(const Vector& origin, const QAngle& angles, const Vector& mins, const Vector& maxs, 
	Color c, IMaterial* pMaterial, bool bInsideOut /*= false*/)
{
	InitializeStandardMaterials();
	CMatRenderContextPtr pRenderContext(materials);

	pRenderContext->BeginRender();
	pRenderContext->Bind(pMaterial);

	IMesh* pMesh = pRenderContext->GetDynamicMesh();

	CMeshBuilder meshBuilder;

	// The four sides
	meshBuilder.Begin(pMesh, MATERIAL_TRIANGLE_STRIP, 2 * 4);
	for (int i = 0; i < 10; i++)
	{
		meshBuilder.Position3fv(v[i & 7].Base());
		meshBuilder.Color4fv(color);
		meshBuilder.AdvanceVertex();
	}
	meshBuilder.End();
	pMesh->Draw();

	// top and bottom
	meshBuilder.Begin(pMesh, MATERIAL_TRIANGLE_STRIP, 2);

	meshBuilder.Position3fv(v[6].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[0].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[4].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[2].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.End();
	pMesh->Draw();

	meshBuilder.Begin(pMesh, MATERIAL_TRIANGLE_STRIP, 2);

	meshBuilder.Position3fv(v[1].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[7].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[3].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(v[5].Base());
	meshBuilder.Color4fv(color);
	meshBuilder.AdvanceVertex();

	meshBuilder.End();
	pMesh->Draw();

}

void RenderBox(const Vector& origin, const QAngle& angles, const Vector& mins, const Vector& maxs,
	Color c, bool bZBuffer, bool bInsideOut /*= false*/)
{
	IMaterial* pMaterial = bZBuffer ? s_pVertexColor : s_pVertexColorIgnoreZ;
	RenderBox(origin, angles, mins, maxs, c, pMaterial, bInsideOut);
}

// Renders axes, red->x, green->y, blue->z (axis aligned)
void RenderAxes(const Vector& vOrigin, float flScale, bool bZBuffer)
{

}

void RenderAxes(const matrix3x4_t& transform, float flScale, bool bZBuffer)
{

}

// Render a line
void RenderLine(const Vector& v1, const Vector& v2, Color c, bool bZBuffer)
{
	InitializeStandardMaterials();
	CMatRenderContextPtr pRenderContext(materials);
	
	pRenderContext->BeginRender();
	pRenderContext->Bind(bZBuffer ? s_pVertexColor : s_pVertexColorIgnoreZ);

	IMesh* pMesh = pRenderContext->GetDynamicMesh();
	CMeshBuilder meshBuilder;
	meshBuilder.Begin(pMesh, MATERIAL_LINES, 1);

	meshBuilder.Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder.Position3fv(v1.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ub(c.r(), c.g(), c.b(), c.a());
	meshBuilder.Position3fv(v2.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.End();
	pMesh->Draw();
}

// Draws a triangle
void RenderTriangle(const Vector& p1, const Vector& p2, const Vector& p3, Color c, IMaterial* pMaterial)
{

}

void RenderTriangle(const Vector& p1, const Vector& p2, const Vector& p3, Color c, bool bZBuffer)
{
	IMaterial* pMaterial = bZBuffer ? s_pVertexColor : s_pVertexColorIgnoreZ;
	RenderTriangle(p1, p2, p3, c, pMaterial);
}

// Draws a axis-aligned quad
void RenderQuad(IMaterial* pMaterial, float x, float y, float w, float h, float z, float s0, float t0, float s1, float t1, const Color& clr)
{

}

// Renders a screen space quad
void DrawScreenSpaceRectangle(IMaterial* pMaterial,
	int nDestX, int nDestY, int nWidth, int nHeight,	// Rect to draw into in screen space
	float flSrcTextureX0, float flSrcTextureY0,			// which texel you want to appear at destx/y
	float flSrcTextureX1, float flSrcTextureY1,			// which texel you want to appear at destx+width-1, desty+height-1
	int nSrcTextureWidth, int nSrcTextureHeight,		// needed for fixup
	void* pClientRenderable /*= NULL*/,						// Used to pass to the bind proxies
	int nXDice /*= 1*/,
	int nYDice /*= 1*/,
	float fDepth /*= 0.0f*/)								// what Z value to put in the verts
{

}