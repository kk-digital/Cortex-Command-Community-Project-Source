//////////////////////////////////////////////////////////////////////////////////////////
// File:            SceneLayer.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the SceneLayer class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>

#include "SceneLayer.h"
#include "ContentFile.h"

namespace RTE {

ConcreteClassInfo(SceneLayer, Entity, 0)


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this SceneLayer, effectively
//                  resetting the members of this abstraction level only.

void SceneLayer::Clear()
{
    m_TextureFile.Reset();
	if(m_MainBitmapOwned && m_pMainTexture)
		SDL_DestroyTexture(m_pMainTexture);

    m_pMainTexture = nullptr;
	std::free(m_PixelsRW);
	m_PixelsRW = nullptr;
	m_PixelsWO = nullptr;
	m_Pitch = 0;
	m_pMainTextureHeight = 0;
	m_pMainTextureWidth = 0;
	m_pMainTextureFormat = SDL_PIXELFORMAT_UNKNOWN;
    m_MainBitmapOwned = false;

    m_DrawTrans = true;

    m_Offset.Reset();
    m_ScrollInfo.SetXY(1.0, 1.0);
    m_ScrollRatio.SetXY(1.0, 1.0);
    m_ScaleFactor.SetXY(1.0, 1.0);
    m_ScaleInverse.SetXY(1.0, 1.0);
    m_ScaledDimensions.SetXY(1.0, 1.0);

    m_WrapX = true;
    m_WrapY = true;

    m_FillLeftColor = g_MaskColor;
    m_FillRightColor = g_MaskColor;
    m_FillUpColor = g_MaskColor;
    m_FillDownColor = g_MaskColor;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the SceneLayer object ready for use.

int SceneLayer::Create()
{
    // Read all the properties
    if (Entity::Create() < 0)
        return -1;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the SceneLayer object ready for use.

int SceneLayer::Create(ContentFile textureFile,
                       bool drawTrans,
                       Vector offset,
                       bool wrapX,
                       bool wrapY,
                       Vector scrollInfo)
{
    m_TextureFile = textureFile;

	// Load the Texture file from disk requesting streaming access
    m_pMainTexture = m_TextureFile.GetAsTexture(true);
    RTEAssert(m_pMainTexture, "Failed to load BITMAP in SceneLayer::Create");

    Create(m_pMainTexture, drawTrans, offset, wrapX, wrapY, scrollInfo);

    m_MainBitmapOwned = false;

    // Establisht he scaled dimensions of this
    m_ScaledDimensions.SetXY(m_pMainTextureWidth * m_ScaleFactor.m_X, m_pMainTextureHeight * m_ScaleFactor.m_Y);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the SceneLayer object ready for use.

int SceneLayer::Create(SDL_Texture *pTexture,
                       bool drawTrans,
                       Vector offset,
                       bool wrapX,
                       bool wrapY,
                       Vector scrollInfo)
{

	RTEAssert(pTexture, "Null bitmap passed in when creating SceneLayer");
	int access;

	SDL_QueryTexture(pTexture, &m_pMainTextureFormat, &access, &m_pMainTextureWidth, &m_pMainTextureHeight);

	RTEAssert(access == SDL_TEXTUREACCESS_STREAMING,
		      "Non streaming access texture passed in when creating SceneLayer")
	m_pMainTexture = pTexture;

	m_MainBitmapOwned = true;

    m_DrawTrans = drawTrans;
    m_Offset = offset;
    m_WrapX = wrapX;
    m_WrapY = wrapY;
    m_ScrollInfo = scrollInfo;

    if (m_WrapX)
        m_ScrollRatio.m_X = m_ScrollInfo.m_X;
    else
        if (m_ScrollInfo.m_X == -1.0 || m_ScrollInfo.m_X == 1.0)
            m_ScrollRatio.m_X = 1.0;
        else if (m_ScrollInfo.m_X == g_FrameMan.GetPlayerScreenWidth())
            m_ScrollRatio.m_X = m_pMainTextureWidth - g_FrameMan.GetPlayerScreenWidth();
        else if (m_pMainTextureWidth == g_FrameMan.GetPlayerScreenWidth())
            m_ScrollRatio.m_X = 1.0f / (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerScreenWidth());
        else
            m_ScrollRatio.m_X = (float)(m_pMainTextureWidth - g_FrameMan.GetPlayerScreenWidth()) /
                                (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerScreenWidth());

    if (m_WrapY)
        m_ScrollRatio.m_Y = m_ScrollInfo.m_Y;
    else
        if (m_ScrollInfo.m_Y == -1.0 || m_ScrollInfo.m_Y == 1.0)
            m_ScrollRatio.m_Y = 1.0;
        else if (m_ScrollInfo.m_Y == g_FrameMan.GetPlayerScreenHeight())
            m_ScrollRatio.m_Y = m_pMainTextureHeight - g_FrameMan.GetPlayerScreenHeight();
        else if (m_pMainTextureHeight == g_FrameMan.GetPlayerScreenHeight())
            m_ScrollRatio.m_Y = 1.0f / (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerScreenHeight());
        else
            m_ScrollRatio.m_Y = (float)(m_pMainTextureHeight - g_FrameMan.GetPlayerScreenHeight()) /
                                (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerScreenHeight());

    // Establish the scaled dimensions of this
    m_ScaledDimensions.SetXY(m_pMainTextureWidth * m_ScaleFactor.m_X, m_pMainTextureHeight * m_ScaleFactor.m_Y);

    // Sampled color at the edges of the layer that can be used to fill gap if the layer isn't large enough to cover a target bitmap
	m_FillLeftColor =
		m_WrapX ? g_MaskColor
		        : reinterpret_cast<Uint32 *>(
		              m_PixelsRW)[m_pMainTextureWidth *
		                          (m_pMainTextureHeight / 2)];
	m_FillRightColor =
		m_WrapX
		    ? g_MaskColor
		    : reinterpret_cast<Uint32 *>(
		          m_PixelsRW)[m_pMainTextureWidth * (m_pMainTextureHeight / 2) +
		                    m_pMainTextureWidth - 1];
	m_FillUpColor = m_WrapY ? g_MaskColor
		                    : reinterpret_cast<Uint32 *>(
		                          m_PixelsRW)[(m_pMainTextureWidth / 2) - 1];

	m_FillDownColor =
		m_WrapY
		    ? g_MaskColor
		    : reinterpret_cast<Uint32 *>(
		          m_PixelsRW)[m_pMainTextureWidth * (m_pMainTextureHeight - 1) +
		                      (m_pMainTextureWidth / 2)];

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a SceneLayer to be identical to another, by deep copy.
// Arguments:       A reference to the SceneLayer to deep copy.

int SceneLayer::Create(const SceneLayer &reference)
{
    Entity::Create(reference);

    m_TextureFile = reference.m_TextureFile;

    // Deep copy the bitmap
    if (reference.m_pMainTexture)
    {
        // Copy the bitmap from the ContentFile, because we're going to be changing it!
        SDL_Texture *pCopyFrom = reference.m_pMainTexture;
        RTEAssert(pCopyFrom, "Couldn't load the bitmap file specified for SceneLayer!");




        // Destination
		m_pMainTexture = SDL_CreateTexture(
			g_FrameMan.GetRenderer(), reference.m_pMainTextureFormat,
			SDL_TEXTUREACCESS_STREAMING, reference.m_pMainTextureWidth,
			reference.m_pMainTextureHeight);


		RTEAssert(m_pMainTexture,
			      "Failed to allocate SDL_Texture in SceneLayer::Create");

		// Copy the RW access Pixels from the reference
		try {
			if (m_PixelsRW) {
				m_PixelsRW = std::realloc(m_PixelsRW,
					                       reference.m_Pitch *
					                           reference.m_pMainTextureHeight);
			} else {
				m_PixelsRW = std::malloc(reference.m_Pitch *
					                      reference.m_pMainTextureHeight);
			}
		} catch (std::bad_alloc) {
			RTEAbort("Could not allocate memory for SceneLayer")
		}

		std::memcpy(m_PixelsRW, reference.m_PixelsRW,
			   reference.m_Pitch * reference.m_pMainTextureHeight);

		// Lock the newly created Texture
		SDL_LockTexture(m_pMainTexture, nullptr, &m_PixelsWO, &m_Pitch);
		// Copy the RW pixels to the Texture
		std::memcpy(m_PixelsWO, m_PixelsRW, m_Pitch*m_pMainTextureHeight);
		// Unlock the Texture again
		SDL_UnlockTexture(m_pMainTexture);

        InitScrollRatios();

        m_MainBitmapOwned = true;
    }
    // If no bitmap to copy, has to load the data (LoadData) to create this in the copied to SL
    else
        m_MainBitmapOwned = false;

    m_DrawTrans = reference.m_DrawTrans;
    m_Offset = reference.m_Offset;
    m_WrapX = reference.m_WrapX;
    m_WrapY = reference.m_WrapY;
    m_ScrollInfo = reference.m_ScrollInfo;
    // The ratios need to be calculated from the ScrollInfo with InitScrollRatios later in LoadData
    m_ScrollRatio = reference.m_ScrollRatio;
    m_ScaleFactor = reference.m_ScaleFactor;
    m_ScaleInverse = reference.m_ScaleInverse;
    m_ScaledDimensions = reference.m_ScaledDimensions;
    m_FillLeftColor = reference.m_FillLeftColor;
    m_FillRightColor = reference.m_FillRightColor;
    m_FillUpColor = reference.m_FillUpColor;
    m_FillDownColor = reference.m_FillDownColor;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  LoadData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Actually loads previously specified/created data into memory. Has
//                  to be done before using this SceneLayer.

int SceneLayer::LoadData()
{
/* No need to do this copying, we are re-loading fresh from disk each time
    // Copy the bitmap from the ContentFile, because we're going to be changing it!
    BITMAP *pCopyFrom = m_BitmapFile.GetAsBitmap();
    RTEAssert(pCopyFrom, "Couldn't load the bitmap file specified for SceneLayer!");

    // Destination
    m_pMainBitmap = create_bitmap_ex(8, pCopyFrom->w, pCopyFrom->h);
    RTEAssert(m_pMainBitmap, "Failed to allocate BITMAP in SceneLayer::Create");

    // Copy!
    blit(pCopyFrom, m_pMainBitmap, 0, 0, 0, 0, pCopyFrom->w, pCopyFrom->h);
*/
    // Re-load directly from disk each time; don't do any caching of these bitmaps
    m_pMainTexture = m_TextureFile.GetAsTexture(false);

    m_MainBitmapOwned = true;

    InitScrollRatios();

	// Sampled color at the edges of the layer that can be used to fill gap if
	// the layer isn't large enough to cover a target bitmap
	m_FillLeftColor =
		m_WrapX
		    ? g_MaskColor
		    : reinterpret_cast<Uint32 *>(
		          m_PixelsRW)[m_pMainTextureWidth * (m_pMainTextureHeight / 2)];
	m_FillRightColor =
		m_WrapX
		    ? g_MaskColor
		    : reinterpret_cast<Uint32 *>(
		          m_PixelsRW)[m_pMainTextureWidth * (m_pMainTextureHeight / 2) +
		                      m_pMainTextureWidth - 1];
	m_FillUpColor = m_WrapY ? g_MaskColor
		                    : reinterpret_cast<Uint32 *>(
		                          m_PixelsRW)[(m_pMainTextureWidth / 2) - 1];

	m_FillDownColor =
		m_WrapY
		    ? g_MaskColor
		    : reinterpret_cast<Uint32 *>(
		          m_PixelsRW)[m_pMainTextureWidth * (m_pMainTextureHeight - 1) +
		                      (m_pMainTextureWidth / 2)];

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SaveData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves current data in memory to disk.

int SceneLayer::SaveData(string bitmapPath)
{
    if (bitmapPath.empty())
        return -1;

    // Save out the bitmap
    if (m_pMainTexture)
    {
		SDL_Surface *savePNG = SDL_CreateRGBSurfaceWithFormatFrom(m_PixelsRW, m_pMainTextureWidth, m_pMainTextureHeight, 32, m_Pitch, m_pMainTextureFormat);
		if(!savePNG){
			return -1;
		}
		if(!IMG_SavePNG(savePNG, bitmapPath.c_str()))
			return -1;

        // Set the new path to point to the new file location - only if there was a successful save of the bitmap
        m_TextureFile.SetDataPath(bitmapPath);
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ClearData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears out any previously loaded bitmap data from memory.

int SceneLayer::ClearData()
{
    if (m_pMainTexture && m_MainBitmapOwned)
		SDL_DestroyTexture(m_pMainTexture);
    m_pMainTexture = nullptr;
	m_pMainTextureWidth = 0;
	m_pMainTextureHeight = 0;
	m_pMainTextureFormat = SDL_PIXELFORMAT_UNKNOWN;
	m_PixelsWO = nullptr;
	free(m_PixelsRW);
	m_PixelsRW = nullptr;
	m_Pitch = 0;

    m_MainBitmapOwned = false;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int SceneLayer::ReadProperty(const std::string_view &propName, Reader &reader)
{
    if (propName == "BitmapFile")
        reader >> m_TextureFile;
    else if (propName == "DrawTransparent")
        reader >> m_DrawTrans;
    else if (propName == "Offset")
        reader >> m_Offset;
    else if (propName == "WrapX")
        reader >> m_WrapX;
    else if (propName == "WrapY")
        reader >> m_WrapY;
    // Actually read the scrollinfo, not the ratio. The ratios will be initalized later
    else if (propName == "ScrollRatio")
        reader >> m_ScrollInfo;
    else if (propName == "ScaleFactor")
    {
        reader >> m_ScaleFactor;
        // Gotto init more
        SetScaleFactor(m_ScaleFactor);
    }
    else
        return Entity::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this SceneLayer with a Writer for
//                  later recreation with Create(Reader &reader);

int SceneLayer::Save(Writer &writer) const
{
    Entity::Save(writer);

    writer.NewProperty("BitmapFile");
    writer << m_TextureFile;
    writer.NewProperty("DrawTransparent");
    writer << m_DrawTrans;
    writer.NewProperty("Offset");
    writer << m_Offset;
    writer.NewProperty("WrapX");
    writer << m_WrapX;
    writer.NewProperty("WrapY");
    writer << m_WrapY;
    writer.NewProperty("ScrollRatio");
    writer << m_ScrollInfo;
    writer.NewProperty("ScaleFactor");
    writer << m_ScaleFactor;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the SceneLayer object.

void SceneLayer::Destroy(bool notInherited)
{
    if (m_MainBitmapOwned)
        SDL_DestroyTexture(m_pMainTexture);

    if (!notInherited)
        Entity::Destroy();
    Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetScaleFactor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the scale that this should be drawn at when using DrawScaled.

void SceneLayer::SetScaleFactor(const Vector newScale)
{
    m_ScaleFactor = newScale;
    m_ScaleInverse.SetXY(1.0f / newScale.m_X, 1.0f / newScale.m_Y);
    if (m_pMainTexture)
        m_ScaledDimensions.SetXY(m_pMainTextureWidth * newScale.m_X, m_pMainTextureHeight * newScale.m_Y);
}

void SceneLayer::UnlockTexture()
{
	SDL_UnlockTexture(m_pMainTexture);
	m_PixelsWO = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a specific pixel from the main bitmap of this SceneLayer.
//                  LockColorBitmap() must be called before using this method.

Uint32 SceneLayer::GetPixel(const int pixelX, const int pixelY)
{
    // Make sure it's within the boundaries of the bitmap.
    if (pixelX < 0 || pixelX >= m_pMainTextureWidth || pixelY < 0 || pixelY >= m_pMainTextureHeight)
        return 0;

    return reinterpret_cast<Uint32 *>(m_PixelsRW)[pixelX + pixelY * m_Pitch/4];
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific pixel on the main bitmap of this SceneLayer to a
//                  specific value. LockColorBitmap() must be called before using this
//                  method.

void SceneLayer::SetPixel(const int pixelX, const int pixelY, const Uint32 value)
{
    RTEAssert(m_MainBitmapOwned, "Trying to set a pixel of a SceneLayer's bitmap which isn't owned!");

    // Make sure it's within the boundaries of the bitmap.
    if (pixelX < 0 ||
       pixelX >= m_pMainTextureWidth ||
       pixelY < 0 ||
       pixelY >= m_pMainTextureHeight)
       return;

	reinterpret_cast<Uint32 *>(m_PixelsRW)[pixelX + pixelY * m_Pitch / 4] = value;
	reinterpret_cast<Uint32 *>(m_PixelsWO)[pixelX + pixelY * m_Pitch / 4] = value;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ForceBounds
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Wraps or bounds a position coordinate if it is off bounds of the
//                  SceneLayer, depending on the wrap settings of this SceneLayer.

bool SceneLayer::ForceBounds(int &posX, int &posY, bool scaled) const
{
    bool wrapped = false;
    int width = scaled ? m_ScaledDimensions.GetFloorIntX() : m_pMainTextureWidth;
    int height = scaled ? m_ScaledDimensions.GetFloorIntY() : m_pMainTextureHeight;

    if (posX < 0) {
        if (m_WrapX)
        {
            while (posX < 0)
                posX += width;
            wrapped = true;
        }
        else
            posX = 0;
    }

    if (posY < 0) {
        if (m_WrapY)
        {
            while (posY < 0)
                posY += height;
            wrapped = true;
        }
        else
            posY = 0;
    }

    if (posX >= width) {
        if (m_WrapX)
        {
            posX %= width;
            wrapped = true;
        }
        else
            posX = width - 1;
    }

    if (posY >= height) {
        if (m_WrapY)
        {
            posY %= height;
            wrapped = true;
        }
        else
            posY = height - 1;
    }

    return wrapped;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ForceBounds
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Wraps or bounds a position coordinate if it is off bounds of the
//                  SceneLayer, depending on the wrap settings of this SceneLayer.

bool SceneLayer::ForceBounds(Vector &pos, bool scaled) const
{
    int posX = std::floor(pos.m_X);
    int posY = std::floor(pos.m_Y);

    bool wrapped = ForceBounds(posX, posY, scaled);

    pos.m_X = posX + (pos.m_X - std::floor(pos.m_X));
    pos.m_Y = posY + (pos.m_Y - std::floor(pos.m_Y));

    return wrapped;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WrapPosition
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Only wraps a position coordinate if it is off bounds of the SceneLayer
//                  and wrapping in the corresponding axes are turned on.

bool SceneLayer::WrapPosition(int &posX, int &posY, bool scaled) const
{
    bool wrapped = false;
    int width = scaled ? m_ScaledDimensions.GetFloorIntX() : m_pMainTextureWidth;
    int height = scaled ? m_ScaledDimensions.GetFloorIntY() : m_pMainTextureHeight;

    if (m_WrapX) {
        if (posX < 0) {
            while (posX < 0)
                posX += width;
            wrapped = true;
        }
        else if (posX >= width) {
            posX %= width;
            wrapped = true;
        }
    }

    if (m_WrapY) {
        if (posY < 0) {
            while (posY < 0)
                posY += height;
            wrapped = true;
        }
        else if (posY >= height) {
            posY %= height;
            wrapped = true;
        }
    }

    return wrapped;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WrapPosition
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Only wraps a position coordinate if it is off bounds of the SceneLayer
//                  and wrapping in the corresponding axes are turned on.

bool SceneLayer::WrapPosition(Vector &pos, bool scaled) const
{
    int posX = std::floor(pos.m_X);
    int posY = std::floor(pos.m_Y);

    bool wrapped = WrapPosition(posX, posY, scaled);

    pos.m_X = posX + (pos.m_X - std::floor(pos.m_X));
    pos.m_Y = posY + (pos.m_Y - std::floor(pos.m_Y));

    return wrapped;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this SceneLayer. Supposed to be done every frame.

void SceneLayer::Update()
{
    ;
}


// TODO: Declare this in the header and remove the dupe declaration in NetworkClient.cpp
// Data structure for constructing the draw boxes we'll need to use for drawing
struct SLDrawBox
{
    int sourceX;
    int sourceY;
    int sourceW;
    int sourceH;
    int destX;
    int destY;
};


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this SceneLayer's current scrolled position to a bitmap.

void SceneLayer::Draw(SDL_Renderer * renderer, Box& targetBox, const Vector &scrollOverride) const
{
    RTEAssert(m_pMainTexture, "Data of this SceneLayer has not been loaded before trying to draw!");

	SDL_Rect source;
	SDL_Rect dest;
    list<SLDrawBox> drawList;

	SDL_Rect viewportDimensions;
	SDL_RenderGetViewport(renderer, &viewportDimensions);

	int offsetX;
    int offsetY;
    bool scrollOverridden = !(scrollOverride.m_X == -1 && scrollOverride.m_Y == -1);

    // Overridden scroll position
    if (scrollOverridden)
    {
        offsetX = scrollOverride.GetFloorIntX();
        offsetY = scrollOverride.GetFloorIntY();
    }
    // Regular scroll
    else
    {
        offsetX = std::floor(m_Offset.m_X * m_ScrollRatio.m_X);
        offsetY = std::floor(m_Offset.m_Y * m_ScrollRatio.m_Y);
        // Only force bounds when doing regular scroll offset because the override is used to do terrain object application tricks and sometimes needs the offsets to be < 0
//        ForceBounds(offsetX, offsetY);
        WrapPosition(offsetX, offsetY);
    }

    // Make target box valid size if it's empty
    if (targetBox.IsEmpty())
    {
        targetBox.SetCorner(Vector(0, 0));
        targetBox.SetWidth(viewportDimensions.w);
        targetBox.SetHeight(viewportDimensions.h);
    }

    // Set the clipping rectangle of the renderer to match the specified target box
	SDL_Rect clipRect{static_cast<int>(targetBox.GetCorner().m_X),
		              static_cast<int>(targetBox.GetCorner().m_Y),
		              static_cast<int>(targetBox.GetWidth()),
		              static_cast<int>(targetBox.GetHeight())};
	SDL_RenderSetClipRect(renderer, &clipRect);

	// See if this SceneLayer is wider AND higher than the target bitmap; then use simple wrapping logic - oterhwise need to tile
    if (m_pMainTextureWidth >= viewportDimensions.w && m_pMainTextureHeight >= viewportDimensions.h)
    {
		source.x = offsetX;
		source.y = offsetY;
		source.w = m_pMainTextureWidth -offsetX;
		source.h = m_pMainTextureHeight - offsetY;

		dest.x = targetBox.GetCorner().m_X;
		dest.y = targetBox.GetCorner().m_Y;
		dest.w = source.w;
		dest.h = source.h;

		SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

		source.x = 0;
		source.y = offsetY;
		source.w = offsetX;
		source.h = m_pMainTextureHeight - offsetY;

        dest.x       = targetBox.GetCorner().m_X + m_pMainTextureWidth - offsetX;
        dest.y       = targetBox.GetCorner().m_Y;
		dest.w = source.w;
		dest.h = source.h;

		SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

        source.x     = offsetX;
        source.y     = 0;
        source.w     = m_pMainTextureWidth - offsetX;
        source.h     = offsetY;
        dest.x       = targetBox.GetCorner().m_X;
        dest.y       = targetBox.GetCorner().m_Y + m_pMainTextureHeight - offsetY;
		dest.w = source.w;
		dest.h = source.h;
		SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

        source.x     = 0;
        source.y     = 0;
        source.w     = offsetX;
        source.h     = offsetY;
        dest.x       = targetBox.GetCorner().m_X + m_pMainTextureWidth - offsetX;
        dest.y       = targetBox.GetCorner().m_Y + m_pMainTextureHeight - offsetY;
		dest.w = source.w;
		dest.h = source.h;
		SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);
    }
    // Target bitmap is larger in some dimension, so need to draw this tiled as many times as necessary to cover the whole target
    else
    {
        int tiledOffsetX = 0;
        int tiledOffsetY = 0;
        // Use the dimensions of the target box, if it has any area at all
        int targetWidth = std::min(viewportDimensions.w, static_cast<int>(targetBox.GetWidth()));
        int targetHeight = std::min(viewportDimensions.h, static_cast<int>(targetBox.GetHeight()));
        int toCoverX = offsetX + targetBox.GetCorner().m_X + targetWidth;
        int toCoverY = offsetY + targetBox.GetCorner().m_Y + targetHeight;

        // Check for special case adjustment when the screen is larger than the scene
        bool screenLargerThanSceneX = false;
        bool screenLargerThanSceneY = false;
        if (!scrollOverridden && g_SceneMan.GetSceneWidth() > 0)
        {
            screenLargerThanSceneX = viewportDimensions.w > g_SceneMan.GetSceneWidth();
            screenLargerThanSceneY = viewportDimensions.h > g_SceneMan.GetSceneHeight();
        }

        // Y tiling
        do
        {
            // X tiling
            do
            {
                source.x     = 0;
                source.y     = 0;
                source.w     = m_pMainTextureWidth;
                source.h     = m_pMainTextureHeight;
                // If the unwrapped and untiled direction can't cover the target area, place it in the middle of the target bitmap, and leave the excess perimeter on each side untouched
                dest.x       = (!m_WrapX && screenLargerThanSceneX) ? ((viewportDimensions.w / 2) - (m_pMainTextureWidth / 2)) : (targetBox.GetCorner().m_X + tiledOffsetX - offsetX);
                dest.y       = (!m_WrapY && screenLargerThanSceneY) ? ((viewportDimensions.h / 2) - (m_pMainTextureHeight / 2)) : (targetBox.GetCorner().m_Y + tiledOffsetY - offsetY);
				dest.w = source.w;
				dest.h = source.h;
				SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

                tiledOffsetX += m_pMainTextureWidth;
            }
            // Only tile if we're supposed to wrap widthwise
            while (m_WrapX && toCoverX > tiledOffsetX);

            tiledOffsetY += m_pMainTextureHeight;
        }
        // Only tile if we're supposed to wrap heightwise
        while (m_WrapY && toCoverY > tiledOffsetY);

// TODO: Do this above instead, testing down here only
        // Detect if nonwrapping layer dimensions can't cover the whole target area with its main bitmap. If so, fill in the gap with appropriate solid color sampled from the hanging edge
		SDL_PixelFormat *format = SDL_AllocFormat(m_pMainTextureFormat);
		uint_fast8_t r, g, b, a;
		if (!m_WrapX && !screenLargerThanSceneX && m_ScrollRatio.m_X < 0) {
			if (m_FillLeftColor != g_MaskColor && offsetX != 0){
				SDL_GetRGBA(m_FillLeftColor, format, &r, &g, &b, &a);
				SDL_SetRenderDrawColor(renderer, r, g, b, a);
				SDL_Rect target{
					static_cast<int>(targetBox.GetCorner().m_X),
					static_cast<int>(targetBox.GetCorner().m_Y),
					static_cast<int>(-offsetX),
					static_cast<int>(targetBox.GetHeight())
				};
				SDL_RenderDrawRect(renderer, &target);
			}
            if (m_FillRightColor != g_MaskColor){
				SDL_GetRGBA(m_FillRightColor, format, &r, &g, &b, &a);
				SDL_SetRenderDrawColor(renderer, r, g, b, a);
				SDL_Rect target{
					static_cast<int>(targetBox.GetCorner().m_X-offsetX)+m_pMainTextureWidth,
					static_cast<int>(targetBox.GetCorner().m_Y),
					static_cast<int>(targetBox.GetWidth()+offsetX),
					static_cast<int>(-offsetY)
				};
				SDL_RenderDrawRect(renderer, &target);
			}
		}

		if (!m_WrapY && !screenLargerThanSceneY && m_ScrollRatio.m_Y < 0)
        {
            if (m_FillUpColor != g_MaskColor && offsetY != 0){

				SDL_GetRGBA(m_FillUpColor, format, &r, &g, &b, &a);
				SDL_SetRenderDrawColor(renderer, r, g, b, a);
				SDL_Rect target{
					static_cast<int>(targetBox.GetCorner().m_X),
					static_cast<int>(targetBox.GetCorner().m_Y),
					static_cast<int>(targetBox.GetWidth()),
					static_cast<int>(-offsetY)};
				SDL_RenderDrawRect(renderer, &target);
			}
            if (m_FillDownColor != g_MaskColor){
				SDL_GetRGBA(m_FillDownColor, format, &r, &g, &b, &a);
				SDL_SetRenderDrawColor(renderer, r, g, b, a);
				SDL_Rect target{
					static_cast<int>(targetBox.GetCorner().m_X),
					static_cast<int>(targetBox.GetCorner().m_Y - offsetY) +
					    m_pMainTextureHeight,
					static_cast<int>(targetBox.GetWidth()),
					static_cast<int>(targetBox.GetHeight() + offsetY -
					                 m_pMainTextureHeight)};
				SDL_RenderDrawRect(renderer, &target);
			}
        }
		SDL_FreeFormat(format);
	}

    // Reset the clip rect back to the entire target bitmap
	SDL_RenderSetClipRect(renderer, nullptr);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  DrawScaled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this SceneLayer's current scrolled position to a bitmap, but also
//                  scaled according to what has been set with SetScaleFactor.

void SceneLayer::DrawScaled(SDL_Renderer *renderer, Box &targetBox, const Vector &scrollOverride) const
{
    // If no scaling, use the regular scaling routine
    if (m_ScaleFactor.m_X == 1.0 && m_ScaleFactor.m_Y == 1.0)
        return Draw(renderer, targetBox, scrollOverride);

    RTEAssert(m_pMainTexture, "Data of this SceneLayer has not been loaded before trying to draw!");


/*

    // Calculate how many times smaller the unseen map is compared to the entire terrain's dimensions
    Vector resDenom((float)pTerrain->GetBitmap()->w / (float)pUnseenLayer->GetBitmap()->w, (float)pTerrain->GetBitmap()->h / (float)pUnseenLayer->GetBitmap()->h);
    resDenom.Floor();
    // Now convert it back to the multiplier so that the flooring that happened above is reflected in the multiplier too, to avoid jittering
    Vector resMult(1.0f / resDenom.m_X, 1.0f / resDenom.m_Y);
    // This is the sub-source-pixel offset on the target to make scrolling of the pixelated bitmap smooth
    Vector subOffset((int)targetPos.m_X % (int)resDenom.m_X, (int)targetPos.m_Y % (int)resDenom.m_Y);
    masked_stretch_blit(pUnseenLayer->GetBitmap(), pTargetBitmap, (int)(targetPos.m_X * resMult.m_X + 0.0001f), (int)(targetPos.m_Y * resMult.m_Y + 0.0001f), viewportDimensions.w * resMult.m_X + 1.0001f, viewportDimensions.h * resMult.m_Y + 1.0001f, -subOffset.m_X, -subOffset.m_Y, pTargetBitmap->w + resDenom.m_X, pTargetBitmap->h + resDenom.m_Y);



// TODO: Remove
//                char balle[245];
//                std::snprintf(balle, sizeof(balle), "y: %f becomes %f through %f and %f, needs to match: %f, which is: %i", targetPos.m_Y, subOffset.m_Y, resMult.m_Y, resDenom.m_Y, targetPos.m_Y * resMult.m_Y + 0.0001f, (int)(targetPos.m_Y * resMult.m_Y + 0.0001f));
//                g_FrameMan.SetScreenText(string(balle));


*/

	SDL_Rect viewportDimensions;
	SDL_RenderGetViewport(renderer, &viewportDimensions);

	SDL_Rect source{0,0,0,0};
	SDL_Rect dest{0,0,0,0};
    list<SLDrawBox> drawList;

    int offsetX;
    int offsetY;
    bool scrollOverridden = !(scrollOverride.m_X == -1 && scrollOverride.m_Y == -1);

    // Overridden scroll position
    if (scrollOverridden)
    {
        offsetX = scrollOverride.GetFloorIntX();
        offsetY = scrollOverride.GetFloorIntY();
    }
    // Regular scroll
    else
    {
        offsetX = std::floor(m_Offset.m_X * m_ScrollRatio.m_X);
        offsetY = std::floor(m_Offset.m_Y * m_ScrollRatio.m_Y);
        // Only force bounds when doing regular scroll offset because the override is used to do terrain object application tricks and sometimes needs the offsets to be < 0
//        ForceBounds(offsetX, offsetY);
        WrapPosition(offsetX, offsetY);
    }

    // Make target box valid size if it's empty
    if (targetBox.IsEmpty())
    {
        targetBox.SetCorner(Vector(0, 0));
        targetBox.SetWidth(viewportDimensions.w);
        targetBox.SetHeight(viewportDimensions.h);
    }

	// Set the clipping rectangle of the target bitmap to match the specified
	// target box
	SDL_Rect clipRect{static_cast<int>(targetBox.GetCorner().m_X),
		              static_cast<int>(targetBox.GetCorner().m_Y),
		              static_cast<int>(targetBox.GetWidth()),
		              static_cast<int>(targetBox.GetHeight())};
	SDL_RenderSetClipRect(renderer, &clipRect);

    // Get a scaled offset for the source layer
    Vector sourceOffset(offsetX * m_ScaleInverse.m_X, offsetY * m_ScaleInverse.m_Y);

    // See if this SceneLayer is wider AND higher than the target bitmap when scaled; then use simple wrapping logic - oterhwise need to tile
    if (m_ScaledDimensions.m_X >= viewportDimensions.w && m_ScaledDimensions.m_Y >= viewportDimensions.h)
    {
        // Upper left
        source.x     = 0;
        source.y     = 0;
        source.w     = m_pMainTextureWidth;
        source.h     = m_pMainTextureHeight;
        dest.x       = targetBox.GetCorner().m_X - offsetX;
        dest.y       = targetBox.GetCorner().m_Y - offsetY;
        dest.w       = source.w * m_ScaleFactor.m_X + 1;
        dest.h       = source.h * m_ScaleFactor.m_Y + 1;
        SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

        // Upper right
        source.x     = 0;
        source.y     = 0;
        source.w     = sourceOffset.m_X;
        source.h     = m_pMainTextureHeight;
        dest.x       = targetBox.GetCorner().m_X + m_ScaledDimensions.m_X - offsetX;
        dest.y       = targetBox.GetCorner().m_Y - offsetY;
        dest.w       = source.w * m_ScaleFactor.m_X + 1;
        dest.h       = source.h * m_ScaleFactor.m_Y + 1;
        SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

        // Lower left
        source.x     = 0;
        source.y     = 0;
        source.w     = m_pMainTextureWidth;
        source.h     = sourceOffset.m_Y;
        dest.x       = targetBox.GetCorner().m_X - offsetX;
        dest.y       = targetBox.GetCorner().m_Y + m_ScaledDimensions.m_Y - offsetY;
        dest.w       = source.w * m_ScaleFactor.m_X + 1;
        dest.h       = source.h * m_ScaleFactor.m_Y + 1;
        SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

        // Lower right
        source.x     = 0;
        source.y     = 0;
        source.w     = sourceOffset.m_X;
        source.h     = sourceOffset.m_Y;
        dest.x       = targetBox.GetCorner().m_X + m_ScaledDimensions.m_X - offsetX;
        dest.y       = targetBox.GetCorner().m_Y + m_ScaledDimensions.m_Y - offsetY;
        dest.w       = source.w * m_ScaleFactor.m_X + 1;
        dest.h       = source.h * m_ScaleFactor.m_Y + 1;
        SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);
    }
    // Target bitmap is larger in some dimension, so need to draw this tiled as many times as necessary to cover the whole target
    else
    {
        int tiledOffsetX = 0;
        int tiledOffsetY = 0;
        // Use the dimensions of the target box, if it has any area at all
        int targetWidth = std::min(viewportDimensions.w, static_cast<int>(targetBox.GetWidth()));
        int targetHeight = std::min(viewportDimensions.h, static_cast<int>(targetBox.GetHeight()));
        int toCoverX = offsetX + targetBox.GetCorner().m_X + targetWidth;
        int toCoverY = offsetY + targetBox.GetCorner().m_Y + targetHeight;

        // Check for special case adjustment when the screen is larger than the scene
        bool screenLargerThanSceneX = false;
        bool screenLargerThanSceneY = false;
        if (!scrollOverridden && g_SceneMan.GetSceneWidth() > 0)
        {
            screenLargerThanSceneX = viewportDimensions.w > g_SceneMan.GetSceneWidth();
            screenLargerThanSceneY = viewportDimensions.h > g_SceneMan.GetSceneHeight();
        }

        // Y tiling
        do
        {
            // X tiling
            do
            {
                source.x     = 0;
                source.y     = 0;
                source.w     = m_pMainTextureWidth;
                source.h     = m_pMainTextureHeight;
                // If the unwrapped and untiled direction can't cover the target area, place it in the middle of the target bitmap, and leave the excess perimeter on each side untouched
                dest.x       = (!m_WrapX && screenLargerThanSceneX) ? ((viewportDimensions.w / 2) - (m_ScaledDimensions.m_X / 2)) : (targetBox.GetCorner().m_X + tiledOffsetX - offsetX);
                dest.y       = (!m_WrapY && screenLargerThanSceneY) ? ((viewportDimensions.h / 2) - (m_ScaledDimensions.m_Y / 2)) : (targetBox.GetCorner().m_Y + tiledOffsetY - offsetY);
                dest.w       = m_ScaledDimensions.m_X;
                dest.h       = m_ScaledDimensions.m_Y;
                SDL_RenderCopy(renderer, m_pMainTexture, &source, &dest);

                tiledOffsetX += m_ScaledDimensions.m_X;
            }
            // Only tile if we're supposed to wrap widthwise
            while (m_WrapX && toCoverX > tiledOffsetX);

            tiledOffsetY += m_ScaledDimensions.m_Y;
        }
        // Only tile if we're supposed to wrap heightwise
        while (m_WrapY && toCoverY > tiledOffsetY);

// TODO: Do this above instead, testing down here only
/*
        // Detect if nonwrapping layer dimensions can't cover the whole target area with its main bitmap. If so, fill in the gap with appropriate solid color sampled from the hanging edge
        if (!m_WrapX && !screenLargerThanSceneX && m_ScrollRatio.m_X < 0)
        {
            if (m_FillLeftColor != g_MaskColor && offsetX != 0)
                rectfill(pTargetBitmap, targetBox.GetCorner().m_X, targetBox.GetCorner().m_Y, targetBox.GetCorner().m_X - offsetX, targetBox.GetCorner().m_Y + targetBox.GetHeight(), m_FillLeftColor);
            if (m_FillRightColor != g_MaskColor)
                rectfill(pTargetBitmap, (targetBox.GetCorner().m_X - offsetX) + m_pMainTextureWidth, targetBox.GetCorner().m_Y, targetBox.GetCorner().m_X + targetBox.GetWidth(), targetBox.GetCorner().m_Y + targetBox.GetHeight(), m_FillRightColor);
        }

        if (!m_WrapY && !screenLargerThanSceneY && m_ScrollRatio.m_Y < 0)
        {
            if (m_FillUpColor != g_MaskColor && offsetY != 0)
                rectfill(pTargetBitmap, targetBox.GetCorner().m_X, targetBox.GetCorner().m_Y, targetBox.GetCorner().m_X + targetBox.GetWidth(), targetBox.GetCorner().m_Y - offsetY, m_FillUpColor);
            if (m_FillDownColor != g_MaskColor)
                rectfill(pTargetBitmap, targetBox.GetCorner().m_X, (targetBox.GetCorner().m_Y - offsetY) + m_pMainTextureHeight, targetBox.GetCorner().m_X + targetBox.GetWidth(), targetBox.GetCorner().m_Y + targetBox.GetHeight(), m_FillDownColor);
        }
*/
    }

    // Reset the clip rect back to the entire target bitmap
	SDL_RenderSetClipRect(renderer, nullptr);
}

/* not neccessary
//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadContour
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads a terrain contour file and applies it to this SceneLayer.

virtual int SceneLayer::LoadContour(char material, bool dirtBelowContour, ContentFile &contourFile)
{
    char *contour
}
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  InitScrollRatios
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Initialize the scroll ratios from the encoded scroll info. Must be
//                  done after the bitmap has been created in the derived concrete classes.

void SceneLayer::InitScrollRatios()
{
    if (m_WrapX)
        m_ScrollRatio.m_X = m_ScrollInfo.m_X;
    else
    {
        if (m_ScrollInfo.m_X == -1.0 || m_ScrollInfo.m_X == 1.0)
            m_ScrollRatio.m_X = 1.0;
        else if (m_ScrollInfo.m_X == g_FrameMan.GetPlayerScreenWidth())
            m_ScrollRatio.m_X = m_pMainTextureWidth - g_FrameMan.GetPlayerScreenWidth();
        else if (m_pMainTextureWidth == g_FrameMan.GetPlayerScreenWidth())
            m_ScrollRatio.m_X = 1.0f / (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerScreenWidth());
        else
            m_ScrollRatio.m_X = (float)(m_pMainTextureWidth - g_FrameMan.GetPlayerScreenWidth()) /
                                (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerScreenWidth());
    }

    if (m_WrapY)
        m_ScrollRatio.m_Y = m_ScrollInfo.m_Y;
    else
    {
        if (m_ScrollInfo.m_Y == -1.0 || m_ScrollInfo.m_Y == 1.0)
            m_ScrollRatio.m_Y = 1.0;
        else if (m_ScrollInfo.m_Y == g_FrameMan.GetPlayerScreenHeight())
            m_ScrollRatio.m_Y = m_pMainTextureHeight - g_FrameMan.GetPlayerScreenHeight();
        else if (m_pMainTextureHeight == g_FrameMan.GetPlayerScreenHeight())
            m_ScrollRatio.m_Y = 1.0f / (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerScreenHeight());
        else
            m_ScrollRatio.m_Y = (float)(m_pMainTextureHeight - g_FrameMan.GetPlayerScreenHeight()) /
                                (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerScreenHeight());
    }

    // Establish the scaled dimensions of this
    m_ScaledDimensions.SetXY(m_pMainTextureWidth * m_ScaleFactor.m_X, m_pMainTextureHeight * m_ScaleFactor.m_Y);
}


void SceneLayer::UpdateScrollRatiosForNetworkPlayer(int player)
{
	if (m_WrapX)
		m_ScrollRatio.m_X = m_ScrollInfo.m_X;
	else
	{
		if (m_ScrollInfo.m_X == -1.0 || m_ScrollInfo.m_X == 1.0)
			m_ScrollRatio.m_X = 1.0;
		else if (m_ScrollInfo.m_X == g_FrameMan.GetPlayerFrameBufferWidth(player))
			m_ScrollRatio.m_X = m_pMainTextureWidth - g_FrameMan.GetPlayerFrameBufferWidth(player);
		else if (m_pMainTextureWidth == g_FrameMan.GetPlayerFrameBufferWidth(player))
			m_ScrollRatio.m_X = 1.0f / (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerFrameBufferWidth(player));
		else
			m_ScrollRatio.m_X = (float)(m_pMainTextureWidth - g_FrameMan.GetPlayerFrameBufferWidth(player)) /
			(float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerFrameBufferWidth(player));
	}

	if (m_WrapY)
		m_ScrollRatio.m_Y = m_ScrollInfo.m_Y;
	else
	{
		if (m_ScrollInfo.m_Y == -1.0 || m_ScrollInfo.m_Y == 1.0)
			m_ScrollRatio.m_Y = 1.0;
		else if (m_ScrollInfo.m_Y == g_FrameMan.GetPlayerFrameBufferHeight(player))
			m_ScrollRatio.m_Y = m_pMainTextureHeight - g_FrameMan.GetPlayerFrameBufferHeight(player);
		else if (m_pMainTextureHeight == g_FrameMan.GetPlayerFrameBufferHeight(player))
			m_ScrollRatio.m_Y = 1.0f / (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerFrameBufferHeight(player));
		else
			m_ScrollRatio.m_Y = (float)(m_pMainTextureHeight - g_FrameMan.GetPlayerFrameBufferHeight(player)) /
			(float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerFrameBufferHeight(player));
	}

	// Establish the scaled dimensions of this
	m_ScaledDimensions.SetXY(m_pMainTextureWidth * m_ScaleFactor.m_X, m_pMainTextureHeight * m_ScaleFactor.m_Y);
}


} // namespace RTE
