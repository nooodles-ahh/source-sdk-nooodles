//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <stdio.h>

#include <vgui/IBorder.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/IBorder.h>
#include <KeyValues.h>

#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Image.h>
#include <vgui_controls/Controls.h>
#ifdef VGUI_ENHANCEMENTS
#include "vgui/ILocalize.h"
#include "bitmap/TGALoader.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

#ifdef VGUI_ENHANCEMENTS
class TGAImage : public IImage
{
public:
	TGAImage(const char * pszTGA, const unsigned char* rgba, int wide, int tall )
	{
		m_nX = m_nY = 0;
		m_TGAwide = m_wide = wide;
		m_TGAtall = m_tall = tall;
		m_iTextureID = surface()->CreateNewTextureID(true);
		surface()->DrawSetTextureRGBA( m_iTextureID, rgba, m_TGAwide, m_TGAtall, true, false );
		m_Color = Color( 255, 255, 255, 255 );
	}

	~TGAImage()
	{
		surface()->DestroyTextureID( m_iTextureID );
	}

	// Call to Paint the image
	// Image will draw within the current panel context at the specified position
	virtual void Paint()
	{
		surface()->DrawSetTexture( m_iTextureID );
		int w, t;
		surface()->DrawGetTextureSize(m_iTextureID, w, t );
		surface()->DrawSetColor( 255, 255, 255, 255 );

		// need create texture coords to match the aspect ratio of the image
		float texs1 = (float)m_TGAwide / (float)w;
		float text1 = (float)m_TGAtall / (float)t;

		const float flHalfPixelX = ( 0.5f / 180.0f );
		const float flHalfPixelY = ( 0.5f / 100.0f );

		surface()->DrawTexturedSubRect(
			0, 0, m_wide, m_tall,
			0.0f, 0.0f, texs1 - flHalfPixelX, text1 - flHalfPixelY );
	}

	// Set the position of the image
	virtual void SetPos( int x, int y )
	{
		m_nX = x;
		m_nY = y;
	}

	// Gets the size of the content
	virtual void GetContentSize( int& wide, int& tall )
	{
		wide = m_TGAwide;
		tall = m_TGAtall;
	}

	// Get the size the image will actually draw in (usually defaults to the content size)
	virtual void GetSize( int& wide, int& tall )
	{
		GetContentSize( wide, tall );
	}

	// Sets the size of the image
	virtual void SetSize( int wide, int tall )
	{
		m_wide = wide;
		m_tall = tall;
	}

	// Set the draw color 
	virtual void SetColor( Color col )
	{}
	
	// not for general purpose use
	// evicts the underlying image from memory if refcounts permit, otherwise ignored
	// returns true if eviction occurred, otherwise false
	virtual bool Evict()
	{
		return false;
	}

	virtual int GetNumFrames()
	{
		return 0;
	}
	virtual void SetFrame( int nFrame ) 
	{
		return;
	}

	virtual HTexture GetID()
	{
		return 0;
	}

	virtual void SetRotation( int iRotation )
	{
		return;
	}

private:
	Color m_Color;
	int m_iTextureID;
	int m_nX, m_nY;
	int m_TGAwide, m_TGAtall;
	int m_wide, m_tall;
};

TGAImage* GetTGAImage(const char *pszTGAName)
{
	CUtlMemory<unsigned char> tga;
	int w, h;
	if ( TGALoader::LoadRGBA8888( pszTGAName, tga, w, h ) )
	{
		return new TGAImage( pszTGAName, tga.Base(), w, h );
	}
	return nullptr;
}
#endif

