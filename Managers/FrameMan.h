#ifndef _SDLFRAMEMAN_
#define _SDLFRAMEMAN_

#include "System/Singleton.h"
#include "System/Box.h"
#include "Timer.h"
#include "ContentFile.h"

#include <SDL2/SDL_rect.h>

#include "Renderer/GLTexture.h"
#include "Renderer/GLPalette.h"

#define g_FrameMan FrameMan::Instance()


struct SDL_Window;

namespace RTE {
	class SDLScreen;
	class SDLGUITexture;
	class GUIFont;
	class RenderTarget;
	class RenderTexture;

	struct sdl_window_deleter {
		void operator()(SDL_Window *p);
	};
	struct sdl_context_deleter {
		void operator()(void *p);
	};

	/// <summary>
	/// The Singleton manager over the composition and display of frames.
	/// </summary>

	class FrameMan : public Singleton<FrameMan> {
		friend class SettingsMan;

	public:
		/// <summary>
		/// Constructor method used to instantiate a FrameMan object in system
		/// memory. Create() should be called before using the object.
		/// </summary>
		FrameMan();

		int CreateWindowAndRenderer();

		/// <summary>
		/// Makes the FrameMan object ready for use, which is to be used with
		/// SettingsMan first.
		/// </summary>
		/// <returns>
		/// An error return value signaling success or any particular failure.
		/// Values below 0 indicate an error signal.
		/// </returns>
		int Initialize();

		/// <summary>
		/// Destructor method used to clean up a FrameMan object before deletion
		/// from system memory
		/// </summary>
		~FrameMan();

		/// <summary>
		/// Destroys and resets (through Clear()) the FrameMan object.
		/// </summary>
		void Destroy();

		/// <summary>
		/// Updates the state of this FrameMan. Supposed to be done every frame.
		/// </summary>
		void Update();

		/////////////////
		/* Splitscreen */
		/////////////////

		/// <summary>
		/// Gets wether the screen is split horizontally across the screen, i.e.
		/// as two splitscreens one above another.
		/// </summary>
		/// <returns>
		/// Wehter or not screen has horizontal split
		/// </returns>
		bool GetHSplit() const { return m_HSplit; }

		/// <summary>
		/// Sets wether the screen is split horizontally across the screen, i.e.
		/// as two splitscreens one obove the other.
		/// </summary>
		/// <param name="hSplit">
		/// Wether or not to have a horizontal split.
		/// </param>
		void SetHSplit(bool hSplit) { m_HSplit = hSplit; }

		/// <summary>
		/// Gets wether the screen is split vertically, i.e. as two splitscreens
		/// side by side.
		/// </summary>
		/// <returns>
		/// Wether the screen is split vertically
		/// </returns>
		bool GetVSplit() const { return m_VSplit; }

		/// <summary>
		/// Sets wether the screen is split vertically, i.e. as two splitscreens
		/// side by side.
		/// </summary>
		/// <param name="">
		/// Wether or not to have a vertical split.
		/// </param>
		void SetVSplit(bool vSplit) { m_VSplit = vSplit; }

		bool GetTwoPlayerVSplit() const { return m_TwoPlayerVSplit; }

		void SetTwoPlayerVSplit(bool twoPlayerVSplit) { m_TwoPlayerVSplit = twoPlayerVSplit; }

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
		unsigned short GetScreenCount() const { return m_HSplit || m_VSplit ? (m_HSplit && m_VSplit ? 4 : 2) : 1; }

		/// <summary>
		/// Gets the width of the individual player screens.
		/// </summary>
		/// <returns>
		/// The width of the player screens.
		/// </returns>
		int GetPlayerScreenWidth() const { return GetPlayerFrameBufferWidth(-1); }

		/// <summary>
		/// Gets the height of the individual player screens.
		/// </summary>
		/// <returns>
		/// The height of the player screens.
		/// </returns>
		int GetPlayerScreenHeight() const { return GetPlayerFrameBufferHeight(-1); }

