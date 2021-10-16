#ifndef _RTECONTENTFILE_
#define _RTECONTENTFILE_

#include "Serializable.h"
#include "Renderer/GLTexture.h"

namespace FMOD { class Sound; }

namespace RTE {

	/// <summary>
	/// A representation of a content file that is stored directly on disk.
	/// </summary>
	class ContentFile : public Serializable {

	public:

		SerializableClassNameGetter;
		SerializableOverrideMethods;

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a ContentFile object in system memory. Create() should be called before using the object.
		/// </summary>
		ContentFile() { Clear(); }

		/// <summary>
		/// Constructor method used to instantiate a ContentFile object in system memory, and also do a Create() in the same line.
		/// </summary>
		/// <param name="filePath">A string defining the path to where the content file itself is located.</param>
		explicit ContentFile(const char *filePath) { Clear(); Create(filePath); }

		/// <summary>
		/// Constructor method used to instantiate a ContentFile object in system memory from a hash value of the file path, and also do a Create() in the same line.
		/// </summary>
		/// <param name="hash">A hash value containing the path to where the content file itself is located.</param>
		explicit ContentFile(size_t hash) { Clear(); Create(GetPathFromHash(hash).c_str()); }

		/// <summary>
		/// Makes the ContentFile object ready for use.
		/// </summary>
		/// <param name="filePath">A string defining the path to where the content file itself is located.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(const char *filePath);

		/// <summary>
		/// Creates a ContentFile to be identical to another, by deep copy.
		/// </summary>
		/// <param name="reference">A reference to the ContentFile to deep copy.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(const ContentFile &reference);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to clean up a ContentFile object before deletion from system memory.
		/// </summary>
		~ContentFile() override = default;

		/// <summary>
		/// Resets the entire ContentFile, including its inherited members, to their default settings or values.
		/// </summary>
		void Reset() override { Clear(); }

#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the ID of the Data Module this file is inside.
		/// </summary>
		/// <returns>The ID of the DataModule containing this' file.</returns>
		int GetDataModuleID() const;

		/// <summary>
		/// Gets the file path of the content file represented by this ContentFile object.
		/// </summary>
		/// <returns>A string with the file name path.</returns>
		const std::string & GetDataPath() const { return m_DataPath; }

		/// <summary>
		/// Sets the file path of the content file represented by this ContentFile object.
		/// </summary>
		/// <param name="newDataPath">A string with the new file name path.</param>
		void SetDataPath(const std::string &newDataPath);

		/// <summary>
		/// Sets the DataPath combined with the file and line it's being created from. This is used in cases we can't get the file and line from Serializable::Create(&reader).
		/// For example when creating a ContentFile for the sound during the readSound lambda in SoundContainer::ReadAndGetSound.
		/// </summary>
		/// <param name="newPosition">The file and line that are currently being read.</param>
		void SetFormattedReaderPosition(const std::string &newPosition);

		/// <summary>
		/// Creates a hash value out of a path to a ContentFile.
		/// </summary>
		/// <returns>Hash value of a path to a ContentFile.</returns>
		size_t GetHash() const { return std::hash<std::string>()(m_DataPath); }

		/// <summary>
		/// Converts hash values into file paths to ContentFiles.
		/// </summary>
		/// <param name="hash">Hash value to get file path from.</param>
		/// <returns>Path to ContentFile.</returns>
		static std::string GetPathFromHash(size_t hash) { return (s_PathHashes.find(hash) == s_PathHashes.end()) ? "" : s_PathHashes[hash]; }
#pragma endregion

#pragma region Data Handling
		/// <summary>
		/// Gets the data represented by this ContentFile object as an
		/// RTE::Texture object, loading it into the static maps if it's not
		/// already loaded.
		/// </summary>
		/// <param name="storeBitmap">Whether to store the Texture in the
		/// relevant static map after loading it or not.</param>
		/// <param name="dataPathToSpecificFrame">
		/// Path to a specific frame when loading
		/// an animation to avoid overwriting the original preset DataPath when
		/// loading each frame.
		/// </param>
		/// <param name="textureAccess">
		/// The access specifier used in creation of the Texture
		/// </param>
		/// <returns>Pointer to the SDL_Texture loaded
		/// from disk.</returns>
		std::shared_ptr<GLTexture> GetAsTexture(bool storeBitmap = true,
		                     const std::string &dataPathToSpecificFrame = "",
		                     bool streamingAccess = false);

