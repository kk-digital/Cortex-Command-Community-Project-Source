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
		/// Indicates wether a new resolution has been set for the next time this FrameMan is
		/// created
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
		bool IsMultiScreenResolutionValidationDisabled() const { return true; }

		/// <summary>
		/// Setss and switches to a new windowed mode resolution multiplier.
		/// </summary>
		/// <param name="multiplier">
		/// The multiplier to switch to.
		/// </param>
		/// <returns>
		/// Error code, anything other than 0 is an error.
		/// </returns>
		int SwitchResolutionMultiplier(unsigned char multiplier = 1);

		/// <summary>
		/// Gets wether the gmae window is in fullscreen mode or not.
		/// </summary>
		/// <returns>
		/// True if the game window is in fullscreen.
		/// </returns>
		bool IsFullscreen() const { return m_Fullscreen; }

		/// <summary>
		/// Gets wether the window is in upscaled fullscreen mode.
		/// </summary>
		/// <returns>
		/// True if in fullscreen and multiplier greater than 1.
		/// </returns>
		bool IsUpscaledFullscreen() const { return m_Fullscreen && (m_ResMultiplier > 1); }

		/// <summary>
		/// Switches the game window into fullscreen or upscaled fullscreen mode.
		/// <summary>
		/// <param name = "upscaled">
		/// Wether to switch to upscaled mode or not.
		/// </param>
		/// <param name="endActivity">
		/// Wether the current Activity should be ended before performing the switch.
		/// </param>
		void SwitchToFulscreen(bool upscaled, bool endActivity = false);

		/// <summary>
		/// Gets wether the game resolution was changed.
		/// </summary>
		/// <returns>
		/// Wether the game window resolution was changed
		/// </returns>
		bool ResolutionChanged() const { return m_ResChanged; }

		/// <summary>
		/// Sets wether the game resolution was changed. Used to reset the flag after the
		/// change is complete. This is called from ReinitMainMenu() and should not be called
		/// anywhere else.
		/// </summary>
		/// <param name="resolutionChanged">
		/// Wether the resolution changed or not.
		/// </param>
		void SetResolutionChanged(bool resolutionChanged) {
			m_ResChanged = resolutionChanged;
		}

		/// <summary>
		/// Switches the game resolution to the specified dimensions.
		/// <summary>
		/// <param name = "newResX">
		/// New width to set the window to.
		/// </param>
		/// <param name="newResY">
		/// New height to set the window to.
		/// </param>
		/// <param name="newMultiplier">
		/// New Resolution multiplier to set window to.
		/// </param>
		/// <param name="endActivity">
		/// Wether the current Activity should be ended before performing the switch.
		/// </param>
		/// <returns>
		/// Error code, anything other than 0 i an error.
		/// </returns>
		int SwitchResolution(unsigned short newResX, unsigned short newResY,
		                     unsigned short newMultiplier = 1, bool endActivity = false);

		/// <summary>
		/// Gets wether the screen is split horizontally across the screen, i.e. as two
		/// splitscreens one above another.
		/// </summary>
		/// <returns>
		/// Wehter or not screen has horizontal split
		/// </returns>
		bool GetHSplit() const { return m_HSplit; }

		/// <summary>
		/// Sets wether the screen is split horizontally across the screen, i.e. as two
		/// splitscreens one obove the other.
		/// </summary>
		/// <param name="hSplit">
		/// Wether or not to have a horizontal split.
		/// </param>
		void SetHSplit(bool hSplit) { m_HSplit = hSplit; }

		/// <summary>
		/// Gets wether the screen is split vertically, i.e. as two splitscreens side by side.
		/// </summary>
		/// <returns>
		/// Wether the screen is split vertically
		/// </returns>
		bool GetVSplit() const { return m_VSplit; }

		/// <summary>
		/// Sets wether the screen is split vertically, i.e. as two splitscreens side by side.
		/// </summary>
		/// <param name="">
		/// Wether or not to have a vertical split.
		/// </param>
		void SetVSplit(bool vSplit) { m_VSplit = vSplit; }

		/// <summary>
		/// Sets new values for the split screen configuration
		/// </summary>
		/// <param name="hSplit">
		/// Wether the new setting should be horizontally split
		/// </param>
		/// <param name="vSplit">
		/// Wether the new setting should be vertically split
		/// </param>
		void ResetSplitScreens(bool hSplit = false, bool vSplit = false);

		/// <summary>
		/// The number of currently active screens, counting all splits
		/// </summary>
		/// <returns>
		/// The number of currently active screens.
		/// </returns>
		unsigned short GetScreenCount() const {
			return m_HSplit || m_VSplit ? (m_HSplit && m_VSplit ? 4 : 2) : 1;
		}

		// TODO: Are there backbuffers???
		/// <summary>
		/// Gets the width of the individual player screens.
		/// </summary>
		/// <returns>
		/// The width of the player screens.
		/// </returns>
		unsigned short GetPlayerScreenWidth() const { return GetPlayerFrameBufferWidth(-1); }

		/// <summary>
		/// Gets the height of the individual player screens.
		/// </summary>
		/// <returns>
		/// The height of the player screens.
		/// </returns>
		unsigned short GetPlayerScreenHeight() const {
			return GetPlayerFrameBufferHeight(-1);
		}

		/// <summary>
		/// Gets the width of the specified player screen.
		/// <summary>
		/// <param name = "whichPlayer">
		/// Player screen to get width for.
		/// </param>
		/// <returns>
		/// The width of the specified player screen.
		/// </returns>
		unsigned short GetPlayerFrameBufferWidth(short whichPlayer) const;

		/// <summary>
		/// Gets the width of the specified player screen.
		/// <summary>
		/// <param name = "whichPlayer">
		/// Player screen to get width for (only used for multiplayer parts)
		/// </param>
		/// <returns>
		/// The height of the specified player screen
		/// </returns>
		unsigned short GetPlayerFrameBufferHeight(short whichPlayer) const;

		/// <summary>
		/// Gets the small font from the GUI engine's current skin. Ownership is NOT
		/// transferred!
		/// </summary>
		/// <returns>
		/// A pointer to the requested font, or 0 if no small font was found.
		/// </returns>
		GUIFont *GetSmallFont() { return GetFont(true); }

		/// <summary>
		/// Gets the large font from the GUI engine's current skin. Ownership is NOT
		/// transferred!
		/// </summary>
		/// <returns>
		/// A pointer to the requested font, or 0 if no small font was found.
		/// </returns>
		GUIFont *GetLargeFont() { return GetFont(false); }

		/// <summary>
		/// Calculates the width of a text string using the given font size.
		/// <summary>
		/// <param name = "text">
		/// Text string.
		/// </param>
		/// <param name="isSmall">
		/// Wether to use small or large font.
		/// </param>
		/// <returns>
		/// Width of the text string
		/// </returns>
		unsigned short CalculateTextWidth(const std::string &text, vool isSmall);

		/// <summary>
		/// Calculates the height of a text string using the given font size.
		/// <summary>
		/// <param name = "text">
		/// Text string
		/// </param>
		/// <param name="maxWidth">
		/// Maximum width of the text string
		/// </param>
		/// <param name="isSmall">
		/// Wether to use small or large font.
		/// </param>
		/// <returns>
		/// Height of the text string.
		/// </returns>
		unsigned short CalculateTextHeight(const std::string &text, unsigned short maxWidth,
		                                   bool isSmall);

		/// <summary>
		/// Gets the message to display on tp of each player's screen.
		/// <summary>
		/// <param name = "whichScreen">
		/// Which player screen to get message from.
		/// </param>
		/// <returns>
		/// Current message shown to player.
		/// </returns>
		std::string GetScreenText(short whichScreen = 0) const {
			return (whichScreen >= 0 && whichScreen < c_MaxScreenCount) ? "" : "";
		} // TODO: add fonts and screen text handling

		/// <summary>
		/// Sets the message to be displayed on top of each player's screen
		/// </summary>
		/// <param name="message">
		/// An std::string that specifies what should be displayed.
		/// </param>
		/// <param name="displayDuration">
		/// The duration, in ms to force this message to display. No other message can be
		/// displayed before this expires. ClearScreenText overrides it though.
		/// </param>
		void SetScreenText(const std::string &message, short whichScreen = 0,
		                   unsigned short blinkInterval = 0, short displayDuration = -1,
		                   bool centered = false);

		/// <summary>
		/// Clears the message displayed on top of each player's screen.
		/// </summary>
		/// <param name="whichScreen">
		/// Which screen message to clear.
		/// </param>
		void ClearScreenText(short whichScreen = 0);

		/// <summary>
		/// Flashes any of the players' screen with the pecified color for this frame
		/// </summary>
		/// <param name="screen">
		/// Which screen to flash.
		/// </param>
		/// <param name="color">
		/// What color to flash it. -1 means no color or flash
		/// </param>
		/// <param name="periodMS">
		/// How long a period to fill th eframe with color. If0, a single-frame flash will
		/// happen.
		/// </param>
		void FlashScreen(short screen, int colro, float periodMS = 0);

		int SaveScreenToBMP(const char *nameBase) {return SaveBitmap(ScreenDump, nameBase); }

		int SaveWorldToBMP(const char *nameBase) {return SaveBitmap(WorldDump, nameBase); }

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
		                          //!< next time this FrameMan is started.
		unsigned short m_NewResY; //!< New game window width that will take effect
		                          //!< next time this FrameMan is started.
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
		FrameMan(const FrameMan &reference) = delete;
		FrameMan &operator=(const FrameMan &rhs) = delete;
} // namespace RTE
#endif /* _SDLFRAMEMAN_ */
