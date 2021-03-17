#include "ContentFile.h"
#include "AudioMan.h"
#include "PresetMan.h"
#include "ConsoleMan.h"

#include "FrameMan.h"
#include "SDLHelper.h"
#include <SDL2/SDL_image.h>

namespace RTE {

	const std::string ContentFile::c_ClassName = "ContentFile";

	std::unordered_map<std::string, std::shared_ptr<Texture>> ContentFile::s_LoadedTextures;
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

		if (!m_DataPath.empty()) { writer.NewPropertyWithValue("FilePath", m_DataPath); }

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

	std::shared_ptr<Texture> ContentFile::GetAsTexture(bool storeTexture,
	                               const std::string &dataPathToSpecificFrame,
								   bool streamingAccess) {
		if (m_DataPath.empty()) {
			return nullptr;
		}
		std::string dataPathToLoad = dataPathToSpecificFrame.empty()
		                                 ? m_DataPath
		                                 : dataPathToSpecificFrame;
		SetFormattedReaderPosition(GetFormattedReaderPosition());
		std::shared_ptr<Texture> returnTexture;

		// Check if the file has already been read and loaded from the disk and,
		// if so, use that data.
		auto it_foundTexture = s_LoadedTextures.find(dataPathToLoad);
		if (it_foundTexture != s_LoadedTextures.end()) {
			returnTexture = (*it_foundTexture).second;
		} else {
			if (!std::filesystem::exists(dataPathToLoad)) {
				const std::string dataPathWithoutExtension =
				    dataPathToLoad.substr(0, dataPathToLoad.length() -
				                                 m_DataPathExtension.length());
				const std::string altFileExtension =
				    (m_DataPathExtension == ".png") ? ".bmp" : ".png";

				if (std::filesystem::exists(dataPathWithoutExtension +
				                            altFileExtension)) {
					g_ConsoleMan.AddLoadWarningLogEntry(
					    m_DataPath, m_FormattedReaderPosition,
					    altFileExtension);
					SetDataPath(m_DataPathWithoutExtension + altFileExtension);
					dataPathToLoad =
					    dataPathWithoutExtension + altFileExtension;
				} else {
					RTEAbort(
					    "Failed to find image file with following path and name:\n\n" +
					    m_DataPath + " or " + altFileExtension + "\n" +
					    m_FormattedReaderPosition);
				}
			}
			// NOTE: This takes ownership of the texture file
			returnTexture = std::make_shared<Texture>(
			    LoadAndReleaseImage(dataPathToLoad, streamingAccess));

			// Insert the texture into the map, PASSING OVER OWNERSHIP OF THE
			// LOADED DATAFILE
			if (storeTexture) {
				s_LoadedTextures.insert(
				    {dataPathToLoad, returnTexture});
			}
		}
	return returnTexture;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<std::shared_ptr<Texture>>  ContentFile::GetAsAnimation(int frameCount) {
		if (m_DataPath.empty()) {
			std::vector<std::shared_ptr<Texture>> empty;
			return empty;
		}
		// Create the array of as many BITMAP pointers as requested frames
		std::vector<std::shared_ptr<Texture>> returnTextures;
		SetFormattedReaderPosition(GetFormattedReaderPosition());

		// Don't try to append numbers if there's only one frame
		if (frameCount == 1) {
			// Check for 000 in the file name in case it is part of an animation but the FrameCount was set to 1. Do not warn about this because it's normal operation, but warn about incorrect extension.
			if (!std::filesystem::exists(m_DataPath)) {
				const std::string altFileExtension = (m_DataPathExtension == ".png") ? ".bmp" : ".png";

				if (std::filesystem::exists(m_DataPathWithoutExtension + "000" + m_DataPathExtension)) {
					SetDataPath(m_DataPathWithoutExtension + "000" + m_DataPathExtension);
				} else if (std::filesystem::exists(m_DataPathWithoutExtension + "000" + altFileExtension)) {
					g_ConsoleMan.AddLoadWarningLogEntry(m_DataPath, m_FormattedReaderPosition, altFileExtension);
					SetDataPath(m_DataPathWithoutExtension + "000" + altFileExtension);
				}
			}
			returnTextures.push_back(GetAsTexture());
			return returnTextures;
		}
		char framePath[1024];
		for (int frameNum = 0; frameNum < frameCount; frameNum++) {
			std::snprintf(framePath, sizeof(framePath), "%s%03i%s", m_DataPathWithoutExtension.c_str(), frameNum, m_DataPathExtension.c_str());
			returnTextures.push_back(GetAsTexture(true, framePath));
		}
		return returnTextures;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Texture
	ContentFile::LoadAndReleaseImage(const std::string &dataPathToSpecificFrame,
	                                 bool streamingAccess) {

		const std::string dataPathToLoad = dataPathToSpecificFrame.empty()
		                                       ? m_DataPath
		                                       : dataPathToSpecificFrame;
		SetFormattedReaderPosition(GetFormattedReaderPosition());

		std::unique_ptr<SDL_Surface, sdl_deleter> tempSurface{
		    IMG_Load(dataPathToLoad.c_str())};

		RTEAssert(
		    tempSurface.get(),
		    "Failed to load image file with following path and name:\n\n" +
		        m_DataPathAndReaderPosition +
		        "\nThe file may be corrupt, incorrectly converted or saved with unsupported parameters.\n" +
		        IMG_GetError());

		// Set the colorkey of tempSurface for transparency
		Uint32 colorKey{SDL_MapRGB(tempSurface->format, 255, 0, 255)};
		SDL_SetColorKey(tempSurface.get(), SDL_TRUE, colorKey);

		// Copy the pixels from the surface for accessing pixel colors
		Texture returnTexture;
		if (!streamingAccess) {
			// Create a Texture from the loaded Image with STATIC ACCESS (!)
			// that lives in vram
			returnTexture.m_Texture.reset(SDL_CreateTextureFromSurface(
			    g_FrameMan.GetRenderer(), tempSurface.get()));
		} else {
			// Create a Texture with STREAMING ACCESS that lives in vram
			returnTexture.m_Texture.reset(SDL_CreateTexture(
			    g_FrameMan.GetRenderer(),
			    returnTexture.getNativeAlphaFormat(g_FrameMan.GetRenderer()),
			    SDL_TEXTUREACCESS_STREAMING, tempSurface->w, tempSurface->h));
		}

		RTEAssert(returnTexture.m_Texture.get(),
		          "Failed to create Texture from " +
		              m_DataPathAndReaderPosition +
		              "\n SDL Error: " + SDL_GetError());
		int access;
		Uint32 format;
		int w, h;

		// Get the format, access, width and height of the created Texture, to
		// fill in the member Variables
		SDL_QueryTexture(returnTexture.m_Texture.get(), &format, &access, &w,
		                 &h);

		returnTexture.m_Access=access;
		returnTexture.m_Format=format;
		returnTexture.w = w;
		returnTexture.h = h;

		// Resize the pixels vector so we don't write out of bounds
		returnTexture.m_PixelsRO.resize(w * h);

		// Make a copy of the pixeldata to retain it for read access
		SDL_LockSurface(tempSurface.get());
		SDL_ConvertPixels(w, h, tempSurface->format->format,
		                  tempSurface->pixels, tempSurface->pitch, format,
		                  returnTexture.m_PixelsRO.data(),
		                  w * SDL_BYTESPERPIXEL(format));
		SDL_UnlockSurface(tempSurface.get());

		// In case a streaming texture was requested copy the pixels over now
		if (streamingAccess) {
			returnTexture.lock();

			// Copy the pixels from the read access array to the texture
			// Do not modify the following line unless you know exactly what
			// you're doing
			SDL_ConvertPixels(w, h, returnTexture.m_Format,
			                  returnTexture.m_PixelsRO.data(),
			                  w * sizeof(uint32_t), returnTexture.m_Format,
			                  returnTexture.m_PixelsWO, returnTexture.m_Pitch);

			returnTexture.unlock();
		}
		// Set the blend mode to blend so that alpha is actually used
		returnTexture.setBlendMode(SDL_BLENDMODE_BLEND);

		return returnTexture;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FMOD::Sound * ContentFile::GetAsSound(bool abortGameForInvalidSound, bool asyncLoading) {
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
			s_LoadedSamples.insert({ m_DataPath, returnSample });
		}
		return returnSample;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FMOD::Sound * ContentFile::LoadAndReleaseSound(bool abortGameForInvalidSound, bool asyncLoading) {
		if (m_DataPath.empty() || !g_AudioMan.IsAudioEnabled()) {
			return nullptr;
		}

		if (!std::filesystem::exists(m_DataPath)) {
			bool foundAltExtension = false;
			for (const std::string &altFileExtension : c_SupportedAudioFormats) {
				if (std::filesystem::exists(m_DataPathWithoutExtension + altFileExtension)) {
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
			const std::string errorMessage = "Failed to create sound because because the file was empty. The path and name were: ";
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
}
