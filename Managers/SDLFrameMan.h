#ifndef _SDLFRAMEMAN_
#define _SDLFRAMEMAN_

#include "ContentFile.h"
#include "Timer.h"
#include "Box.h"
#include "SDL2/SDL.h"

#define g_FrameMan FrameMan::Instance()

namespace RTE {
	class SDLScreen;
	class SDLBitmap;
	class GUIFont;

	/// <summary>
	/// The Singleton manager over the composition and display of frames.
	/// </summary>

	class FrameMan : public Singleton<FrameMan>, public Serializable {

	public:
		SerializableOverrideMethods

		/// <summary>
		/// Constructor method used to instantiate a FrameMan object in system
		/// memory. Create() should be called before using the object.
		/// </summary>
		FrameMan() {
			Clear();
		}

		/// <summary>
		/// Makes the FrameMan object ready for use, which is to be used with
		/// SettingsMan first.
		/// </summary>
		/// <returns>
		/// An error return value signaling success or any particular failure.
		/// Values below 0 indicate an error signal.
		/// </returns>
		int Create() override;

		/// <summary>
		/// Destructor method used to clean up a FrameMan object before deletion
		/// from system memory
		/// </summary>
		virtual ~FrameMan();

		/// <summary>
		/// Destroys and resets (through Clear()) the FrameMan object.
		/// </summary>
		void Destroy();

		/// <summary>
		/// Resets the entire FrameMan, including its inherited memembers, to their
		/// default settings or values.
		/// </summary>
		void Reset() override { Clear(); }

		/// <summary>
		/// Updates the state of this FrameMan. Supposed to be done every frame.
		/// </summary>
		void Update();

		/// <summary>
		/// Draws the current frame to the screen.
		/// </summary>
		void Draw();

		/// <summary>
		/// Gets the horizontal resolution of the screen.
		/// </summary>
		/// <returns>
		/// An int describing the horizontal resolution of the screen in pixels.
		/// </returns>
		unsigned short GetResX() const { return m_ResX; }

		/// <summary>
		/// Gets the vertical resolution of the screen
		/// </summary>
		/// <returns>
		/// An int describing the vertical resolution
		/// </returns>
		unsigned short GetResY() const { return m_ResY; }

		/// <summary>
		/// Gets the horizontal resolution of the screen that will be used the next
		/// time this FrameMan is created.
		/// </summary>
		/// <returns>
		/// An int describing the horizontal resolution of the new screen in pixels.
		/// </returns>
		unsigned short GetNewResX() const { return m_NewResX; }

		/// <summary>
		/// Sets the horizontal resolution of the screen that will be used next time
		/// this FrameMan is created.
		/// </summary>
		/// <param name="newResX">
		/// an int describing the horizontal resolution of the new screen in pixels.
		/// </param>
		void SetNewResX(unsigned short newResX) { m_NewResX = newResX; }

		/// <summary>
		/// Gets the vertical resolution of the screen that will be used the next
		/// time this FrameMan is created.
		/// </summary>
		/// <returns>
		/// An int describing the horizontal resolution of the new screen in pixels.
		/// </returns>
		unsigned short GetNewResY() const { return m_NewResY; }

		/// <summary>
		/// Sets the vertical resolution that will be used the next time this
		/// FrameMan is created
		/// </summary>
		/// <param name="newResY">
		/// An int describing the horizontal resolution of the new screen in pixels.
		/// </param>
		void SetNewResY(unsigned short newResY) { m_NewResY = newResY; }

		/// <summary>
		/// Indicates wether a new resolution has been set for the next time this FrameMan is created
		/// </summary>
		/// <returns>
		/// Wether the new resolution set differs from the current one.
		/// </returns>
		bool IsNewResSet() const { return m_NewResX != m_ResX || m_NewResY != m_ResY; }