		/// <summary>
		/// Gets the data represented by this ContentFile object as an array of SDL_Textures, each representing a frame in the animation.
		/// It loads the SDL_Textures into the static maps if they're not already loaded.
		/// </summary>
		/// <param name="frameCount">The number of frames to attempt to load, more than 1 frame will mean 00# is appended to datapath to handle naming conventions.</param>
		/// <returns>Pointer to the beginning of the array of SDL_Texture pointers loaded from the disk, the length of which is specified with the FrameCount argument.</returns>
		std::vector<std::shared_ptr<GLTexture>> GetAsAnimation(int frameCount = 1);

		/// <summary>
		/// Gets the data represented by this ContentFile object as an array of SDL_Textures, each representing a frame in the animation.
		/// It loads the SDL_Textures into the static maps if they're not already loaded.
		/// </summary>
		/// <param name="frameCount">The number of frames to attempt to load, more than 1 frame will mean 00# is appended to datapath to handle naming conventions.</param>
		/// <returns>Pointer to the beginning of the array of SDL_Texture pointers loaded from the disk, the length of which is specified with the FrameCount argument.</returns>
		void GetAsAnimation(std::vector<std::shared_ptr<GLTexture>> & vectorToFill, int frameCount = 1) {vectorToFill = GetAsAnimation(frameCount);};

		/// <summary>
		/// Gets the data represented by this ContentFile object as an FMOD FSOUND_SAMPLE, loading it into the static maps if it's not already loaded. Ownership of the FSOUND_SAMPLE is NOT transferred!
		/// </summary>
		/// <param name="abortGameForInvalidSound">Whether to abort the game if the sound couldn't be added, or just show a console error. Default true.</param>
		/// <param name="asyncLoading">Whether to enable FMOD asynchronous loading or not. Should be disabled for loading audio files with Lua AddSound.
		/// <returns>Pointer to the FSOUND_SAMPLE loaded from disk.</returns>
		FMOD::Sound * GetAsSound(bool abortGameForInvalidSound = true, bool asyncLoading = true);
#pragma endregion

	protected:

		static std::unordered_map<size_t, std::string> s_PathHashes; //!< Static map containing the hash values of paths of all loaded data files.
		static std::unordered_map<std::string, std::shared_ptr<GLTexture>> s_LoadedTextures; //!< Static map containing all the already loaded SDL_Textures and their paths.
		static std::unordered_map<std::string, FMOD::Sound *> s_LoadedSamples; //!< Static map containing all the already loaded FSOUND_SAMPLEs and their paths.

		std::string m_DataPath; //!< The path to this ContentFile's data file. In the case of an animation, this filename/name will be appended with 000, 001, 002 etc.
		std::string m_DataPathExtension; //!< The extension of the data file of this ContentFile's path.
		std::string m_DataPathWithoutExtension; //!< The path to this ContentFile's data file without the file's extension.

		std::string m_FormattedReaderPosition; //!< A string containing the currently read file path and the line being read. Formatted to be used for logging.
		std::string m_DataPathAndReaderPosition; //!< The path to this ContentFile's data file combined with the ini file and line it is being read from. This is used for logging.

		int m_DataModuleID; //!< Data Module ID of where this was loaded from.

	private:

		static const std::string c_ClassName; //!< A string with the friendly-formatted type name of this object.

#pragma region Data Handling
		/// <summary>
		/// Loads and transfers the data represented by this ContentFile object as an SDL_Texture. Ownership of the SDL_Texture IS transferred!
		/// Note that this is relatively slow since it reads the data from disk each time.
		/// Also note that the texture is created with static access which means
		/// it will render out very fast, but will not allow any pixel level access.
		/// </summary>
		/// <param name="dataPathToSpecificFrame">Path to a specific frame when loading an animation to avoid overwriting the original preset DataPath when loading each frame.</param>
		/// <returns>Pointer to the SDL_Texture loaded from disk.</returns>
		SharedTexture LoadAndReleaseImage(const std::string &dataPathToSpecificFrame,
		                            bool streamingAccess);

		/// <summary>
		/// Loads and transfers the data represented by this ContentFile
		/// object as an FMOD FSOUND_SAMPLE. Ownership of the FSOUND_SAMPLE
		/// is NOT transferred!
		/// </summary>
		/// <param name="abortGameForInvalidSound">Whether to abort the game
		/// if the sound couldn't be added, or just show a console error.
		/// Default true.</param> <param name="asyncLoading">Whether to
		/// enable FMOD asynchronous loading or not. Should be disabled for
		/// loading audio files with Lua AddSound.</param> <returns>Pointer
		/// to the FSOUND_SAMPLE loaded from disk.</returns>
		FMOD::Sound *LoadAndReleaseSound(bool abortGameForInvalidSound = true,
		                                 bool asyncLoading = true);
#pragma endregion

		/// <summary>
		/// Clears all the member variables of this ContentFile, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear();
	};
}
#endif