		/// <summary>
		/// Gets the width of the specified player screen.
		/// <summary>
		/// <param name = "whichPlayer">
		/// Player screen to get width for.
		/// </param>
		/// <returns>
		/// The width of the specified player screen.
		/// </returns>
		int GetPlayerFrameBufferWidth(short whichPlayer) const;

		/// <summary>
		/// Gets the width of the specified player screen.
		/// <summary>
		/// <param name = "whichPlayer">
		/// Player screen to get width for (only used for multiplayer parts)
		/// </param>
		/// <returns>
		/// The height of the specified player screen
		/// </returns>
		int GetPlayerFrameBufferHeight(short whichPlayer) const;

		/***********************
		 * Renderer Management *
		 ***********************/

		/// <summary>
		/// Get the current render target.
		/// </summary>
		/// <returns>
		/// A Pointer to the current RenderTarget, non owning.
		/// </returns>
		RenderTarget *GetRenderer() { return m_Renderer.get(); }

		void * GetContext() {return m_Context.get();}

		void RenderClear();

		/// <summary>
		/// Show the current Frame on the screen.
		/// </summary>
		void RenderPresent();

		/// <summary>
		/// Draws the current frame to the screen.
		/// </summary>
		void Draw();

		/// <summary>
		/// Get the default shader for textured vertices at the given bitdepth.
		/// </summary>
		/// <param name="depth">
		/// The requested bit depth.
		/// </param>
		/// <returns>
		/// A shared pointer to the requested shader.
		/// </returns>
		std::shared_ptr<Shader> GetTextureShader(BitDepth depth);

		/// <summary>
		/// Get the fill shader for textured vertices at the given bitdepth.
		/// </summary>
		/// <param name="depth">
		/// The requested bit depth.
		/// </param>
		/// <returns>
		/// A shared pointer to the requested shader.
		/// </returns>
		std::shared_ptr<Shader> GetTextureShaderFill(BitDepth depth);

		/// <summary>
		/// Get the fill shader for colored vertices.
		/// </summary>
		/// <returns>
		/// A shared pointer to the requested shader.
		/// </returns>
		std::shared_ptr<Shader> GetColorShader();

		RenderTexture* GetPostProcessFramebuffer() {return m_PostProcessFramebuffer.get();}
		RenderTexture* GetGUIFramebuffer() {return m_GUIFramebuffer.get();}

		void FadeInPalette();

		std::shared_ptr<Palette> GetDefaultPalette() {return m_DefaultPalette;}

		/*********************
		 * Window Management *
		 *********************/

		/// <summary>
		/// Get the current game window as SDL_Window
		/// </summary>
		/// <returns>
		/// A pointer to the current SDL_Window
		/// </returns>
		SDL_Window *GetWindow() { return m_Window.get(); }

		/// <summary>
		/// Get the pixelformat of the currently used window
		/// </summary>
		/// <returns>
		/// An Uint32 representing a pixelformat from SDL_PixelFormatEnum
		/// </returns>
		uint32_t GetPixelFormat() const;

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

		unsigned short GetMaxResX() const { return m_ScreenRes->w; }
		unsigned short GetMaxResY() const { return m_ScreenRes->h; }

		/// <summary>
		/// Gets the horizontal resolution of the screen that will be used the
		/// next time this FrameMan is created.
		/// </summary>
		/// <returns>
		/// An int describing the horizontal resolution of the new screen in
		/// pixels.
		/// </returns>
		unsigned short GetNewResX() const { return m_NewResX; }

		/// <summary>
		/// Sets the horizontal resolution of the screen that will be used next
		/// time this FrameMan is created.
		/// </summary>
		/// <param name="newResX">
		/// an int describing the horizontal resolution of the new screen in
		/// pixels.
		/// </param>
		void SetNewResX(unsigned short newResX) { m_NewResX = newResX; }

