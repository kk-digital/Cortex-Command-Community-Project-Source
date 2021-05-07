//////////////////////////////////////////////////////////////////////////////////////////
// File:            SLTerrain.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the SLTerrain class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "SLTerrain.h"
#include "TerrainDebris.h"
#include "TerrainObject.h"
#include "PresetMan.h"
#include "DataModule.h"
#include "SceneObject.h"
#include "MOPixel.h"
#include "MOSprite.h"
#include "Atom.h"

#include "FrameMan.h"

#include "System/SDLHelper.h"
#include <SDL2/SDL2_rotozoom.h>

namespace RTE {

ConcreteClassInfo(SLTerrain, SceneLayer, 0)

const string SLTerrain::TerrainFrosting::c_ClassName = "TerrainFrosting";



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this TerrainFrosting, effectively
//                  resetting the members of this abstraction level only.

void SLTerrain::TerrainFrosting::Clear()
{
    m_TargetMaterial.Reset();
    m_FrostingMaterial.Reset();
    m_MinThickness = 5;
    m_MaxThickness = 5;
    m_InAirOnly = true;
}

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the TerrainFrosting object ready for use.

int SLTerrain::TerrainFrosting::Create()
{
    if (Serializable::Create() < 0)
        return -1;

    return 0;
}
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a TerrainFrosting to be identical to another, by deep copy.

int SLTerrain::TerrainFrosting::Create(const TerrainFrosting &reference)
{
    m_TargetMaterial = reference.m_TargetMaterial;
    m_FrostingMaterial = reference.m_FrostingMaterial;
    m_MinThickness = reference.m_MinThickness;
    m_MaxThickness = reference.m_MaxThickness;
    m_InAirOnly = reference.m_InAirOnly;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int SLTerrain::TerrainFrosting::ReadProperty(const std::string_view &propName, Reader &reader)
{
    if (propName == "TargetMaterial")
        reader >> m_TargetMaterial;
    else if (propName == "FrostingMaterial")
        reader >> m_FrostingMaterial;
    else if (propName == "MinThickness")
        reader >> m_MinThickness;
    else if (propName == "MaxThickness")
        reader >> m_MaxThickness;
    else if (propName == "InAirOnly")
        reader >> m_InAirOnly;
    else
        return Serializable::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this TerrainFrosting with a Writer for
//                  later recreation with Create(Reader &reader);

int SLTerrain::TerrainFrosting::Save(Writer &writer) const
{
    Serializable::Save(writer);

    writer.NewProperty("TargetMaterial");
    writer << m_TargetMaterial;
    writer.NewProperty("FrostingMaterial");
    writer << m_FrostingMaterial;
    writer.NewProperty("MinThickness");
    writer << m_MinThickness;
    writer.NewProperty("MaxThickness");
    writer << m_MaxThickness;
    writer.NewProperty("InAirOnly");
    writer << m_InAirOnly;

    return 0;
}


std::unique_ptr<Texture> SLTerrain::s_TempRenderTarget;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this SLTerrain, effectively
//                  resetting the members of this abstraction level only.

void SLTerrain::Clear()
{
    m_pFGColor = 0;
    m_pBGColor = 0;
    m_pStructural = 0;
    m_BGTextureFile.Reset();
    m_TerrainFrostings.clear();
    m_TerrainDebris.clear();
    m_TerrainObjects.clear();
    m_UpdatedMateralAreas.clear();
    m_DrawMaterial = false;
	m_NeedToClearFrostings = false;
	m_NeedToClearDebris = false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the SLTerrain object ready for use.

int SLTerrain::Create()
{
    if (SceneLayer::Create() < 0)
        return -1;

    if (!m_pFGColor)
        m_pFGColor = new SceneLayer();
    if (!m_pBGColor)
        m_pBGColor = new SceneLayer();

    return 0;
}

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the SLTerrain object ready for use.

int SLTerrain::Create(char *filename,
                    bool drawTrans,
                    Vector offset,
                    bool wrapX,
                    bool wrapY,
                    Vector scrollInfo)
{
    //SceneLayer::Create();

    return 0;
}
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a SLTerrain to be identical to another, by deep copy.

int SLTerrain::Create(const SLTerrain &reference)
{
    SceneLayer::Create(reference);

    // Leave these because they are loaded late by LoadData
    m_pFGColor = dynamic_cast<SceneLayer *>(reference.m_pFGColor->Clone());
    m_pBGColor = dynamic_cast<SceneLayer *>(reference.m_pBGColor->Clone());
//    m_pStructural;
    m_BGTextureFile = reference.m_BGTextureFile;

    ////////////////////////////
    // Frostings
    m_TerrainFrostings.clear();
    for (list<TerrainFrosting>::const_iterator tfItr = reference.m_TerrainFrostings.begin(); tfItr != reference.m_TerrainFrostings.end(); ++tfItr)
        m_TerrainFrostings.push_back(TerrainFrosting((*tfItr)));

    //////////////////////
    // Debris
    // First have to clear out what we've got
    for (list<TerrainDebris *>::iterator tdItr = m_TerrainDebris.begin(); tdItr != m_TerrainDebris.end(); ++tdItr)
        delete (*tdItr);
    m_TerrainDebris.clear();
    // Then add the copies
    for (list<TerrainDebris *>::const_iterator tdItr = reference.m_TerrainDebris.begin(); tdItr != reference.m_TerrainDebris.end(); ++tdItr)
        m_TerrainDebris.push_back(dynamic_cast<TerrainDebris *>((*tdItr)->Clone()));

    /////////////////
    // Objects
    // First clear out what we've got
    for (list<TerrainObject *>::iterator toItr = m_TerrainObjects.begin(); toItr != m_TerrainObjects.end(); ++toItr)
        delete (*toItr);
    m_TerrainObjects.clear();
    // Then add the copies
    for (list<TerrainObject *>::const_iterator toItr = reference.m_TerrainObjects.begin(); toItr != reference.m_TerrainObjects.end(); ++toItr)
        m_TerrainObjects.push_back(dynamic_cast<TerrainObject *>((*toItr)->Clone()));

    m_DrawMaterial = reference.m_DrawMaterial;

	m_NeedToClearFrostings = true;
	m_NeedToClearDebris = true;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  LoadData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Actually loads previously specified/created data into memory. Has
//                  to be done before using this SceneLayer.

int SLTerrain::LoadData()
{
    // Load the materials bitmap into the main bitmap
    if (SceneLayer::LoadData())
        return -1;

    RTEAssert(m_pFGColor, "Terrain's foreground layer not instantiated before trying to load its data!");
    RTEAssert(m_pBGColor, "Terrain's background layer not instantiated before trying to load its data!");

    // Check if our color layers' BITMAP data is also to be loaded from disk, and not be generated from the material bitmap!
    if (m_pFGColor->IsFileData() && m_pBGColor->IsFileData())
    {
        if (m_pFGColor->LoadData() < 0)
        {
            RTEAbort("Could not load the Foreground Color SceneLayer data from file, when a path was specified for it!");
            return -1;
        }
        if (m_pBGColor->LoadData() < 0)
        {
            RTEAbort("Could not load the Background Color SceneLayer data from file, when a path was specified for it!");
            return -1;
        }
        // Ok, we have now loaded the layers in from files, don't need to generate them from the material layer
        InitScrollRatios();
        return 0;
    }

    // Create blank foreground layer
    m_pFGColor->Destroy();

    Texture pFGTexture(g_FrameMan.GetRenderer(), m_pMainTexture->getW(), m_pMainTexture->getH(), SDL_TEXTUREACCESS_STREAMING);
    if (!pFGTexture || m_pFGColor->Create(pFGTexture, true, m_Offset, m_WrapX, m_WrapY, m_ScrollRatio))
    {
        RTEAbort("Failed to create terrain's foreground layer's bitmap!");
        return -1;
    }

    // Create blank background layer
    m_pBGColor->Destroy();
    Texture pBGTexture(g_FrameMan.GetRenderer(), m_pMainTexture->getW(), m_pMainTexture->getH(), SDL_TEXTUREACCESS_STREAMING);
    if (!pBGTexture || m_pBGColor->Create(pBGTexture, true, m_Offset, m_WrapX, m_WrapY, m_ScrollRatio))
    {
        RTEAbort("Failed to create terrain's background layer's bitmap!");
        return -1;
    }

    // Structural integrity calc buffer bitmap
	m_pStructural = std::make_unique<Texture>(
		g_FrameMan.GetRenderer(), m_pMainTexture->getW(),
		m_pMainTexture->getH(), SDL_TEXTUREACCESS_STREAMING);
	RTEAssert(m_pStructural.get(),
		      "Failed to allocate BITMAP in Terrain::Create");

	///////////////////////////////////////////////
    // Load and texturize the FG color bitmap, based on the materials defined in the recently loaded (main) material layer!

    int xPos, yPos, matIndex;
	uint32_t pixelColor;

    // Temporary references for all the materials' textures and colors, since we'll access them a lot
	std::array<std::shared_ptr<Texture>,256> apTexTextures;
	std::array<int,256> aColors;
    // Null em out so we can tell which ones we've already got once so far
	apTexTextures.fill(0);
	aColors.fill(0);

    // Get the background texture
	std::shared_ptr<Texture> m_pBGTexture = m_BGTextureFile.GetAsTexture();
    // Get the material palette for quicker access
    Material **apMaterials = g_SceneMan.GetMaterialPalette();
    // Get the Material palette ID mappings local to the DataModule this SLTerrain is loaded from
    const std::array<uint32_t, c_PaletteEntriesNumber> materialMappings = g_PresetMan.GetDataModule(m_TextureFile.GetDataModuleID())->GetAllMaterialMappings();
    Material *pMaterial = nullptr;

    // Lock all involved bitmaps
    m_pMainTexture->lock();
	m_pFGColor->LockTexture();
	m_pBGColor->LockTexture();
	m_pBGTexture->lock();

    // Go through each pixel on the main bitmap, which contains all the material pixels loaded from the bitmap
    // Place texture pixels on the FG layer corresponding to the materials on the main material bitmap
    for (xPos = 0; xPos < m_pMainTexture->getW(); ++xPos)
    {
        for (yPos = 0; yPos < m_pMainTexture->getH(); ++yPos)
        {
            // Read which material the current pixel represents
            matIndex = m_pMainTexture->getPixel(xPos, yPos);
            // Map any materials defined in this data module but initially collided with other material ID's and thus were displaced to other ID's
            if (materialMappings[matIndex] != 0)
            {
                // Assign the mapping and put it onto the material bitmap too
                matIndex = materialMappings[matIndex];
                m_pMainTexture->setPixel(xPos, yPos, matIndex);
            }

            // Validate the material, or default to default material
            if (matIndex >= 0 && matIndex < c_PaletteEntriesNumber && apMaterials[matIndex])
                pMaterial = apMaterials[matIndex];
            else
                pMaterial = apMaterials[g_MaterialDefault];

            // If haven't read a pixel of this material before, then get its texture so we can quickly access it
            if (!apTexTextures[matIndex])
            {
                // Get, and acquire the texture bitmap if material has any
                if (apTexTextures[matIndex] = pMaterial->GetTexture())
                    apTexTextures[matIndex]->lock();
            }

            // If actually no texture for the material, then use the material's solid color instead
            if (!apTexTextures[matIndex])
            {
                // If the color hasn't been retrieved yet, then do so
                if (!aColors[matIndex])
                    aColors[matIndex] = pMaterial->GetColor().GetIndex();
                // Use the color
                pixelColor = aColors[matIndex];
            }
            // Use the texture's color
            else
            {
//                acquire_bitmap(apTexTextures[matIndex]);
                pixelColor = apTexTextures[matIndex]->getPixel(xPos % apTexTextures[matIndex]->getW(), yPos % apTexTextures[matIndex]->getH());
            }

            // Draw the correct color pixel on the foreground
            m_pFGColor->SetPixel(xPos, yPos, pixelColor);

            // Draw background texture on the background where this is stuff on the foreground
            if (m_pBGTexture && pixelColor != g_MaskColor)
            {
                pixelColor = m_pBGTexture->getPixel(xPos % m_pBGTexture->getW(), yPos % m_pBGTexture->getH());
                m_pBGColor->SetPixel(xPos, yPos, pixelColor);
            }
            // Put a keycolor pixel in the bg otherwise
            else
                m_pBGColor->SetPixel(xPos, yPos, g_AlphaZero);
        }
    }

    ///////////////////////////////////////
    // Material frostings application!

    bool targetFound = false, applyingFrosting = false;
    int targetId, frostingId, thickness, thicknessGoal;
	std::shared_ptr<Texture> pFrostingTex;
    for (list<TerrainFrosting>::iterator tfItr = m_TerrainFrostings.begin(); tfItr != m_TerrainFrostings.end(); ++tfItr)
    {
        targetId = (*tfItr).GetTargetMaterial().GetIndex();
        frostingId = (*tfItr).GetFrostingMaterial().GetIndex();
        // Try to get the color texture of the frosting material. If fail, we'll use the color isntead
        pFrostingTex = (*tfItr).GetFrostingMaterial().GetTexture();
        if (pFrostingTex)
            pFrostingTex->lock();

        // Loop through all columns
        for (xPos = 0; xPos < m_pMainTexture->getW(); ++xPos)
        {
            // Get the thickness for this column
            thicknessGoal = (*tfItr).GetThicknessSample();

            // Work upward from the bottom of each column
            for (yPos = m_pMainTexture->getH() - 1; yPos >= 0; --yPos)
            {
                // Read which material the current pixel represents
                matIndex = m_pMainTexture->getPixel(xPos, yPos);

                // We've encountered the target material! Prepare to apply frosting as soon as it ends!
                if (!targetFound && matIndex == targetId)
                {
                    targetFound = true;
                    thickness = 0;
                }
                // Target material has ended! See if we shuold start putting on the frosting
                else if (targetFound && matIndex != targetId && thickness <= thicknessGoal)
                {
                    applyingFrosting = true;
                    targetFound = false;
                }

                // If time to put down frosting pixels, then do so IF there is air, OR we're set to ignore what we're overwriting
                if (applyingFrosting && (matIndex == g_MaterialAir || !(*tfItr).InAirOnly()) && thickness <= thicknessGoal)
                {
                    // Get the color either from the frosting material's texture or the solid color
                    if (pFrostingTex)
                        pixelColor = pFrostingTex->getPixel(xPos % pFrostingTex->getW(), yPos % pFrostingTex->getH());
                    else
                        pixelColor = (*tfItr).GetFrostingMaterial().GetColor().GetIndex();

                    // Put the frosting pixel color on the FG color layer
                    m_pFGColor->SetPixel(xPos, yPos, pixelColor);
                    // Put the material ID pixel on the material layer
                    m_pMainTexture->setPixel(xPos, yPos, materialMappings[frostingId]);

                    // Keep track of the applied thickness
                    thickness++;
                }
                else
                    applyingFrosting = false;
            }
        }

        if (pFrostingTex)
            pFrostingTex->unlock();
    }

    // Release all involved bitmaps
    m_pMainTexture->unlock();
	m_pFGColor->UnlockTexture();
	m_pBGColor->UnlockTexture();
    m_pBGTexture->unlock();

    for (matIndex = 0; matIndex < 256; ++matIndex)
    {
        if (apTexTextures[matIndex])
            apTexTextures[matIndex]->unlock();
    }

    ///////////////////////////////////////////////
    // TerrainDebris application

    for (list<TerrainDebris *>::iterator tdItr = m_TerrainDebris.begin(); tdItr != m_TerrainDebris.end(); ++tdItr)
    {
        (*tdItr)->ApplyDebris(this);
    }

    ///////////////////////////////////////////////
    // Now take care of the TerrainObjects

    for (list<TerrainObject *>::iterator toItr = m_TerrainObjects.begin(); toItr != m_TerrainObjects.end(); ++toItr)
    {
        ApplyTerrainObject(*toItr);
    }
    CleanAir();

    InitScrollRatios();

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SaveData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves currently data in memory to disk.

int SLTerrain::SaveData(string pathBase)
{
    if (pathBase.empty())
        return -1;

    // Save the bitmap of the material bitmap
    if (SceneLayer::SaveData(pathBase + " Mat.bmp") < 0)
    {
        RTEAbort("Failed to write the material bitmap data saving an SLTerrain!");
        return -1;
    }
    // Then the foreground color layer
    if (m_pFGColor->SaveData(pathBase + " FG.bmp") < 0)
    {
        RTEAbort("Failed to write the FG color bitmap data saving an SLTerrain!");
        return -1;
    }
    // Then the background color layer
    if (m_pBGColor->SaveData(pathBase + " BG.bmp") < 0)
    {
        RTEAbort("Failed to write the BG color bitmap data saving an SLTerrain!");
        return -1;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ClearData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears out any previously loaded bitmap data from memory.

int SLTerrain::ClearData()
{
    // Clear the material layer
    if (SceneLayer::ClearData() < 0)
    {
        RTEAbort("Failed to clear material bitmap data of an SLTerrain!");
        return -1;
    }
    // Clear the foreground color layer
    if (m_pFGColor && m_pFGColor->ClearData() < 0)
    {
        RTEAbort("Failed to clear the foreground color bitmap data of an SLTerrain!");
        return -1;
    }
    // Clear the background color layer
    if (m_pBGColor && m_pBGColor->ClearData() < 0)
    {
        RTEAbort("Failed to clear the background color bitmap data of an SLTerrain!");
        return -1;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int SLTerrain::ReadProperty(const std::string_view &propName, Reader &reader)
{
    if (propName == "BackgroundTexture")
        reader >> m_BGTextureFile;
    else if (propName == "FGColorLayer")
    {
        delete m_pFGColor;
        m_pFGColor = new SceneLayer();
        reader >> m_pFGColor;
    }
    else if (propName == "BGColorLayer")
    {
        delete m_pBGColor;
        m_pBGColor = new SceneLayer();
        reader >> m_pBGColor;
    }
    else if (propName == "AddTerrainFrosting")
    {
		// Clear frostings if we derived them from some other SLTerrain object
		// and then read another set from explicit terrain definition
		if (m_NeedToClearFrostings)
		{
			m_NeedToClearFrostings = false;
			m_TerrainFrostings.clear();
		}

        TerrainFrosting frosting;
        reader >> frosting;
        m_TerrainFrostings.push_back(frosting);
    }
    else if (propName == "AddTerrainDebris")
    {
		// Clear debris if we derived them from some other SLTerrain object
		// and then read another set from explicit terrain definition
		if (m_NeedToClearDebris)
		{
			m_NeedToClearDebris = false;
			m_TerrainDebris.clear();
		}

        TerrainDebris *pTerrainDebris = new TerrainDebris;
        reader >> pTerrainDebris;
        m_TerrainDebris.push_back(pTerrainDebris);
    }
    else if (propName == "AddTerrainObject" || propName == "PlaceTerrainObject")
    {
        TerrainObject *pTerrainObject = new TerrainObject;
        reader >> pTerrainObject;
        m_TerrainObjects.push_back(pTerrainObject);
    }
    else
        return SceneLayer::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this SceneLayer with a Writer for
//                  later recreation with Create(Reader &reader);

int SLTerrain::Save(Writer &writer) const
{
    SceneLayer::Save(writer);

    // Only write the background texture info if the background itself is not saved out as a file already
    if (m_pBGColor->IsFileData())
    {
        writer.NewProperty("BGColorLayer");
        writer << m_pBGColor;
    }
    else
    {
        writer.NewProperty("BackgroundTexture");
        writer << m_BGTextureFile;
    }

    // Only if we haven't saved out the FG as a done and altered bitmap file should we save the procedural params here
    if (m_pFGColor->IsFileData())
    {
        writer.NewProperty("FGColorLayer");
        writer << m_pFGColor;
    }
    // Layer data is not saved into a bitmap file yet, so just write out the procedural params to build the terrain
    else
    {
        for (list<TerrainFrosting>::const_iterator tfItr = m_TerrainFrostings.begin(); tfItr != m_TerrainFrostings.end(); ++tfItr)
        {
            writer.NewProperty("AddTerrainFrosting");
            writer << *tfItr;
        }
        for (list<TerrainDebris *>::const_iterator tdItr = m_TerrainDebris.begin(); tdItr != m_TerrainDebris.end(); ++tdItr)
        {
            writer.NewProperty("AddTerrainDebris");
            writer << *tdItr;
        }
        for (list<TerrainObject *>::const_iterator toItr = m_TerrainObjects.begin(); toItr != m_TerrainObjects.end(); ++toItr)
        {
            // Write out only what is needed to place a copy of this in the Terrain
            writer.NewProperty("AddTerrainObject");
            writer.ObjectStart((*toItr)->GetClassName());
            writer.NewProperty("CopyOf");
            writer << (*toItr)->GetModuleAndPresetName();
            writer.NewProperty("Position");
            writer << (*toItr)->GetPos();
            writer.ObjectEnd();
        }
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the SLTerrain object.

void SLTerrain::Destroy(bool notInherited)
{
    delete m_pFGColor;
    delete m_pBGColor;

    for (list<TerrainDebris *>::iterator tdItr = m_TerrainDebris.begin(); tdItr != m_TerrainDebris.end(); ++tdItr)
    {
        delete (*tdItr);
        *tdItr = 0;
    }
    for (list<TerrainObject *>::iterator toItr = m_TerrainObjects.begin(); toItr != m_TerrainObjects.end(); ++toItr)
    {
        delete (*toItr);
        *toItr = 0;
    }

    if (!notInherited)
        SceneLayer::Destroy();
    Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetFGColorPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a specific pixel from the foreground color bitmap of this.
//                  LockTextures() must be called before using this method.

uint32_t SLTerrain::GetFGColorPixel(const int pixelX, const int pixelY) const
{
    int posX = pixelX;
    int posY = pixelY;

    WrapPosition(posX, posY);

    // If it's still below or to the sides out of bounds after
    // wrapping what is supposed to be wrapped, shit is out of bounds.
    if (posX < 0 || posX >= m_pMainTexture->getW() || posY >= m_pMainTexture->getH())
        return g_AlphaZero;

    // If above terrain bitmap, return key color.
    if (posY < 0)
        return g_AlphaZero;

//    RTEAssert(m_pMainTexture->m_LockCount > 0, "Trying to access unlocked terrain bitmap");
    return m_pFGColor->GetPixel(posX, posY);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetBGColorPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a specific pixel from the background color bitmap of this.
//                  LockTextures() must be called before using this method.

uint32_t SLTerrain::GetBGColorPixel(const int pixelX, const int pixelY) const
{
    int posX = pixelX;
    int posY = pixelY;

    WrapPosition(posX, posY);

    // If it's still below or to the sides out of bounds after
    // wrapping what is supposed to be wrapped, shit is out of bounds.
    if (posX < 0 || posX >= m_pMainTexture->getW() || posY >= m_pMainTexture->getH())
        return g_AlphaZero;

    // If above terrain bitmap, return key color.
    if (posY < 0)
        return g_AlphaZero;

//    RTEAssert(m_pMainTexture->m_LockCount > 0, "Trying to access unlocked terrain bitmap");
    return m_pBGColor->GetPixel(posX, posY);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMaterialPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a specific pixel from the material bitmap of this SceneLayer.
//                  LockMaterialTexture() must be called before using this method.

uint32_t SLTerrain::GetMaterialPixel(const int pixelX, const int pixelY) const
{
    int posX = pixelX;
    int posY = pixelY;

    WrapPosition(posX, posY);

    // If it's still below or to the sides out of bounds after
    // wrapping what is supposed to be wrapped, shit is out of bounds.
    if (posX < 0 || posX >= m_pMainTexture->getW() || posY >= m_pMainTexture->getH())
//        return g_MaterialOutOfBounds;
        return g_MaterialAir;

    // If above terrain bitmap, return air material.
    if (posY < 0)
        return g_MaterialAir;

//    RTEAssert(m_pMainTexture->m_LockCount > 0, "Trying to access unlocked terrain bitmap");
    return m_pMainTexture->getPixel(posX, posY);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsAirPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether a terrain pixel is of air or cavity material.

bool SLTerrain::IsAirPixel(const int pixelX, const int pixelY) const
{
	int posX = pixelX;
	int posY = pixelY;

	WrapPosition(posX, posY);

	// If it's still below or to the sides out of bounds after
	// wrapping what is supposed to be wrapped, shit is out of bounds.
	if (posX < 0 || posX >= m_pMainTexture->getW() || posY >= m_pMainTexture->getH())
		//        return g_MaterialOutOfBounds;
		return true;

	// If above terrain bitmap, return air material.
	if (posY < 0)
		return true;

	uint32_t checkPixel = m_pMainTexture->getPixel(posX, posY);
	//    RTEAssert(m_pMainTexture->m_LockCount > 0, "Trying to access unlocked
	//    terrain bitmap");
	return checkPixel == g_MaterialAir || checkPixel == g_MaterialCavity;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetFGColorPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific pixel on the FG Color bitmap of this SLTerrain to a
//                  specific color. LockTextures() must be called before using this
//                  method.

void SLTerrain::SetFGColorPixel(const int pixelX, const int pixelY, const uint32_t color)
{
    int posX = pixelX;
    int posY = pixelY;

    WrapPosition(posX, posY);

    // If it's still below or to the sides out of bounds after
    // wrapping what is supposed to be wrapped, shit is out of bounds.
    if (posX < 0 ||
       posX >= m_pMainTexture->getW() ||
       posY < 0 ||
       posY >= m_pMainTexture->getH())
       return;

//    RTEAssert(m_pFGColor->GetTexture()->m_LockCount > 0, "Trying to access unlocked terrain bitmap");
    m_pFGColor->SetPixel(posX, posY, color);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetBGColorPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific pixel on the BG Color bitmap of this SLTerrain to a
//                  specific color. LockTextures() must be called before using this
//                  method.

void SLTerrain::SetBGColorPixel(const int pixelX, const int pixelY, const uint32_t color)
{
    int posX = pixelX;
    int posY = pixelY;

    WrapPosition(posX, posY);

    // If it's still below or to the sides out of bounds after
    // wrapping what is supposed to be wrapped, shit is out of bounds.
    if (posX < 0 ||
       posX >= m_pMainTexture->getW() ||
       posY < 0 ||
       posY >= m_pMainTexture->getH())
       return;

//    RTEAssert(m_pBGColor->GetTexture()->m_LockCount > 0, "Trying to access unlocked terrain bitmap");
    m_pBGColor->SetPixel(posX, posY, color);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMaterialPixel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific pixel on the material bitmap of this SLTerrain to a
//                  specific material. LockMaterialTexture() must be called before using this
//                  method.

void SLTerrain::SetMaterialPixel(const int pixelX, const int pixelY, const uint32_t material)
{
    int posX = pixelX;
    int posY = pixelY;

    WrapPosition(posX, posY);

    // If it's still below or to the sides out of bounds after
    // wrapping what is supposed to be wrapped, shit is out of bounds.
    if (posX < 0 ||
       posX >= m_pMainTexture->getW() ||
       posY < 0 ||
       posY >= m_pMainTexture->getH())
       return;
//    RTEAssert(m_pMainTexture->m_LockCount > 0, "Trying to access unlocked terrain bitmap");
    m_pMainTexture->setPixel(posX, posY, material);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          EraseSilhouette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Takes a BITMAP and scans through the pixels on this terrain for pixels
//                  which overlap with it. Erases them from the terrain and can optionally
//                  generate MOPixel:s based on the erased or 'dislodged' terrain pixels.

deque<MOPixel *> SLTerrain::EraseSilhouette(std::shared_ptr<Texture> pSprite,
                                            Vector pos,
                                            Vector pivot,
                                            Matrix rotation,
                                            float scale,
                                            bool makeMOPs,
                                            int skipMOP,
                                            int maxMOPs)
{
// TODO: OPTIMIZE THIS, IT'S A TIME HOG. MAYBE JSUT STAMP THE OUTLINE AND SAMPLE SOME RANDOM PARTICLES?

    RTEAssert(pSprite.get(), "Null BITMAP passed to SLTerrain::EraseSilhouette");
	RTEAssert(pSprite->getAccess()!=SDL_TEXTUREACCESS_TARGET, "Render Target passed to SLTerrain::EraseSilhouette")

    deque<MOPixel *> MOPDeque;

	// TODO: this might be doable without using rendering instead
	// Create temporary SDL_Surfaces from Sprite and Surface (this is very fast
	// because the pixels are not copied)
	std::unique_ptr<SDL_Surface, sdl_deleter> tempSpriteSurface{
		SDL_CreateRGBSurfaceWithFormatFrom(
		    pSprite->getPixelsRW(), pSprite->getW(), pSprite->getH(), 32,
		    pSprite->getW() * sizeof(uint32_t), pSprite->getFormat())
	};

	std::unique_ptr<SDL_Surface, sdl_deleter> rotozoomedSprite{
		rotozoomSurface(tempSpriteSurface.get(), rotation.GetDegAngle(), scale, false)
	};

	//TODO: All of the below should be possible to do using some clever blitting
	// Find the maximum possible sized bitmap that the passed-in sprite will need

    int skipCount = skipMOP;

    // Do the test of intersection between color pixels of the test bitmap and non-air pixels of the terrain
    // Generate and collect MOPixels that represent the terrain overlap and clear the same pixels out of the terrain
    int testX, testY, terrX, terrY;
    MOPixel *pPixel = 0;
	Material const * sceneMat = g_SceneMan.GetMaterialFromID(g_MaterialAir);
	Material const * spawnMat = g_SceneMan.GetMaterialFromID(g_MaterialAir);
    int halfWidth = rotozoomedSprite->w / 2;
    int halfHeight = rotozoomedSprite->h / 2;
    uint32_t testPixel = 0, matPixel = 0, colorPixel = 0;

	m_pMainTexture->lock();
	m_pFGColor->LockTexture();

    for (testY = 0; testY < rotozoomedSprite->h; ++testY)
    {
        for (testX = 0; testX < rotozoomedSprite->w; ++testX)
        {
            terrX = pos.m_X - halfWidth + testX;
            terrY = pos.m_Y - halfHeight + testY;

            // Make sure we're checking within bounds
            if (terrX < 0) {
                if (m_WrapX)
                    while (terrX < 0)
                        terrX += m_pMainTexture->getW();
                else
                    continue;
            }
            if (terrY < 0) {
                if (m_WrapY)
                    while (terrY < 0)
                        terrY += m_pMainTexture->getH();
                else
                    continue;
            }
            if (terrX >= m_pMainTexture->getW()) {
                if (m_WrapX)
                    terrX %= m_pMainTexture->getW();
                else
                    continue;
            }
            if (terrY >= m_pMainTexture->getH()) {
                if (m_WrapY)
                    terrY %= m_pMainTexture->getH();
                else
                    continue;
            }

            testPixel =  static_cast<uint32_t*>(rotozoomedSprite->pixels)[testX + testY*rotozoomedSprite->w];
            matPixel = m_pMainTexture->getPixel(terrX, terrY);
            colorPixel = m_pFGColor->GetPixel(terrX, terrY);

            if (testPixel != g_MaskColor)
            {
                // Only add PixelMO if we're not due to skip any
                if (makeMOPs && matPixel != g_MaterialAir && colorPixel != g_MaskColor && ++skipCount > skipMOP && MOPDeque.size() < maxMOPs)
                {
                    skipCount = 0;
                    sceneMat = g_SceneMan.GetMaterialFromID(matPixel);
                    spawnMat = sceneMat->GetSpawnMaterial() ? g_SceneMan.GetMaterialFromID(sceneMat->GetSpawnMaterial()) : sceneMat;
                    // Create the MOPixel based off the Terrain data.
                    pPixel = new MOPixel(colorPixel,
                                         spawnMat->GetPixelDensity(),
                                         Vector(terrX, terrY),
                                         Vector(),
                                         new Atom(Vector(), spawnMat->GetIndex(), 0, colorPixel, 2),
                                         0);

                    pPixel->SetToHitMOs(false);
                    MOPDeque.push_back(pPixel);
                    pPixel = 0;
                }

                // Clear the terrain pixels
                if (matPixel != g_MaterialAir)
                    m_pMainTexture->setPixel(terrX, terrY, g_MaterialAir);
				if (colorPixel != g_AlphaZero)
				{
					m_pFGColor->SetPixel(terrX, terrY, g_AlphaZero);
					g_SceneMan.RegisterTerrainChange(terrX, terrY, 1, 1, g_AlphaZero, false);
				}
            }
        }
    }

    // Add a box to the updated areas list to show there's been change to the materials layer
// TODO: improve fit/tightness of box here
    m_UpdatedMateralAreas.push_back(Box(pos - pivot, rotozoomedSprite->w, rotozoomedSprite->h));

    return MOPDeque;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyObject
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a passed in Object's graphical and material representations to
//                  this Terrain's respective layers.

bool SLTerrain::ApplyObject(Entity *pEntity)
{
    if (!pEntity)
        return false;

    // Find out what kind it is
    if (MovableObject *pMO = dynamic_cast<MovableObject *>(pEntity))
    {
        ApplyMovableObject(pMO);
        return true;
    }
    else if (TerrainObject *pTO = dynamic_cast<TerrainObject *>(pEntity))
    {
        ApplyTerrainObject(pTO);
        return true;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyMovableObject
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a passed in MovableObject's graphical and material
//                  representations to this Terrain's respective layers.

void SLTerrain::ApplyMovableObject(MovableObject *pMObject)
{
    if (!pMObject)
        return;

    // Determine whether a sprite or just a pixel-based MO
    MOSprite *pMOSprite = dynamic_cast<MOSprite *>(pMObject);


    // Sprite, so try to integrate it into the terrain, with terrain on top
    if (pMOSprite)
    {

		int spriteDiameter{static_cast<int>(pMObject->GetDiameter())};

		SDL_Renderer *renderer = g_FrameMan.GetRenderer();


		// if neccessary resize the temporary render Target
		if(!s_TempRenderTarget || spriteDiameter>s_TempRenderTarget->getW())
			s_TempRenderTarget = std::make_unique<Texture>(renderer, spriteDiameter, spriteDiameter, SDL_TEXTUREACCESS_TARGET);

        // The position of the upper left corner of the temporary bitmap in the scene
        Vector bitmapScroll = pMObject->GetPos().GetFloored() - Vector(pMObject->GetDiameter() / 2, pMObject->GetDiameter() / 2);

		GetFGColorTexture()->lock(SDL_Rect{static_cast<int>(bitmapScroll.m_X),
			                               static_cast<int>(bitmapScroll.m_Y),
			                               spriteDiameter, spriteDiameter});

		Box notUsed;

		g_FrameMan.PushRenderTarget(s_TempRenderTarget->getAsRenderTarget());

		pMOSprite->Draw(renderer, bitmapScroll, g_DrawColor, true);

		m_pFGColor->Draw(renderer, notUsed, bitmapScroll);

		SDL_RenderReadPixels(renderer, nullptr,
			                 GetFGColorTexture()->getFormat(),
			                 GetFGColorTexture()->getPixelsRW(),
			                 GetFGColorTexture()->getW() * sizeof(uint32_t));

		GetFGColorTexture()->unlock();


		// Register terrain change
		g_SceneMan.RegisterTerrainChange(bitmapScroll.m_X, bitmapScroll.m_Y, spriteDiameter, spriteDiameter, g_MaskColor, false);


// TODO: centralize seam drawing!
        // Draw over seams
        if (g_SceneMan.SceneWrapsX())
        {
            if (bitmapScroll.m_X < 0){
				GetFGColorTexture()->lock(SDL_Rect{static_cast<int>(bitmapScroll.m_X)+g_SceneMan.GetSceneWidth(), static_cast<int>(bitmapScroll.m_Y), spriteDiameter, spriteDiameter});
			} else if (bitmapScroll.m_X + spriteDiameter >
				       g_SceneMan.GetSceneWidth()) {
				GetFGColorTexture()->lock(SDL_Rect{static_cast<int>(bitmapScroll.m_X)-g_SceneMan.GetSceneWidth(), static_cast<int>(bitmapScroll.m_Y), spriteDiameter, spriteDiameter});
			}
			SDL_RenderReadPixels(renderer, nullptr, GetFGColorTexture()->getFormat(), GetFGColorTexture()->getPixelsRW(), GetFGColorTexture()->getW()*sizeof(uint32_t));
			GetFGColorTexture()->unlock();
        }
        if (g_SceneMan.SceneWrapsY())
        {
            if (bitmapScroll.m_Y < 0){
				GetFGColorTexture()->lock(SDL_Rect{static_cast<int>(bitmapScroll.m_X), static_cast<int>(bitmapScroll.m_Y) + g_SceneMan.GetSceneHeight(), spriteDiameter, spriteDiameter});
			}
            else if (bitmapScroll.m_Y + spriteDiameter > g_SceneMan.GetSceneHeight()){
				GetFGColorTexture()->lock(SDL_Rect{static_cast<int>(bitmapScroll.m_X), static_cast<int>(bitmapScroll.m_Y) - g_SceneMan.GetSceneHeight(), spriteDiameter, spriteDiameter});
			}
			SDL_RenderReadPixels(renderer, nullptr,
				                 GetFGColorTexture()->getFormat(),
				                 GetFGColorTexture()->getPixelsRW(),
				                 GetFGColorTexture()->getW() * sizeof(uint32_t));
			GetFGColorTexture()->unlock();
		}

		SDL_PixelFormat *globalFormat{SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32)};
		uint8_t air_r, air_g, air_b, air_a;
		SDL_GetRGBA(g_MaterialAir, globalFormat, &air_r, &air_g, &air_b,
			        &air_a);
		SDL_FreeFormat(globalFormat);

		SDL_SetRenderDrawColor(renderer, air_r, air_g, air_b, air_a);
		SDL_RenderClear(renderer);
		// Draw the actor and then the scene material layer to temp bitmap
        pMOSprite->Draw(renderer, bitmapScroll, g_DrawMaterial, true);
        SceneLayer::Draw(renderer, notUsed, bitmapScroll);

		GetMaterialTexture()->lock(SDL_Rect{static_cast<int>(bitmapScroll.m_X), static_cast<int>(bitmapScroll.m_Y), spriteDiameter, spriteDiameter});

		SDL_RenderReadPixels(renderer, nullptr,
			                 GetMaterialTexture()->getFormat(),
			                 GetMaterialTexture()->getPixelsRW(),
			                 GetMaterialTexture()->getW() * sizeof(uint32_t));

		GetMaterialTexture()->unlock();
        // Add a box to the updated areas list to show there's been change to the materials layer
        m_UpdatedMateralAreas.push_back(Box(bitmapScroll, spriteDiameter, spriteDiameter));
// TODO: centralize seam drawing!
        // Draw over seams
        if (g_SceneMan.SceneWrapsX())
        {
            if (bitmapScroll.m_X < 0){
				GetFGColorTexture()->lock(
					SDL_Rect{static_cast<int>(bitmapScroll.m_X) +
					             g_SceneMan.GetSceneWidth(),
					         static_cast<int>(bitmapScroll.m_Y), spriteDiameter,
					         spriteDiameter});
			} else if (bitmapScroll.m_X + spriteDiameter >
				       g_SceneMan.GetSceneWidth()) {
				GetFGColorTexture()->lock(
					SDL_Rect{static_cast<int>(bitmapScroll.m_X) -
					             g_SceneMan.GetSceneWidth(),
					         static_cast<int>(bitmapScroll.m_Y), spriteDiameter,
					         spriteDiameter});
			}
			SDL_RenderReadPixels(renderer, nullptr,
				                 GetFGColorTexture()->getFormat(),
				                 GetFGColorTexture()->getPixelsRW(),
				                 GetFGColorTexture()->getW() * sizeof(uint32_t));
			GetFGColorTexture()->unlock();
		}
		if (g_SceneMan.SceneWrapsY())
        {
			if (bitmapScroll.m_Y < 0) {
				GetFGColorTexture()->lock(
					SDL_Rect{static_cast<int>(bitmapScroll.m_X),
					         static_cast<int>(bitmapScroll.m_Y) +
					             g_SceneMan.GetSceneHeight(),
					         spriteDiameter, spriteDiameter});
			} else if (bitmapScroll.m_Y + spriteDiameter >
				       g_SceneMan.GetSceneHeight()) {
				GetFGColorTexture()->lock(
					SDL_Rect{static_cast<int>(bitmapScroll.m_X),
					         static_cast<int>(bitmapScroll.m_Y) -
					             g_SceneMan.GetSceneHeight(),
					         spriteDiameter, spriteDiameter});
			}
			SDL_RenderReadPixels(renderer, nullptr,
				                 GetFGColorTexture()->getFormat(),
				                 GetFGColorTexture()->getPixelsRW(),
				                 GetFGColorTexture()->getW() * sizeof(uint32_t));
			GetFGColorTexture()->unlock();
		}

		g_FrameMan.PopRenderTarget();
    }
    // MOPixel, so update single pixel
    else
    {
		MOPixel* pPixel{dynamic_cast<MOPixel *>(pMObject)};
		if (pPixel) {
			Color c{pPixel->GetColor()};
			GetFGColorTexture()->lock();
			GetFGColorTexture()->setPixel(pPixel->GetPos().GetFloorIntX(),
				                          pPixel->GetPos().GetFloorIntY(),
				                          c.GetR(), c.GetG(), c.GetB(), 255);
			GetFGColorTexture()->unlock();
			// Register terrain change
			g_SceneMan.RegisterTerrainChange(pPixel->GetPos().m_X,
				                             pPixel->GetPos().m_Y, 1, 1,
				                             g_DrawColor, false);

			uint32_t mat = pPixel->GetAtom()->GetMaterial()->GetSettleMaterial();
			GetMaterialTexture()->lock();
			GetMaterialTexture()->setPixel(pPixel->GetPos().GetFloorIntX(),pPixel->GetPos().GetFloorIntY(), mat);
			GetMaterialTexture()->unlock();

		}
	}
}



void SLTerrain::RegisterTerrainChange(TerrainObject *pTObject)
{
	if (!pTObject)
		return;

	Vector loc = pTObject->GetPos() + pTObject->GetTextureOffset();

	if (pTObject->HasBGColor())
	{
		g_SceneMan.RegisterTerrainChange(loc.m_X, loc.m_Y, pTObject->GetTextureWidth(), pTObject->GetTextureHeight(), g_MaskColor, true);
	}

	// Register terrain change
	g_SceneMan.RegisterTerrainChange(loc.m_X, loc.m_Y, pTObject->GetTextureWidth(), pTObject->GetTextureHeight(), g_MaskColor, false);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyTerrainObject
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a passed in TerrainObject's graphical and material
//                  representations to this Terrain's respective layers.

void SLTerrain::ApplyTerrainObject(TerrainObject *pTObject)
{
    if (!pTObject)
        return;

    Vector loc = pTObject->GetPos() + pTObject->GetTextureOffset();

	SDL_Rect lockBox{static_cast<int>(loc.m_X), static_cast<int>(loc.m_Y),
		             pTObject->GetTextureWidth(), pTObject->GetTextureHeight()};

	// Do duplicate drawing if the terrain object straddles a wrapping border
    if (loc.m_X < 0)
    {
		SDL_Point offset{m_pMainTexture->getW(),0};
		m_pMainTexture->lock(lockBox+offset);
		SDL_ConvertPixels(
			lockBox.w, lockBox.h, pTObject->GetMaterialTexture()->getFormat(),
			pTObject->GetMaterialTexture()->getPixelsRW(),
			lockBox.w * sizeof(uint32_t), m_pMainTexture->getFormat(),
			m_pMainTexture->getPixelsRW(),
			m_pMainTexture->getW()* sizeof(uint32_t));
		m_pMainTexture->unlock();

		offset.x = m_pFGColor->GetTexture()->getW();
		m_pFGColor->GetTexture()->lock(lockBox+offset);
		SDL_ConvertPixels(
			lockBox.w, lockBox.h, pTObject->GetFGColorTexture()->getFormat(),
			pTObject->GetFGColorTexture()->getPixelsRW(),
			lockBox.w * sizeof(uint32_t), m_pFGColor->GetTexture()->getFormat(),
			m_pFGColor->GetTexture()->getPixelsRW(),
			m_pFGColor->GetTexture()->getW() * sizeof(uint32_t));
		m_pFGColor->GetTexture()->unlock();

		if (pTObject->HasBGColor()){
			offset.x = m_pBGColor->GetTexture()->getW();
			m_pBGColor->GetTexture()->lock(lockBox+offset);

			SDL_ConvertPixels(lockBox.w, lockBox.h,
				              pTObject->GetFGColorTexture()->getFormat(),
				              pTObject->GetFGColorTexture()->getPixelsRW(),
				              lockBox.w * sizeof(uint32_t),
				              m_pBGColor->GetTexture()->getFormat(),
				              m_pBGColor->GetTexture()->getPixelsRW(),
				              m_pBGColor->GetTexture()->getW() *
				                  sizeof(uint32_t));
			m_pBGColor->GetTexture()->unlock();
		}
    }
    else if (loc.m_X >= m_pMainTexture->getW() - pTObject->GetFGColorTexture()->getW())
    {
		SDL_Point offset{-m_pMainTexture->getW(),0};
		m_pMainTexture->lock(lockBox + offset);
		SDL_ConvertPixels(
			lockBox.w, lockBox.h, pTObject->GetMaterialTexture()->getFormat(),
			pTObject->GetMaterialTexture()->getPixelsRW(),
			lockBox.w * sizeof(uint32_t), m_pMainTexture->getFormat(),
			m_pMainTexture->getPixelsRW(),
			m_pMainTexture->getW() * sizeof(uint32_t));
		m_pMainTexture->unlock();

		offset.x = -m_pFGColor->GetTexture()->getW();
		m_pFGColor->GetTexture()->lock(lockBox + offset);
		SDL_ConvertPixels(
			lockBox.w, lockBox.h, pTObject->GetMaterialTexture()->getFormat(),
			pTObject->GetMaterialTexture()->getPixelsRW(),
			lockBox.w * sizeof(uint32_t), m_pFGColor->GetTexture()->getFormat(),
			m_pFGColor->GetTexture()->getPixelsRW(),
			m_pFGColor->GetTexture()->getW() * sizeof(uint32_t));
		m_pFGColor->GetTexture()->unlock();
        if (pTObject->HasBGColor()){
			offset.x = -m_pBGColor->GetTexture()->getW();
			m_pBGColor->GetTexture()->lock(lockBox + offset);
			SDL_ConvertPixels(lockBox.w, lockBox.h,
				              pTObject->GetMaterialTexture()->getFormat(),
				              pTObject->GetMaterialTexture()->getPixelsRW(),
				              lockBox.w * sizeof(uint32_t),
				              m_pBGColor->GetTexture()->getFormat(),
				              m_pBGColor->GetTexture()->getPixelsRW(),
				              m_pBGColor->GetTexture()->getW() *
				                  sizeof(uint32_t));
			m_pBGColor->GetTexture()->unlock();
		}
    }

    // Regular drawing
	m_pMainTexture->lock(lockBox);
	SDL_ConvertPixels(lockBox.w, lockBox.h,
		              pTObject->GetMaterialTexture()->getFormat(),
		              pTObject->GetMaterialTexture()->getPixelsRW(),
		              lockBox.w * sizeof(uint32_t), m_pMainTexture->getFormat(),
		              m_pMainTexture->getPixelsRW(),
		              m_pMainTexture->getW() * sizeof(uint32_t));
	m_pMainTexture->unlock();

	m_pFGColor->GetTexture()->lock(lockBox);
	SDL_ConvertPixels(
		lockBox.w, lockBox.h, pTObject->GetMaterialTexture()->getFormat(),
		pTObject->GetMaterialTexture()->getPixelsRW(),
		lockBox.w * sizeof(uint32_t), m_pFGColor->GetTexture()->getFormat(),
		m_pFGColor->GetTexture()->getPixelsRW(),
		m_pFGColor->GetTexture()->getW() * sizeof(uint32_t));
	m_pFGColor->GetTexture()->unlock();

	if (pTObject->HasBGColor())
	{

		m_pBGColor->GetTexture()->lock(lockBox);
		SDL_ConvertPixels(
			lockBox.w, lockBox.h, pTObject->GetMaterialTexture()->getFormat(),
			pTObject->GetMaterialTexture()->getPixelsRW(),
			lockBox.w * sizeof(uint32_t), m_pBGColor->GetTexture()->getFormat(),
			m_pBGColor->GetTexture()->getPixelsRW(),
			m_pBGColor->GetTexture()->getW() * sizeof(uint32_t));
		m_pBGColor->GetTexture()->unlock();

		g_SceneMan.RegisterTerrainChange(loc.m_X, loc.m_Y, pTObject->GetTextureWidth(), pTObject->GetTextureHeight(), g_MaskColor, true);
	}

	// Register terrain change
	g_SceneMan.RegisterTerrainChange(loc.m_X, loc.m_Y, pTObject->GetTextureWidth(), pTObject->GetTextureHeight(), g_MaskColor, false);

    // Add a box to the updated areas list to show there's been change to the materials layer
    m_UpdatedMateralAreas.push_back(Box(loc, pTObject->GetMaterialTexture()->getW(), pTObject->GetMaterialTexture()->getH()));

    // Apply all the child objects of the TO, and first reapply the team so all its children are guaranteed to be on the same team!
    pTObject->SetTeam(pTObject->GetTeam());
    for (list<SceneObject::SOPlacer>::const_iterator itr = pTObject->GetChildObjects().begin(); itr != pTObject->GetChildObjects().end() ; ++itr)
    {
// TODO: check if we're placing a brain, and have it replace the resident brain of the scene!
        // Copy and apply, transferring ownership of the new copy into the application
        g_SceneMan.AddSceneObject((*itr).GetPlacedCopy(pTObject));
    }

//    CleanAir();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsBoxBuried
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks whether a bounding box is completely buried in the terrain.

bool SLTerrain::IsBoxBuried(const Box &checkBox) const
{
    bool buried = true;

    // Center
// Not important for buriedness, only corners are, really
//    buried = buried && GetMaterialPixel(checkBox.GetCenter().m_X, checkBox.GetCenter().m_Y) != g_MaterialAir;

    // Corners
    buried = buried && !IsAirPixel(checkBox.GetCorner().m_X, checkBox.GetCorner().m_Y);
    buried = buried && !IsAirPixel(checkBox.GetCorner().m_X + checkBox.GetWidth(), checkBox.GetCorner().m_Y);
    buried = buried && !IsAirPixel(checkBox.GetCorner().m_X, checkBox.GetCorner().m_Y + checkBox.GetHeight());
    buried = buried && !IsAirPixel(checkBox.GetCorner().m_X + checkBox.GetWidth(), checkBox.GetCorner().m_Y + checkBox.GetHeight());

    return buried;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CleanAirBox
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes any color pixel in the color layer of this SLTerrain wherever
//                  there is an air material pixel in the material layer. Whether the scene is X-wrapped or Y-wrapped.

void SLTerrain::CleanAirBox(Box box, bool wrapsX, bool wrapsY)
{
	m_pMainTexture->lock();
	m_pFGColor->GetTexture()->lock();

    int width = m_pMainTexture->getW();
    int height = m_pMainTexture->getH();
    unsigned char matPixel;

	for (int y = box.m_Corner.m_Y; y < box.m_Corner.m_Y + box.m_Height; ++y) {
		for (int x = box.m_Corner.m_X; x < box.m_Corner.m_X + box.m_Width; ++x) {
			float wrapX = x;
			float wrapY = y;

			//Fix coords in case of seam
			if (wrapsX)
			{
				if (wrapX < 0)
					wrapX = wrapX + m_pMainTexture->getW();

				if (wrapX >= m_pMainTexture->getW())
					wrapX = wrapX - m_pMainTexture->getW();
			}

			if (wrapsY)
			{
				if (wrapY < 0)
					wrapY = wrapY + m_pMainTexture->getH();

				if (wrapY >= m_pMainTexture->getH())
					wrapY = wrapY - m_pMainTexture->getH();
			}

			if (wrapX >= 0 && wrapY >=0 && wrapX < width && wrapY < height)
			{
				matPixel = m_pMainTexture->getPixel(wrapX, wrapY);
				if (matPixel == g_MaterialCavity) {
					m_pMainTexture->setPixel(wrapX, wrapY, g_MaterialAir);
					matPixel = g_MaterialAir;
				}
				if (matPixel == g_MaterialAir)
					m_pFGColor->GetTexture()->setPixel(wrapX, wrapY, g_MaskColor);
			}

        }
    }

	m_pMainTexture->unlock();
    m_pFGColor->GetTexture()->unlock();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CleanAir
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes any color pixel in the color layer of this SLTerrain wherever
//                  there is an air material pixel in the material layer.

void SLTerrain::CleanAir()
{
	m_pMainTexture->lock();
	m_pFGColor->GetTexture()->lock();

    int width = m_pMainTexture->getW();
    int height = m_pMainTexture->getH();
    unsigned char matPixel;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            matPixel = m_pMainTexture->getPixel(x, y);
            if (matPixel == g_MaterialCavity) {
                m_pMainTexture->setPixel(x, y, g_MaterialAir);
                matPixel = g_MaterialAir;
            }
            if (matPixel == g_MaterialAir)
                m_pFGColor->GetTexture()->setPixel(x, y, g_MaskColor);
        }
    }

	m_pMainTexture->unlock();
	m_pFGColor->GetTexture()->unlock();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearAllMaterial
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes any FG and material pixels completely form teh terrain.
//                  For the editor mostly.

void SLTerrain::ClearAllMaterial()
{
	m_pMainTexture->lock();
	m_pMainTexture->clearAll();
	m_pMainTexture->unlock();

	m_pFGColor->GetTexture()->lock();
	m_pFGColor->GetTexture()->clearAll(g_MaterialAir);
	m_pFGColor->GetTexture()->unlock();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this SLTerrain. Supposed to be done every frame.

void SLTerrain::Update()
{
    SceneLayer::Update();

    m_pFGColor->SetOffset(m_Offset);
    m_pBGColor->SetOffset(m_Offset);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  DrawBackground
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this SLTerrain's current scrolled position to a bitmap.

void SLTerrain::DrawBackground(SDL_Renderer *renderer, Box &targetBox, const Vector &scrollOverride)
{
    m_pBGColor->Draw(renderer, targetBox, scrollOverride);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this SLTerrain's current scrolled position to a bitmap.

void SLTerrain::Draw(SDL_Renderer *renderer, Box &targetBox, const Vector &scrollOverride) const
{
    if (m_DrawMaterial)
    {
        SceneLayer::Draw(renderer, targetBox, scrollOverride);
    }
    else
    {
        m_pFGColor->Draw(renderer, targetBox, scrollOverride);
    }
}

} // namespace RTE