DECLARE_BUILD_FACTORY( ImagePanel );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImagePanel::ImagePanel(Panel *parent, const char *name) : Panel(parent, name)
{
	m_pImage = NULL;
	m_pszImageName = NULL;
	m_pszFillColorName = NULL;
	m_pszDrawColorName = NULL;	// HPE addition
	m_bCenterImage = false;
	m_bScaleImage = false;
	m_bTileImage = false;
	m_bTileHorizontally = false;
	m_bTileVertically = false;
	m_bPositionImage = true;
	m_fScaleAmount = 0.0f;
	m_FillColor = Color(0, 0, 0, 0);
	m_DrawColor = Color(255,255,255,255);
	m_iRotation = ROTATED_UNROTATED;
#ifdef VGUI_ENHANCEMENTS
	m_bIsTGA = false;
	m_bWaitingForDialogVar = false;
#endif

	SetImage( m_pImage );

	REGISTER_COLOR_AS_OVERRIDABLE( m_FillColor, "fillcolor_override" );
	REGISTER_COLOR_AS_OVERRIDABLE( m_DrawColor, "drawcolor_override" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImagePanel::~ImagePanel()
{
	delete [] m_pszImageName;
	delete [] m_pszFillColorName;
	delete [] m_pszDrawColorName;	// HPE addition
}

//-----------------------------------------------------------------------------
// Purpose: handles size changing
//-----------------------------------------------------------------------------
void ImagePanel::OnSizeChanged(int newWide, int newTall)
{
	BaseClass::OnSizeChanged(newWide, newTall);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImagePanel::SetImage(IImage *image)
{
	m_pImage = image;
	Repaint();
}

//-----------------------------------------------------------------------------
// Purpose: sets an image by file name
//-----------------------------------------------------------------------------
void ImagePanel::SetImage(const char *imageName)
{
	if ( imageName && m_pszImageName && V_stricmp( imageName, m_pszImageName ) == 0 )
		return;

	int len = Q_strlen(imageName) + 1;
	delete [] m_pszImageName;
	m_pszImageName = new char[ len ];
	Q_strncpy(m_pszImageName, imageName, len );
#ifdef VGUI_ENHANCEMENTS
	m_bIsTGA = V_stristr( imageName, ".tga" ) != nullptr;

	if( !m_bWaitingForDialogVar )
#endif
	InvalidateLayout(false, true); // force applyschemesettings to run
}

#ifdef VGUI_ENHANCEMENTS
// we don't set text but we want to be able to handle SetControlString
void ImagePanel::OnSetText( KeyValues* params )
{
	KeyValues* pkvText = params->FindKey( "text", false );
	if ( !pkvText )
		return;

	if ( pkvText->GetDataType() == KeyValues::TYPE_STRING )
	{
		SetImage( pkvText->GetString() );
	}
	else if ( pkvText->GetDataType() == KeyValues::TYPE_WSTRING )
	{
		Assert( !"ImagePanel::OnSetText:  WSTRING not supported" );
		DevMsg( "ImagePanel::OnSetText:  WSTRING not supported\n" );
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
IImage *ImagePanel::GetImage()
{
	return m_pImage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Color ImagePanel::GetDrawColor( void )
{
	return m_DrawColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImagePanel::SetDrawColor( Color drawColor )
{
	m_DrawColor = drawColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImagePanel::PaintBackground()
{
	if (m_FillColor[3] > 0)
	{
		// draw the specified fill color
		int wide, tall;
		GetSize(wide, tall);
		surface()->DrawSetColor(m_FillColor);
		surface()->DrawFilledRect(0, 0, wide, tall);
	}
	if ( m_pImage )
	{
		//=============================================================================
		// HPE_BEGIN:
		// [pfreese] Color should be always set from GetDrawColor(), not just when 
		// scaling is true (see previous code)
		//=============================================================================
		
		// surface()->DrawSetColor( 255, 255, 255, GetAlpha() );
		m_pImage->SetColor( GetDrawColor() );
		
		//=============================================================================
		// HPE_END
		//=============================================================================

		if ( m_bPositionImage )
		{
			if ( m_bCenterImage )
			{
				int wide, tall;
				GetSize(wide, tall);

				int imageWide, imageTall;
				m_pImage->GetSize( imageWide, imageTall );

				if ( m_bScaleImage && m_fScaleAmount > 0.0f )
				{
					imageWide = static_cast<int>( static_cast<float>(imageWide) * m_fScaleAmount );
					imageTall = static_cast<int>( static_cast<float>(imageTall) * m_fScaleAmount );
				}

				m_pImage->SetPos( (wide - imageWide) / 2, (tall - imageTall) / 2 );
			}
			else
			{
				m_pImage->SetPos(0, 0);
			}
		}

		if (m_bScaleImage)
		{
			// Image size is stored in the bitmap, so temporarily set its size
			// to our panel size and then restore after we draw it.

			int imageWide, imageTall;
			m_pImage->GetSize( imageWide, imageTall );

			if ( m_fScaleAmount > 0.0f )
			{
				float wide, tall;
				wide = static_cast<float>(imageWide) * m_fScaleAmount;
				tall = static_cast<float>(imageTall) * m_fScaleAmount;
				m_pImage->SetSize( static_cast<int>(wide), static_cast<int>(tall) );
			}
			else
			{
				int wide, tall;
				GetSize( wide, tall );
				m_pImage->SetSize( wide, tall );
			}

			m_pImage->Paint();

			m_pImage->SetSize( imageWide, imageTall );
		}
		else if ( m_bTileImage || m_bTileHorizontally || m_bTileVertically )
		{
			int wide, tall;
			GetSize(wide, tall);
			int imageWide, imageTall;
			m_pImage->GetSize( imageWide, imageTall );

			int y = 0;
			while ( y < tall )
			{
				int x = 0;
				while (x < wide)
				{
					m_pImage->SetPos(x,y);
					m_pImage->Paint();

					x += imageWide;

					if ( !m_bTileHorizontally )
						break;
				}

				y += imageTall;

				if ( !m_bTileVertically )
					break;
			}

			if ( m_bPositionImage )
			{
				m_pImage->SetPos(0, 0);
			}
		}
		else
		{
			m_pImage->SetColor( GetDrawColor() );
			m_pImage->Paint();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets control settings for editing
//-----------------------------------------------------------------------------
void ImagePanel::GetSettings(KeyValues *outResourceData)
{
	BaseClass::GetSettings(outResourceData);
	if (m_pszImageName)
	{
		outResourceData->SetString("image", m_pszImageName);
	}
	if (m_pszFillColorName)
	{
		outResourceData->SetString("fillcolor", m_pszFillColorName);
	}

	//=============================================================================
	// HPE_BEGIN:
	// [pfreese] Added support for specifying drawcolor
	//=============================================================================
	if (m_pszDrawColorName)
	{
		outResourceData->SetString("drawcolor", m_pszDrawColorName);
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	if (GetBorder())
	{
		outResourceData->SetString("border", GetBorder()->GetName());
	}

	outResourceData->GetInt("positionImage", m_bPositionImage );
	outResourceData->SetInt("scaleImage", m_bScaleImage);
	outResourceData->SetFloat("scaleAmount", m_fScaleAmount);
	outResourceData->SetInt("tileImage", m_bTileImage);
	outResourceData->SetInt("tileHorizontally", m_bTileHorizontally);
	outResourceData->SetInt("tileVertically", m_bTileVertically);
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void ImagePanel::ApplySettings(KeyValues *inResourceData)
{
	delete [] m_pszImageName;
	delete [] m_pszFillColorName;
	delete [] m_pszDrawColorName;	// HPE addition
	m_pszImageName = NULL;
	m_pszFillColorName = NULL;
	m_pszDrawColorName = NULL;		// HPE addition

	m_bPositionImage = inResourceData->GetInt("positionImage", 1);
	m_bScaleImage = inResourceData->GetInt("scaleImage", 0);
	m_fScaleAmount = inResourceData->GetFloat("scaleAmount", 0.0f);
	m_bTileImage = inResourceData->GetInt("tileImage", 0);
	m_bTileHorizontally = inResourceData->GetInt("tileHorizontally", m_bTileImage);
	m_bTileVertically = inResourceData->GetInt("tileVertically", m_bTileImage);
	const char *imageName = inResourceData->GetString("image", "");
#ifdef VGUI_ENHANCEMENTS
	if ( *imageName )
	{
		if ( imageName[ 0 ] == '%' && imageName[ strlen( imageName ) - 1 ] == '%' )
		{
			// it's a variable, set it to be a special variable localized string
			wchar_t unicodeVar[ 256 ];
			g_pVGuiLocalize->ConvertANSIToUnicode( imageName, unicodeVar, sizeof( unicodeVar ) );
			char varName[ 256 ];
			V_snprintf( varName, sizeof( varName ), "#var_%s", imageName );
			g_pVGuiLocalize->AddString( varName + 1, unicodeVar, "" );
			m_bWaitingForDialogVar = true;
			SetImage( varName );
}
		else
		{
			SetImage( imageName );
		}
	}
#else
	if ( *imageName )
	{
		SetImage( imageName );
	}
#endif

	const char *pszFillColor = inResourceData->GetString("fillcolor", "");
	if (*pszFillColor)
	{
		int r = 0, g = 0, b = 0, a = 255;
		int len = Q_strlen(pszFillColor) + 1;
		m_pszFillColorName = new char[ len ];
		Q_strncpy( m_pszFillColorName, pszFillColor, len );

		if (sscanf(pszFillColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_FillColor = Color(r, g, b, a);
		}
		else
		{
			IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
			m_FillColor = pScheme->GetColor(pszFillColor, Color(0, 0, 0, 0));
		}
	}

	//=============================================================================
	// HPE_BEGIN:
	// [pfreese] Added support for specifying drawcolor
	//=============================================================================
	const char *pszDrawColor = inResourceData->GetString("drawcolor", "");
	if (*pszDrawColor)
	{
		int r = 255, g = 255, b = 255, a = 255;
		int len = Q_strlen(pszDrawColor) + 1;
		m_pszDrawColorName = new char[ len ];
		Q_strncpy( m_pszDrawColorName, pszDrawColor, len );

		if (sscanf(pszDrawColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_DrawColor = Color(r, g, b, a);
		}
		else
		{
			IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
			m_DrawColor = pScheme->GetColor(pszDrawColor, Color(255, 255, 255, 255));
		}
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	const char *pszBorder = inResourceData->GetString("border", "");
	if (*pszBorder)
	{
		IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
		SetBorder(pScheme->GetBorder(pszBorder));
	}

	BaseClass::ApplySettings(inResourceData);
}

//-----------------------------------------------------------------------------
// Purpose:  load the image, this is done just before this control is displayed
//-----------------------------------------------------------------------------
void ImagePanel::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings(pScheme);
	if ( m_pszImageName && strlen( m_pszImageName ) > 0 )
	{
#ifdef VGUI_ENHANCEMENTS
		if( m_bIsTGA )
			SetImage( GetTGAImage( m_pszImageName ) );
		else
#endif
			SetImage(scheme()->GetImage(m_pszImageName, m_bScaleImage));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Describes editing details
//-----------------------------------------------------------------------------
const char *ImagePanel::GetDescription()
{
	static char buf[1024];
	_snprintf(buf, sizeof(buf), "%s, string image, string border, string fillcolor, bool scaleImage", BaseClass::GetDescription());
	return buf;
}

//-----------------------------------------------------------------------------
// Purpose: sets whether or not the image should scale to fit the size of the ImagePanel (defaults to false)
//-----------------------------------------------------------------------------
void ImagePanel::SetShouldScaleImage( bool state )
{
	m_bScaleImage = state;
}

//-----------------------------------------------------------------------------
// Purpose: gets whether or not the image should be scaled to fit the size of the ImagePanel
//-----------------------------------------------------------------------------
bool ImagePanel::GetShouldScaleImage()
{
	return m_bScaleImage;
}

//-----------------------------------------------------------------------------
// Purpose: used in conjunction with setting that the image should scale and defines an absolute scale amount
//-----------------------------------------------------------------------------
void ImagePanel::SetScaleAmount( float scale )
{
	m_fScaleAmount = scale;
}

float ImagePanel::GetScaleAmount( void )
{
	return m_fScaleAmount;
}

//-----------------------------------------------------------------------------
// Purpose: set the color to fill with, if no Image is specified
//-----------------------------------------------------------------------------
void ImagePanel::SetFillColor( Color col )
{
	m_FillColor = col;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
Color ImagePanel::GetFillColor()
{
	return m_FillColor;
}

char *ImagePanel::GetImageName()
{
	return m_pszImageName;
}

bool ImagePanel::EvictImage()
{
	if ( !m_pImage )
	{
		// nothing to do
		return false;
	}

	if ( !scheme()->DeleteImage( m_pszImageName ) )
	{
		// no eviction occured, could have an outstanding reference
		return false;
	}

	// clear out our cached concept of it
	// as it may change
	// the next SetImage() will re-establish
	m_pImage = NULL;
	delete [] m_pszImageName;
	m_pszImageName = NULL;

	return true;
}

int ImagePanel::GetNumFrames()
{
	if ( !m_pImage )
	{
		return 0;
	}

	return m_pImage->GetNumFrames();
}

void ImagePanel::SetFrame( int nFrame )
{
	if ( !m_pImage )
	{
		return;
	}

	return m_pImage->SetFrame( nFrame );
}

#ifdef VGUI_ENHANCEMENTS
void ImagePanel::OnDialogVariablesChanged( KeyValues* dialogVariables )
{
	// if m_szTGAName starts with #var_ then we need to localize the string
	if ( m_pszImageName[ 0 ] != '#' )
		return;

	m_bWaitingForDialogVar = false;
	// reconstruct the string from the variables
	wchar_t buf[ MAX_PATH ];
	g_pVGuiLocalize->ConstructString( buf, sizeof( buf ), m_pszImageName, dialogVariables );
	char szLocalized[ MAX_PATH ];
	g_pVGuiLocalize->ConvertUnicodeToANSI( buf, szLocalized, sizeof( szLocalized ) );

	SetImage( szLocalized );
}
#endif