		/// <summary>
		/// Gets the vertical resolution of the screen that will be used the
		/// next time this FrameMan is created.
		/// </summary>
		/// <returns>
		/// An int describing the horizontal resolution of the new screen in
		/// pixels.
		/// </returns>
		unsigned short GetNewResY() const { return m_NewResY; }

		/// <summary>
		/// Sets the vertical resolution that will be used the next time this
		/// FrameMan is created
		/// </summary>
		/// <param name="newResY">
		/// An int describing the horizontal resolution of the new screen in
		/// pixels.
		/// </param>
		void SetNewResY(unsigned short newResY) { m_NewResY = newResY; }

		void ChangeResolution(int newResX, int newResY, bool newResUpscaled, bool newFullscreen) {
			m_NewResX = newResX;
			m_NewResY = newResY;
		}

		/// <summary>
		/// Indicates wether a new resolution has been set for the next time
		/// this FrameMan is created
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
		bool IsValidResolution(int width, int height) const;

		/// <summary>
		/// Tells how many time the screen resolution is being multiplied
		/// </summary>
		/// <returns>
		/// What multiple the screen resolution is run in (1 normal)
		/// </returns>
		int GetResMultiplier() const { return m_ResMultiplier; }

		/// <summary>
		/// Gets wether resolution validation in multi-monitor mode is disabled
		/// </summary>
		/// <returns>
		/// Wether resolution validation is disabled for multi-monitor mode.
		/// </returns>
		bool IsMultiScreenResolutionValidationDisabled() const { return true; }

		/// <summary>
		/// Sets and switches to a new windowed mode resolution multiplier.
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
		bool IsUpscaledFullscreen() const {
			return m_Fullscreen && (m_ResMultiplier > 1);
		}

		/// <summary>
		/// Switches the game window into fullscreen or upscaled fullscreen
		/// mode.
		/// </summary>
		/// <param name = "upscaled">
		/// Wether to switch to upscaled mode or not.
		/// </param>
		/// <param name="endActivity">
		/// Wether the current Activity should be ended before performing the
		/// switch.
		/// </param>
		void SwitchToFullscreen(bool upscaled, bool endActivity = false);

		/// <summary>
		/// Set fullscreen mode according to <paramref name="fullscreen">
		/// </summary>
		/// <param name="fullscreen">
		/// Wether to go to fullscreen mode
		/// </param>
		/// <param name="endActivity">
		/// Wether the current Activity should be ended before performing the
		/// switch.
		/// </param>
		void SetFullscreen(bool fullscreen);

		/// <summary>
		/// Gets wether the game resolution was changed.
		/// </summary>
		/// <returns>
		/// Wether the game window resolution was changed
		/// </returns>
		bool ResolutionChanged() const { return m_ResChanged; }

		/// <summary>
		/// Sets wether the game resolution was changed. Used to reset the flag
		/// after the change is complete. This is called from ReInitMainMenu()
		/// and should not be called anywhere else.
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
		/// Wether the current Activity should be ended before performing the
		/// switch.
		/// </param>
		/// <returns>
		/// Error code, anything other than 0 i an error.
		/// </returns>
		int SwitchResolution(int newResX, int newResY, int newMultiplier = 1, bool endActivity = false);

		//////////////////////////
		/* Screen Text handling */
		//////////////////////////

		/// <summary>
		/// Gets the small font from the GUI engine's current skin. Ownership is
		/// NOT transferred!
		/// </summary>
		/// <returns>
		/// A pointer to the requested font, or 0 if no small font was found.
		/// </returns>
		GUIFont *GetSmallFont() { return GetFont(true); }

		/// <summary>
		/// Gets the large font from the GUI engine's current skin. Ownership is
		/// NOT transferred!
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
		int CalculateTextWidth(const std::string &text, bool isSmall);

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
		int CalculateTextHeight(const std::string &text, int maxWidth, bool isSmall);