		/// <summary>
		/// Returns true if this resolution is supported
		/// </summary>
		/// <param name="width">
		/// Resolution width.
		/// </param>
		/// <param name="height">
		/// Resolution height
		/// </param>
		/// <returns>
		/// True if the resolution is supported.
		/// </returns>
		bool IsValidResolution(unsigned short width, unsigned short height) const;

		/// <summary>
		/// Tells how many time the screen resolution is being multiplied
		/// </summary>
		/// <returns>
		/// What multiple the screen resolution is run in (1 normal)
		/// </returns>
		unsigned short ResolutionMultiplier() const { return m_ResMultiplier; }

		/// <summary>
		/// Gets wether resolution validation in multi-monitor mode is disabled
		/// </summary>
		/// <returns>
		/// Wether resolution validation is disabled for multi-monitor mode.
		/// </returns>
		bool IsMultiScreenResolutionValidationDisabled() const {return true;}

		const std::string &GetClassName() const override { return c_ClassName; }

	protected:
		static const std::string
		    c_ClassName; //!< The friendly-formatted type name of this object

		SDL_Window *m_Window; //!< The game window
		SDL_Renderer *m_Renderer; //!< The renderer instance needed for drawing

		SDL_Rect m_Resolution; //!< Screen area excluding things like start menus,
		                       //!< window decoration, etc.
		static constexpr unsigned short m_BPP = 32; //!< Color Depth (bits per pixel)
		unsigned short m_NumScreens; //!< Number of physical displays
		unsigned short m_ScreenResX; //!< Horizontal resolution of the primary display
		unsigned short m_ScreenResY; //!< Vertical resolution of the primary display

		unsigned short m_ResX; //!< Game window width.
		unsigned short m_ResY; //!< Game window height.
		unsigned short m_ResMultiplier; //!< Resolution multiplier
		unsigned short m_NewResX; //!< New game window height that will take effect
		                          //!< next time the FrameMan is started.
		unsigned short m_NewResY; //!< New game window width that will take effect
		                          //!< next time the FrameMan is started.
		unsigned short m_NewResMultiplier; //!< New multiplier that will take effect
		                                   //!< the next time FrameMan is started
		bool m_ResChanged; //!< Wether the resolution was changed through the
		                   //!< settings fullscreen/upscaled fullscreen buttons.

		bool m_Fullscreen; //!< Wether in fullscreen mode or not.
		bool m_UpscaledFullscreen; //!< Wether in upscaled fullscreen mode or not

		bool m_HSplit; //!< Wether the screen is split horizontally across the
		               //!< screen, i.e. as two scplitscreens above another.
		bool m_VSplit; //!< Wether the screen is split vertically across the screen,
		               //!< i.e. as two splitscreens side by side.
		bool m_HSplitOverride; //!< Wether the screen is set to split horizontally
		                       //!< in settings
		bool m_VSplitOverride; //!< Wether the screen is set to split vertically in
		                       //!< settings

		ContentFile m_PaletteFile; //!< File of the screen palette
		SDL_Palette m_Palette; // TODO: figure out if this works with Accelerated rendering

	private:
		/// <summary>
		/// Check that a resolution will fit the screen(s) and fall back to a safe resolution
		/// if the set resolution was to high.
		/// </summary>
		/// <param name="resX">
		/// Game window horizontal resolution
		/// </param>
		/// <param name="resY">
		/// Game window vertical resolution
		/// </param>
		/// <param name="reMultiplier">
		/// Resolution multiplier
		/// </param>
		void ValidateResolution(unsigned short &resX, unsigned short &resY,
		                        unsigned short &resMultiplier);

		/// <summary>
		/// Clears all the member variables of this FrameMan, effectively resetting the
		/// memebers of this abstraction level only
		/// </summary>
		void Clear();

		// Disallow the use of some implicit methods.
		FrameMan(const FrameMan &reference) {}
		FrameMan &operator=(const FrameMan &rhs) {}
	};
} // namespace RTE
#endif /* _SDLFRAMEMAN_ */
