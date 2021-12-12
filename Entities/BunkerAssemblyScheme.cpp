//////////////////////////////////////////////////////////////////////////////////////////
// File:            BunkerAssemblyScheme.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the BunkerAssemblyScheme class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "BunkerAssemblyScheme.h"
#include "PresetMan.h"
#include "FrameMan.h"

#include "GUI/GUI.h"
#include "GUI/SDLGUITexture.h"

#include "System/Color.h"
#include "System/SDLHelper.h"
#include "RTERenderer.h"
#include "GraphicalPrimitive.h"

namespace RTE {

ConcreteClassInfo(BunkerAssemblyScheme, SceneObject, 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this BunkerAssemblyScheme, effectively
//                  resetting the members of this abstraction level only.

void BunkerAssemblyScheme::Clear()
{
	m_pPresentationBitmap = 0;
    m_ChildObjects.clear();
	m_BitmapOffset = Vector(0,0);
	m_IsOneTypePerScene = false;
	m_Limit = 0;
	m_MaxDeployments = 1;
	m_SymmetricScheme.clear();
	m_AssemblyGroup.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the BunkerAssemblyScheme object ready for use.

int BunkerAssemblyScheme::Create()
{
    if (SceneObject::Create() < 0)
        return -1;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a MOPixel to be identical to another, by deep copy.

int BunkerAssemblyScheme::Create(const BunkerAssemblyScheme &reference)
{
    SceneObject::Create(reference);

    m_pBitmap = reference.m_pBitmap;
    m_pPresentationBitmap = reference.m_pPresentationBitmap;
	m_pIconBitmap = reference.m_pIconBitmap;

    for (list<SOPlacer>::const_iterator itr = reference.m_ChildObjects.begin(); itr != reference.m_ChildObjects.end(); ++itr)
        m_ChildObjects.push_back(*itr);

	m_BitmapOffset = reference.m_BitmapOffset;

	m_IsOneTypePerScene = reference.m_IsOneTypePerScene;
	m_Limit = reference.m_Limit;
	m_MaxDeployments = reference.m_MaxDeployments;
	m_SymmetricScheme = reference.m_SymmetricScheme;
	m_AssemblyGroup = reference.m_AssemblyGroup;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int BunkerAssemblyScheme::ReadProperty(const std::string_view &propName, Reader &reader)
{
    if (propName == "BitmapFile")
    {
        reader >> m_BitmapFile;
        m_pBitmap = m_BitmapFile.GetAsTexture();

		m_pPresentationBitmap = MakeTexture();
		m_pPresentationBitmap->Create(m_pBitmap->GetW() * ScaleX, m_pBitmap->GetH() * ScaleY, BitDepth::Indexed8, g_FrameMan.GetDefaultPalette());

		// Create internal presentation bitmap which will be drawn by editor
		// Create horizontal outlines
        for (int x = 0; x < m_pBitmap->GetW(); ++x)
		{
			SDL_Rect line{x*ScaleX, 0, ScaleX - 1, 1};
	        //Top to bottom
			for (int y = 0; y < m_pBitmap->GetH() ; ++y)
            {
				line.y = y * ScaleY;
				line.h = SchemeWidth;
				int px = m_pBitmap->GetPixel(x, y);
				int pxp = m_pBitmap->GetPixel(x, y - 1) == Surface::PixelOutside ? SCHEME_COLOR_EMPTY : m_pBitmap->GetPixel(x, y - 1);

				if (px == SCHEME_COLOR_WALL && pxp != SCHEME_COLOR_WALL)
					SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_WALL);

				if (px == SCHEME_COLOR_PASSABLE && pxp != SCHEME_COLOR_PASSABLE)
					SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_PASSABLE);

				if (px == SCHEME_COLOR_VARIABLE && pxp != SCHEME_COLOR_VARIABLE)
					SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_VARIABLE);
            }

			//Bottom to top
	        for (int y = m_pBitmap->GetH() - 1; y >= 0 ; --y)
            {
				int px = m_pBitmap->GetPixel(x, y);
				int pxp = m_pBitmap->GetPixel(x, y + 1) == Surface::PixelOutside ? SCHEME_COLOR_EMPTY : m_pBitmap->GetPixel(x, y + 1);
				line.y = y * ScaleY + ScaleX - 1;
				line.h = 1;

				if (px == SCHEME_COLOR_WALL && pxp != SCHEME_COLOR_WALL)
					for (int w = 0; w < SchemeWidth; w++) {
						line.y = y * ScaleY + ScaleX - 1 - w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_WALL);
					}

				if (px == SCHEME_COLOR_PASSABLE && pxp != SCHEME_COLOR_PASSABLE)
					for (int w = 0; w < SchemeWidth; w++) {
						line.y = y * ScaleY + ScaleX - 1 - w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_PASSABLE);
					}

				if (px == SCHEME_COLOR_VARIABLE && pxp != SCHEME_COLOR_VARIABLE)
					for (int w = 0; w < SchemeWidth; w++) {
						line.y = y * ScaleY + ScaleX - 1 - w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_VARIABLE);
					}
            }
		}