		/// <summary>
		/// Gets the message to display on tp of each player's screen.
		/// <summary>
		/// <param name = "whichScreen">
		/// Which player screen to get message from.
		/// </param>
		/// <returns>
		/// Current message shown to player.
		/// </returns>
		std::string GetScreenText(int whichScreen = 0) const { return (whichScreen >= 0 && whichScreen < c_MaxScreenCount) ? m_ScreenText[whichScreen] : ""; }

		/// <summary>
		/// Sets the message to be displayed on top of each player's screen
		/// </summary>
		/// <param name="message">
		/// An std::string that specifies what should be displayed.
		/// </param>
		/// <param name="displayDuration">
		/// The duration, in ms to force this message to display. No other
		/// message can be displayed before this expires. ClearScreenText
		/// overrides it though.
		/// </param>
		void SetScreenText(std::string message, int whichScreen = 0, int blinkInterval = 0, long displayDuration = -1, bool centered = false);

		/// <summary>
		/// Clears the message displayed on top of each player's screen.
		/// </summary>
		/// <param name="whichScreen">
		/// Which screen message to clear.
		/// </param>
		void ClearScreenText(int whichScreen = 0);

		void DrawScreenText(int playerScreen, SDLGUITexture playerGUIBitmap); //TODO

		/* TODO: categorize these */
		/// <summary>
		/// Flashes any of the players' screen with the pecified color for this
		/// frame
		/// </summary>
		/// <param name="screen">
		/// Which screen to flash.
		/// </param>
		/// <param name="color">
		/// What color to flash it. -1 means no color or flash
		/// </param>
		/// <param name="periodMS">
		/// How long a period to fill the frame with color. If 0, a single-frame
		/// flash will happen.
		/// </param>
		void FlashScreen(int screen, uint32_t color, float periodMS = 0);

		int DrawLine(const Vector &start, const Vector &end, uint32_t color, int altColor = 0, int skip = 0, int skipStart = 0, bool shortestWrap = false) const { return 0; }
		int DrawDotLine(RenderTarget *renderer, const Vector &start, const Vector &end, SharedTexture dot, int skip = 0, int skipStart = 0, bool shortestWrap = false) const { return 0; }

		int SaveScreenToPNG(std::string nameBase) { return 0; };

		int SaveWorldToPNG(std::string nameBase) { return 0; };

		int SaveTextureToPNG(std::shared_ptr<GLTexture> tex, std::string nameBase) { return 0; };

		bool IsInMultiplayerMode() const { return false; }

		// Private members
	private:
		// FIXME: Add html/hex color handling to Reader and get rid of these
		ContentFile m_MatPaletteFile;
		SharedTexture m_MatPalette;
		// FIXME: Seriously, these must not stay

		//!< The game window
		std::unique_ptr<SDL_Window, sdl_window_deleter> m_Window;
		//!< The default render target
		std::unique_ptr<RenderTarget> m_Renderer;


		std::unique_ptr<void, sdl_context_deleter> m_Context;

		std::shared_ptr<SDLScreen> m_GUIScreen;
		std::shared_ptr<GUIFont> m_SmallFont;
		std::shared_ptr<GUIFont> m_LargeFont;

		//!< Maximum usable Screen area; In Windowed mode this excludes
		//!< Window decorations, task bar, etc.
		std::unique_ptr<SDL_Rect> m_ScreenRes;

		//!< Color Depth (bits per pixel)
		static constexpr unsigned short m_BPP = 32;

		std::shared_ptr<Shader> m_ShaderBase32; //!< Base Shader for 32bpp textures.
		std::shared_ptr<Shader> m_ShaderBase8; //!< Base shader for indexed 8bpp textures.
		std::shared_ptr<Shader> m_ShaderFill32; //!< Fill shader for 32bpp textures. (TODO: May just need one for  both)
		std::shared_ptr<Shader> m_ShaderFill8; //!< Fill shader for indexed textures.

