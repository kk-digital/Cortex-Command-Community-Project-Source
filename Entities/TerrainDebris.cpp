#include "TerrainDebris.h"
#include "SLTerrain.h"

#include "RTERenderer.h"
#include "SDL2/SDL_surface.h"

namespace RTE {

	ConcreteClassInfo(TerrainDebris, Entity, 0);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TerrainDebris::Clear() {
		m_DebrisFile.Reset();
		m_Textures.clear();
		m_BitmapCount = 0;
		m_Material.Reset();
		m_TargetMaterial.Reset();
		m_OnlyOnSurface = false;
		m_OnlyBuried = false;
		m_MinDepth = 0;
		m_MaxDepth = 10;
		m_Density = 0.01F;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int TerrainDebris::Create() {
		if (Entity::Create() < 0) {
			return -1;
		}
		m_Textures = m_DebrisFile.GetAsAnimation(m_BitmapCount);
		RTEAssert(!m_Textures.empty() && m_Textures[0], "Failed to load debris bitmaps!");

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int TerrainDebris::Create(const TerrainDebris &reference) {
		Entity::Create(reference);

		m_DebrisFile = reference.m_DebrisFile;
		m_Textures = reference.m_Textures;
		m_BitmapCount = reference.m_BitmapCount;
		m_Material = reference.m_Material;
		m_TargetMaterial = reference.m_TargetMaterial;
		m_OnlyOnSurface = reference.m_OnlyOnSurface;
		m_OnlyBuried = reference.m_OnlyBuried;
		m_MinDepth = reference.m_MinDepth;
		m_MaxDepth = reference.m_MaxDepth;
		m_Density = reference.m_Density;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int TerrainDebris::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "DebrisFile") {
			reader >> m_DebrisFile;
		} else if (propName == "DebrisPieceCount") {
			reader >> m_BitmapCount;
			m_Textures.reserve(m_BitmapCount);
		} else if (propName == "DebrisMaterial") {
			reader >> m_Material;
		} else if (propName == "TargetMaterial") {
			reader >> m_TargetMaterial;
		} else if (propName == "OnlyOnSurface") {
			reader >> m_OnlyOnSurface;
		} else if (propName == "OnlyBuried") {
			reader >> m_OnlyBuried;
		} else if (propName == "MinDepth") {
			reader >> m_MinDepth;
		} else if (propName == "MaxDepth") {
			reader >> m_MaxDepth;
		} else if (propName == "DensityPerMeter") {
			reader >> m_Density;
		} else {
			return Entity::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int TerrainDebris::Save(Writer &writer) const {
		Entity::Save(writer);

		writer.NewProperty("DebrisFile");
		writer << m_DebrisFile;
		writer.NewProperty("DebrisPieceCount");
		writer << m_BitmapCount;
		writer.NewProperty("DebrisMaterial");
		writer << m_Material;
		writer.NewProperty("TargetMaterial");
		writer << m_TargetMaterial;
		writer.NewProperty("OnlyOnSurface");
		writer << m_OnlyOnSurface;
		writer.NewProperty("OnlyBuried");
		writer << m_OnlyBuried;
		writer.NewProperty("MinDepth");
		writer << m_MinDepth;
		writer.NewProperty("MaxDepth");
		writer << m_MaxDepth;
		writer.NewProperty("DensityPerMeter");
		writer << m_Density;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TerrainDebris::ApplyDebris(SLTerrain *terrain) {
		RTEAssert(!m_Textures.empty() && m_BitmapCount > 0, "No bitmaps loaded for terrain debris!");

		std::shared_ptr<GLTexture> terrTexture = terrain->GetFGColorTexture()->GetTexture();
		std::shared_ptr<GLTexture> matTexture = terrain->GetMaterialTexture()->GetTexture();


		// How many pieces of debris we're spreading out.
		unsigned int terrainWidth = terrTexture->GetW();
		unsigned int pieceCount = (terrainWidth * c_MPP) * m_Density;

		// First is index in the bitmap array, Second is blit location
		std::list<std::pair<int, Vector>> piecesToPlace;
		Vector location;
		Box pieceBox;

		uint32_t checkPixel;

		for (unsigned int piece = 0; piece < pieceCount; ++piece) {
			bool place = false;
			unsigned int currentBitmap = RandomNum<int>(0, m_BitmapCount - 1);
			RTEAssert(currentBitmap >= 0 && currentBitmap < m_BitmapCount, "Bitmap index is out of bounds!");

			pieceBox.SetWidth(m_Textures[currentBitmap]->GetW());
			pieceBox.SetHeight(m_Textures[currentBitmap]->GetH());

			int x = RandomNum<int>(0, terrainWidth);
			int y = 0;
			int depth = RandomNum(m_MinDepth, m_MaxDepth);

			while (y < terrTexture->GetH()) {
				// Find the air-terrain boundary
				for (; y < terrTexture->GetH(); ++y) {
					checkPixel = matTexture->GetPixel(x, y);
					// Check for terrain hit
					if (checkPixel != g_MaterialAir) {
						if (checkPixel == m_TargetMaterial.GetIndex()) {
							place = true;
							break;
						// If we didn't hit target material, but are specified to, then don't place
						} else if (m_OnlyOnSurface) {
							place = false;
							break;
						}
					}
				}
				if (!place) {
					break;
				}
				// The target locations are on the center of the objects; if supposed to be buried, move down so it is
				y += depth + static_cast<int>(m_OnlyBuried ? pieceBox.GetHeight() * 0.6F : 0);
				pieceBox.SetCenter(Vector(static_cast<float>(x), static_cast<float>(y)));

				// Make sure we're not trying to place something into a cave or other air pocket
				if (!g_SceneMan.GetTerrain()->IsAirPixel(x, y) && (!m_OnlyBuried || g_SceneMan.GetTerrain()->IsBoxBuried(pieceBox))) {
					// Do delayed drawing so that we don't end up placing things on top of each other
					piecesToPlace.emplace_back(currentBitmap, pieceBox.GetCorner());
					break;
				}
			}
		}

		for (const std::pair<int, Vector> &pieceListEntry : piecesToPlace) {
			// Draw the color sprite onto the terrain color layer.
			m_Textures[pieceListEntry.first]->blit(terrain->GetFGColorTexture()->GetTexture(), pieceListEntry.second);

			m_Textures[pieceListEntry.first]->blitMasked(terrain->GetMaterialTexture()->GetTexture(), m_Material.GetIndex(), pieceListEntry.second.m_X, pieceListEntry.second.m_Y);
		}
	}
}