		// Create vertical outlines
        for (int y = 0; y < m_pBitmap->GetH(); ++y)
		{
			SDL_Rect line{0, y * ScaleY, 1, ScaleY - 1};
			// Left
	        for (int x = 0; x < m_pBitmap->GetW() ; ++x)
            {
				uint32_t px = m_pBitmap->GetPixel(x, y);
				uint32_t pxp = m_pBitmap->GetPixel(x - 1, y) == Surface::PixelOutside ? SCHEME_COLOR_EMPTY : m_pBitmap->GetPixel(x - 1, y);

				if (px == SCHEME_COLOR_WALL && pxp != SCHEME_COLOR_WALL)
					for (int w = 0; w < SchemeWidth; w++){
						line.x = x * ScaleX + w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_WALL);
					}

				if (px == SCHEME_COLOR_PASSABLE && pxp != SCHEME_COLOR_PASSABLE)
					for (int w = 0; w < SchemeWidth; w++) {
						line.x = x * ScaleX + w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_PASSABLE);
					}

				if (px == SCHEME_COLOR_VARIABLE && pxp != SCHEME_COLOR_VARIABLE)
					for (int w = 0; w < SchemeWidth; w++) {
						line.x = x * ScaleX + w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_VARIABLE);
					}
			}

	        for (int x = m_pBitmap->GetW() - 1; x >= 0 ; --x)
            {
				uint32_t px = m_pBitmap->GetPixel(x, y);
				uint32_t pxp = m_pBitmap->GetPixel(x + 1, y) == Surface::PixelOutside ? SCHEME_COLOR_EMPTY : m_pBitmap->GetPixel(x + 1, y);

				if (px == SCHEME_COLOR_WALL && pxp != SCHEME_COLOR_WALL)
					for (int w = 0; w < SchemeWidth; w++) {
						line.x = x * ScaleX - w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_WALL);
					}

				if (px == SCHEME_COLOR_PASSABLE && pxp != SCHEME_COLOR_PASSABLE)
					for (int w = 0; w < SchemeWidth; w++) {
						line.x = x * ScaleX - w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_PASSABLE);
					}

				if (px == SCHEME_COLOR_VARIABLE && pxp != SCHEME_COLOR_VARIABLE)
					for (int w = 0; w < SchemeWidth; w++) {
						line.x = x * ScaleX - w;
						SDL_FillRect(m_pPresentationBitmap->GetPixels(), &line, PAINT_COLOR_VARIABLE);
					}
            }
		}

		// Print scheme name
		GUIFont *pSmallFont = g_FrameMan.GetSmallFont();
		SDLGUITexture guiBitmap;
		pSmallFont->DrawAligned(&guiBitmap, 4, 4, m_PresetName, GUIFont::Left);

		// Calculate bitmap offset
		int width = m_pBitmap->GetW() / 2;
		int height = m_pBitmap->GetH() / 2;
		m_BitmapOffset = Vector(-width * ScaleX, -height * ScaleY);

		// Count max deployments if not set
		if (m_MaxDeployments == 0)
		{
			m_MaxDeployments = (int)((float)GetArea() / AREA_PER_DEPLOYMENT);
			if (m_MaxDeployments == 0)
				m_MaxDeployments = 1;
		}

		float scale = (float)ICON_WIDTH / (float)m_pPresentationBitmap->GetW();

		m_pIconBitmap = MakeTexture();
		m_pIconBitmap->Create(m_pPresentationBitmap->GetW() * scale, m_pPresentationBitmap->GetH() * scale, BitDepth::Indexed8, g_FrameMan.GetDefaultPalette());

		for (int x = 0; x < m_pBitmap->GetW() ; ++x)
	        for (int y = 0; y < m_pBitmap->GetH(); ++y)
            {
				SDL_Rect fill{static_cast<int>(x * ScaleX * scale), static_cast<int>(y * ScaleX * scale), ScaleX - 1, ScaleY - 1};
				uint32_t px = m_pBitmap->GetPixel(x, y);

				if (px == SCHEME_COLOR_WALL) {
					SDL_FillRect(m_pIconBitmap->GetPixels(), &fill, PAINT_COLOR_WALL);
				} else if (px == SCHEME_COLOR_PASSABLE)
					SDL_FillRect(m_pIconBitmap->GetPixels(), &fill, PAINT_COLOR_PASSABLE);
				else if (px == SCHEME_COLOR_VARIABLE)
					SDL_FillRect(m_pIconBitmap->GetPixels(), &fill, PAINT_COLOR_VARIABLE);
			}
	}
    else if (propName == "AddChildObject")
    {
        SOPlacer newChild;
        reader >> newChild;
        newChild.SetTeam(m_Team);
        m_ChildObjects.push_back(newChild);
    }
    else if (propName == "Limit")
		reader >> m_Limit;
    else if (propName == "OneTypePerScene")
		reader >> m_IsOneTypePerScene;
    else if (propName == "MaxDeployments")
		reader >> m_MaxDeployments;
    else if (propName == "SymmetricScheme")
		reader >> m_SymmetricScheme;
    else if (propName == "AssemblyGroup")
		reader >> m_AssemblyGroup;
    else
        return SceneObject::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this BunkerAssemblyScheme with a Writer for