		std::shared_ptr<Shader> m_ColorShader; //!< Shader for untextured vertex arrays. (i.e primitives and other shapes)

		//!< Number of physical displays
		int m_NumScreens;

		int m_ResX; //!< Game window width.
		int m_ResY; //!< Game window height.
		int m_ResMultiplier; //!< Resolution multiplier

		//!< New game window height that will take
		//!< effect next time this FrameMan is started.
		int m_NewResX;

		//!< New game window width that will take effect
		//!< next time this FrameMan is started.
		int m_NewResY;

		//!< New multiplier that will take effect
		//!< the next time FrameMan is started
		int m_NewResMultiplier;

		//!< Wether the resolution was changed through the
		//!< settings fullscreen/upscaled fullscreen buttons.
		bool m_ResChanged;

		//!< Wether in fullscreen mode or not.
		bool m_Fullscreen;

		//!< Wether in upscaled fullscreen mode or not
		bool m_UpscaledFullscreen;

		std::unique_ptr<RenderTexture> m_PlayerScreen;

		std::unique_ptr<RenderTexture> m_PostProcessFramebuffer;
		std::unique_ptr<RenderTexture> m_GUIFramebuffer;

		//!< A stack of the current render targets
		std::stack<RenderTarget *> m_TargetStack;

		ContentFile m_PaletteFile;
		bool m_PaletteUpdated;
		std::shared_ptr<Palette> m_NewPalette; //!< The new palette to fade in.
		std::shared_ptr<Palette> m_CurrentPalette; //!< The current palette.
		std::shared_ptr<Palette> m_DefaultPalette; //!< The default palette.

		//!< Wether the screen is split horizontally across the
		//!< screen, i.e. as two scplitscreens above another.
		bool m_HSplit;
		//!< Wether the screen is split vertically across the
		//!< screen, i.e. as two splitscreens side by side.
		bool m_VSplit;

		//!< Wether the screen is set to split vertically
		//!< in settings
		bool m_TwoPlayerVSplit;

		std::array<std::string, c_MaxScreenCount> m_ScreenText;
		std::array<bool, c_MaxScreenCount> m_TextCentered;
		std::array<long, c_MaxScreenCount> m_TextDuration;
		std::array<Timer, c_MaxScreenCount> m_TextDurationTimer;
		std::array<int, c_MaxScreenCount> m_TextBlinking;
		Timer m_TextBlinkTimer;

		std::array<uint32_t, c_MaxScreenCount> m_FlashScreenColor;
		std::array<bool, c_MaxScreenCount> m_FlashedLastFrame;
		std::array<Timer, c_MaxScreenCount> m_FlashTimer;

		// Private functions
	private:
		/// <summary>
		/// Check that a resolution will fit the screen(s) and fall back to a
		/// safe resolution if the set resolution was too high.
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
		// void ValidateResolution(int &resX, int &resY, int &resMultiplier);

		GUIFont *GetFont(bool isSmall);

		/// <summary>
		/// Clears all the member variables of this FrameMan, effectively
		/// resetting the memebers of this abstraction level only
		/// </summary>
		void Clear();

		void DrawScreenFlash(int playerScreen, RenderTarget *renderer);

		/// <summary>
		/// Updates the drawing position of each player screen on the backbuffer when split screen is active. This is called during Draw().
		/// </summary>
		/// <param name="playerScreen">The player screen to update offset for.</param>
		/// <param name="screenOffset">Vector representing the screen offset.</param>
		void UpdateScreenOffsetForSplitScreen(int playerScreen, Vector &screenOffset) const;

		void UpdateFramebuffers();

		// Disallow the use of some implicit methods.
		FrameMan(const FrameMan &reference) = delete;
		FrameMan &operator=(const FrameMan &rhs) = delete;
	};
} // namespace RTE
#endif /* _SDLFRAMEMAN_ */
