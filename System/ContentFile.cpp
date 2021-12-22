#include "ContentFile.h"
#include "AudioMan.h"
#include "PresetMan.h"
#include "ConsoleMan.h"
#include "FrameMan.h"

#include "System/System.h"

#include "SDLHelper.h"
#include <SDL2/SDL_image.h>
#include "Renderer/GLTexture.h"
#include "Renderer/GLPalette.h"
#include "Renderer/GLCheck.h"
#include "GL/glew.h"

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

namespace RTE {

	const std::string ContentFile::c_ClassName = "ContentFile";

	std::unordered_map<std::string, std::shared_ptr<GLTexture>> ContentFile::s_LoadedTextures;
	std::unordered_map<std::string, FMOD::Sound *> ContentFile::s_LoadedSamples;
	std::unordered_map<size_t, std::string> ContentFile::s_PathHashes;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::Clear() {
		m_DataPath.clear();
		m_DataPathExtension.clear();
		m_DataPathWithoutExtension.clear();
		m_FormattedReaderPosition.clear();
		m_DataPathAndReaderPosition.clear();
		m_DataModuleID = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Create(const char *filePath) {
		SetDataPath(filePath);
		SetFormattedReaderPosition(GetFormattedReaderPosition());

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Create(const ContentFile &reference) {
		m_DataPath = reference.m_DataPath;
		m_DataPathExtension = reference.m_DataPathExtension;
		m_DataPathWithoutExtension = reference.m_DataPathWithoutExtension;
		m_DataModuleID = reference.m_DataModuleID;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "FilePath" || propName == "Path") {
			SetDataPath(reader.ReadPropValue());
		} else {
			return Serializable::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::Save(Writer &writer) const {
		Serializable::Save(writer);

		if (!m_DataPath.empty()) {
			writer.NewPropertyWithValue("FilePath", m_DataPath);
		}

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int ContentFile::GetDataModuleID() const { return (m_DataModuleID < 0) ? g_PresetMan.GetModuleIDFromPath(m_DataPath) : m_DataModuleID; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::SetDataPath(const std::string &newDataPath) {
		m_DataPath = CorrectBackslashesInPath(newDataPath);
		m_DataPathExtension = std::filesystem::path(m_DataPath).extension().string();

		RTEAssert(!m_DataPathExtension.empty(), "Failed to find file extension when trying to find file with path and name:\n" + m_DataPath + "\n" + GetFormattedReaderPosition());

		m_DataPathWithoutExtension = m_DataPath.substr(0, m_DataPath.length() - m_DataPathExtension.length());
		s_PathHashes[GetHash()] = m_DataPath;
		m_DataModuleID = g_PresetMan.GetModuleIDFromPath(m_DataPath);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ContentFile::SetFormattedReaderPosition(const std::string &newPosition) {
		m_FormattedReaderPosition = newPosition;
		m_DataPathAndReaderPosition = m_DataPath + "\n" + newPosition;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<GLTexture> ContentFile::GetAsTexture(ColorConvert conversionMode, bool storeTexture, const std::string &dataPathToSpecificFrame) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		std::string dataPathToLoad = dataPathToSpecificFrame.empty() ? m_DataPath : dataPathToSpecificFrame;
		SetFormattedReaderPosition(GetFormattedReaderPosition());
		std::shared_ptr<GLTexture> returnTexture;

		// Check if the file has already been read and loaded from the disk and,
		// if so, use that data.
		auto it_foundTexture = s_LoadedTextures.find(dataPathToLoad);
		if (it_foundTexture != s_LoadedTextures.end()) {
			returnTexture = (*it_foundTexture).second;
		} else {
			if (!System::PathExistsCaseSensitive(dataPathToLoad)) {
				const std::string dataPathWithoutExtension = dataPathToLoad.substr(0, dataPathToLoad.length() - m_DataPathExtension.length());
				const std::string altFileExtension = (m_DataPathExtension == ".png") ? ".bmp" : ".png";

				if (System::PathExistsCaseSensitive(dataPathWithoutExtension + altFileExtension)) {
					g_ConsoleMan.AddLoadWarningLogEntry(m_DataPath, m_FormattedReaderPosition, altFileExtension);
					SetDataPath(m_DataPathWithoutExtension + altFileExtension);
					dataPathToLoad =
					    dataPathWithoutExtension + altFileExtension;
				} else {
					RTEAbort("Failed to find image file with following path and name:\n\n" + dataPathToLoad + " or " + altFileExtension + "\n" + m_FormattedReaderPosition);
				}
			}
			// NOTE: This takes ownership of the texture file
			returnTexture = LoadAndReleaseImage(dataPathToLoad, conversionMode);

			// Insert the texture into the map, PASSING OVER OWNERSHIP OF THE
			// LOADED DATAFILE
			if (storeTexture) {
				s_LoadedTextures.insert({dataPathToLoad, returnTexture});
			}
		}

		return returnTexture;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<Palette> ContentFile::GetAsPalette() {
		std::unique_ptr<SDL_Surface, sdl_deleter> paletteLoader{IMG_Load(m_DataPath.c_str())};
		RTEAssert(paletteLoader.get(), "Failed to load palette from: " + m_DataPath + " because: " + IMG_GetError());
		if (!paletteLoader->format->palette && paletteLoader->w * paletteLoader->h != Palette::PALETTESIZE) {
			RTEAbort("Attempted to load incorrectly sized palette from: " + m_DataPath);
		}
		std::array<glm::u8vec4, Palette::PALETTESIZE> colors;
		if (paletteLoader->format->palette) {
			for (int i = 0; i < paletteLoader->format->palette->ncolors && i < Palette::PALETTESIZE; ++i) {
				SDL_Color color = paletteLoader->format->palette->colors[i];
				colors[i] = {color.r, color.g, color.b, color.a};
			}
		} else {
			if (paletteLoader->format->BitsPerPixel < 32) {
				// Convert to known format
				SDL_Surface *knownFormat = SDL_ConvertSurfaceFormat(paletteLoader.get(), SDL_PIXELFORMAT_RGBA32, 0);
				paletteLoader.reset(knownFormat);
			}
			for (int i = 0; i < paletteLoader->w * paletteLoader->h; ++i) {
				SDL_GetRGBA(static_cast<uint32_t *>(paletteLoader->pixels)[i], paletteLoader->format, &colors[i].r, &colors[i].g, &colors[i].b, &colors[i].a);
			}
		}
		return std::make_shared<Palette>(colors);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<std::shared_ptr<GLTexture>> ContentFile::GetAsAnimation(int frameCount, ColorConvert conversionMode) {
		if (m_DataPath.empty()) {
			std::vector<std::shared_ptr<GLTexture>> empty;
			return empty;
		}
		// Create the array of as many BITMAP pointers as requested frames
		std::vector<std::shared_ptr<GLTexture>> returnTextures;
		SetFormattedReaderPosition(GetFormattedReaderPosition());

		if (frameCount == 1) {
			// Check for 000 in the file name in case it is part of an animation but the FrameCount was set to 1. Do not warn about this because it's normal operation, but warn about incorrect extension.
			if (!System::PathExistsCaseSensitive(m_DataPath)) {
				const std::string altFileExtension = (m_DataPathExtension == ".png") ? ".bmp" : ".png";

				if (System::PathExistsCaseSensitive(m_DataPathWithoutExtension + "000" + m_DataPathExtension)) {
					SetDataPath(m_DataPathWithoutExtension + "000" + m_DataPathExtension);
				} else if (System::PathExistsCaseSensitive(m_DataPathWithoutExtension + "000" + altFileExtension)) {
					g_ConsoleMan.AddLoadWarningLogEntry(m_DataPath, m_FormattedReaderPosition, altFileExtension);
					SetDataPath(m_DataPathWithoutExtension + "000" + altFileExtension);
				}
			}
			returnTextures.push_back(GetAsTexture(conversionMode));
			return returnTextures;
		}
		char framePath[1024];
		for (int frameNum = 0; frameNum < frameCount; frameNum++) {
			std::snprintf(framePath, sizeof(framePath), "%s%03i%s", m_DataPathWithoutExtension.c_str(), frameNum, m_DataPathExtension.c_str());
			returnTextures.push_back(GetAsTexture(conversionMode, true, framePath));
		}
		return returnTextures;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SharedTexture ContentFile::LoadAndReleaseImage(const std::string &dataPathToSpecificFrame, ColorConvert colorConversion) {

		const std::string dataPathToLoad = dataPathToSpecificFrame.empty()
		                                       ? m_DataPath
		                                       : dataPathToSpecificFrame;
		SetFormattedReaderPosition(GetFormattedReaderPosition());

		std::unique_ptr<SDL_Surface, sdl_surface_deleter> tempSurfacePreKey{IMG_Load(dataPathToLoad.c_str())};

		RTEAssert(tempSurfacePreKey.get(), "Failed to load image file with following path and name:\n\n" + m_DataPathAndReaderPosition + "\nThe file may be corrupt, incorrectly converted or saved with unsupported parameters.\n" + SDL_GetError());

		// Set the colorkey of tempSurface for transparency
		Uint32 colorKey{SDL_MapRGB(tempSurfacePreKey->format, 255, 0, 255)};
		SDL_SetColorKey(tempSurfacePreKey.get(), SDL_TRUE, colorKey);

		SharedTexture returnTexture = MakeTexture();

		returnTexture->m_BlendMode = BlendModes::Blend;
		if (colorConversion == ColorConvert::ARGB32 || (colorConversion == ColorConvert::Preserve && !tempSurfacePreKey->format->palette)) {
			SDL_Surface *actualSurface = SDL_ConvertSurfaceFormat(tempSurfacePreKey.get(), SDL_PIXELFORMAT_ARGB8888, 0);
			returnTexture->m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(actualSurface);
			returnTexture->m_BPP = 32;
			returnTexture->m_ShaderBase = g_FrameMan.GetTextureShader(BitDepth::BPP32);
			returnTexture->m_ShaderFill = g_FrameMan.GetTextureShaderFill(BitDepth::BPP32);
		} else {
			if (!tempSurfacePreKey->format->palette) {
				SDL_PixelFormat *globalFmt = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
				SDL_PixelFormat pf = *globalFmt;
				SDL_SetPixelFormatPalette(&pf, g_FrameMan.GetDefaultPalette()->GetAsPalette());
				SDL_Surface *actualSurface = SDL_ConvertSurface(tempSurfacePreKey.get(), &pf, 0);
				returnTexture->m_Pixels = std::unique_ptr<SDL_Surface, sdl_surface_deleter>(actualSurface);
				SDL_FreeFormat(globalFmt);
			} else {
				SDL_SetSurfacePalette(tempSurfacePreKey.get(), g_FrameMan.GetDefaultPalette()->GetAsPalette());
				returnTexture->m_Pixels = std::move(tempSurfacePreKey);
			}
			returnTexture->m_BPP = 8;
			returnTexture->m_Palette = g_FrameMan.GetDefaultPalette();
			returnTexture->m_ShaderBase = g_FrameMan.GetTextureShader(BitDepth::Indexed8);
			returnTexture->m_ShaderFill = g_FrameMan.GetTextureShaderFill(BitDepth::Indexed8);
		}

		returnTexture->m_Width = returnTexture->m_Pixels->w;
		returnTexture->m_Height = returnTexture->m_Pixels->h;
		glCheck(glBindTexture(GL_TEXTURE_2D, returnTexture->m_TextureID));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
		glCheck(glTexImage2D(GL_TEXTURE_2D, 0, returnTexture->m_BPP == 8 ? GL_R8 : GL_RGBA, returnTexture->m_Width, returnTexture->m_Height, 0, returnTexture->m_BPP == 8 ? GL_RED : GL_BGRA, GL_UNSIGNED_BYTE, returnTexture->m_Pixels->pixels));
		glCheck(glGenerateMipmap(GL_TEXTURE_2D));

		glCheck(glBindTexture(GL_TEXTURE_2D, 0));
		return returnTexture;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FMOD::Sound *ContentFile::GetAsSound(bool abortGameForInvalidSound, bool asyncLoading) {
		if (m_DataPath.empty() || !g_AudioMan.IsAudioEnabled()) {
			return nullptr;
		}
		FMOD::Sound *returnSample = nullptr;

		std::unordered_map<std::string, FMOD::Sound *>::iterator foundSound = s_LoadedSamples.find(m_DataPath);
		if (foundSound != s_LoadedSamples.end()) {
			returnSample = (*foundSound).second;
		} else {
			returnSample = LoadAndReleaseSound(abortGameForInvalidSound, asyncLoading); //NOTE: This takes ownership of the sample file

			// Insert the Sound object into the map, PASSING OVER OWNERSHIP OF THE LOADED FILE
			s_LoadedSamples.try_emplace(m_DataPath, returnSample);
		}
		return returnSample;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FMOD::Sound *ContentFile::LoadAndReleaseSound(bool abortGameForInvalidSound, bool asyncLoading) {
		if (m_DataPath.empty() || !g_AudioMan.IsAudioEnabled()) {
			return nullptr;
		}

		if (!System::PathExistsCaseSensitive(m_DataPath)) {
			bool foundAltExtension = false;
			for (const std::string &altFileExtension: c_SupportedAudioFormats) {
				if (System::PathExistsCaseSensitive(m_DataPathWithoutExtension + altFileExtension)) {
					g_ConsoleMan.AddLoadWarningLogEntry(m_DataPath, m_FormattedReaderPosition, altFileExtension);
					SetDataPath(m_DataPathWithoutExtension + altFileExtension);
					foundAltExtension = true;
					break;
				}
			}
			if (!foundAltExtension) {
				std::string errorMessage = "Failed to find audio file with following path and name:\n\n" + m_DataPath + " or any alternative supported file type";
				RTEAssert(!abortGameForInvalidSound, errorMessage + "\n" + m_FormattedReaderPosition);
				g_ConsoleMan.PrintString(errorMessage + ". The file was not loaded!");
				return nullptr;
			}
		}
		if (std::filesystem::file_size(m_DataPath) == 0) {
			const std::string errorMessage = "Failed to create sound because the file was empty. The path and name were: ";
			RTEAssert(!abortGameForInvalidSound, errorMessage + "\n\n" + m_DataPathAndReaderPosition);
			g_ConsoleMan.PrintString("ERROR: " + errorMessage + m_DataPath);
			return nullptr;
		}
		FMOD::Sound *returnSample = nullptr;

		FMOD_MODE fmodFlags = FMOD_CREATESAMPLE | FMOD_3D | (asyncLoading ? FMOD_NONBLOCKING : FMOD_DEFAULT);
		FMOD_RESULT result = g_AudioMan.GetAudioSystem()->createSound(m_DataPath.c_str(), fmodFlags, nullptr, &returnSample);

		if (result != FMOD_OK) {
			const std::string errorMessage = "Failed to create sound because of FMOD error:\n" + std::string(FMOD_ErrorString(result)) + "\nThe path and name were: ";
			RTEAssert(!abortGameForInvalidSound, errorMessage + "\n\n" + m_DataPathAndReaderPosition);
			g_ConsoleMan.PrintString("ERROR: " + errorMessage + m_DataPath);
			return returnSample;
		}
		return returnSample;
	}
} // namespace RTE