//                  later recreation with Create(Reader &reader);

int BunkerAssemblyScheme::Save(Writer &writer) const
{
    SceneObject::Save(writer);

    writer.NewProperty("BitmapFile");
    writer << m_BitmapFile;
    for (list<SOPlacer>::const_iterator itr = m_ChildObjects.begin(); itr != m_ChildObjects.end(); ++itr)
    {
        writer.NewProperty("AddChildObject");
        writer << (*itr);
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the BunkerAssemblyScheme object.

void BunkerAssemblyScheme::Destroy(bool notInherited)
{
	// Probably no need to delete those, as bitmaps are only created when preset is read from file
	// and then they just copy pointers in via Clone()
	//delete m_pPresentationBitmap;
	//m_pPresentationBitmap = 0;

    if (!notInherited)
        SceneObject::Destroy();
    Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetGraphicalIcon
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a bitmap showing a good identifyable icon of this, for use in
//                  GUI lists etc.

const SharedTexture BunkerAssemblyScheme::GetGraphicalIcon() const
{
	return m_pIconBitmap;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  IsOnScenePoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this' current graphical representation overlaps
//                  a point in absolute scene coordinates.

bool BunkerAssemblyScheme::IsOnScenePoint(Vector &scenePoint) const
{
    if (!m_pBitmap)
        return false;

    Vector bitmapPos = m_Pos + m_BitmapOffset;
    if (WithinBox(scenePoint, bitmapPos, m_pPresentationBitmap->GetW(), m_pPresentationBitmap->GetH()))
    {
        // Scene point on the bitmap
        Vector bitmapPoint = scenePoint - bitmapPos;

		int x = bitmapPoint.m_X / ScaleX;
		int y = bitmapPoint.m_Y / ScaleY;

        if (m_pBitmap->GetPixel(x, y) != SCHEME_COLOR_EMPTY)
           return true;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetTeam
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets which team this Actor belongs to.

void BunkerAssemblyScheme::SetTeam(int team)
{
    SceneObject::SetTeam(team);

    // Make sure all the objects to be placed will be of the same team
    for (list<SOPlacer>::iterator itr = m_ChildObjects.begin(); itr != m_ChildObjects.end(); ++itr)
        (*itr).SetTeam(team);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this BunkerAssemblyScheme's current graphical representation to a
//                  BITMAP of choice.

void BunkerAssemblyScheme::Draw(RenderTarget *renderer, const Vector &targetPos, DrawMode mode, bool onlyPhysical, int alphaMod) const
{
    if (!m_pPresentationBitmap)
        RTEAbort("BunkerAssemblyScheme's bitmaps are null when drawing!");

	glm::vec2 viewport = renderer->GetViewport();

    // Take care of wrapping situations
    Vector aDrawPos[4];
    aDrawPos[0] = m_Pos - targetPos + m_BitmapOffset;
    int passes = 1;

    // See if need to double draw this across the scene seam if we're being drawn onto a scenewide bitmap
	if (targetPos.IsZero() && g_SceneMan.GetSceneWidth() <= viewport.x)
    {
        if (aDrawPos[0].m_X < m_pPresentationBitmap->GetW())
        {
            aDrawPos[passes] = aDrawPos[0];
            aDrawPos[passes].m_X += viewport.x;
            passes++;
        }
        else if (aDrawPos[0].m_X > viewport.x - m_pPresentationBitmap->GetW())
        {
            aDrawPos[passes] = aDrawPos[0];
            aDrawPos[passes].m_X -= viewport.x;
            passes++;
        }
    }
    // Only screenwide target bitmap, so double draw within the screen if the screen is straddling a scene seam
    else
    {
        if (g_SceneMan.SceneWrapsX())
        {
            int sceneWidth = g_SceneMan.GetSceneWidth();
            if (targetPos.m_X < 0)
            {
                aDrawPos[passes] = aDrawPos[0];
                aDrawPos[passes].m_X -= sceneWidth;
                passes++;
            }
            if (targetPos.m_X + viewport.x > sceneWidth)
            {
                aDrawPos[passes] = aDrawPos[0];
                aDrawPos[passes].m_X += sceneWidth;
                passes++;
            }
        }
    }

    // Draw all the passes needed
    for (int i = 0; i < passes; ++i)
    {
        if (mode == g_DrawColor)
            m_pPresentationBitmap->render(renderer, aDrawPos[i].GetFloorIntX(), aDrawPos[i].GetFloorIntY());
        else if (mode == g_DrawMaterial)
            m_pPresentationBitmap->render(renderer, aDrawPos[i].GetFloorIntX(), aDrawPos[i].GetFloorIntY());
        else if (mode == g_DrawLess)
            m_pPresentationBitmap->render(renderer, aDrawPos[i].GetFloorIntX(), aDrawPos[i].GetFloorIntY());
        else if (mode == g_DrawTrans)
			m_pPresentationBitmap->setAlphaMod(alphaMod);
		m_pPresentationBitmap->render(renderer, aDrawPos[i].GetFloorIntX(), aDrawPos[i].GetFloorIntY());
			m_pPresentationBitmap->setAlphaMod(255);
    }
}

} // namespace RTE
