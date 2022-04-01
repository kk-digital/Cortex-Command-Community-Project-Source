#include "FrameMan.h"

#include "ContentFile.h"
#include "Timer.h"
#include "Box.h"
#include "PostProcessMan.h"
#include "PrimitiveMan.h"
#include "PerformanceMan.h"
#include "SceneMan.h"
#include "ActivityMan.h"
#include "ConsoleMan.h"
#include "SettingsMan.h"
#include "UInputMan.h"
#include "DebugMan.h"

#include "Renderer/RenderTarget.h"
#include "Renderer/RenderTexture.h"
#include "Renderer/GLTexture.h"
#include "Renderer/GLPalette.h"
#include "Renderer/Shader.h"

#include "Entities/SLTerrain.h"
#include "Entities/Scene.h"

#include "GUI/GUI.h"
#include "GUI/SDLGUITexture.h"
#include "GUI/SDLScreen.h"

#include "System/Constants.h"
#include "System/RTEError.h"

#include "System/SDLHelper.h"
#include "GraphicalPrimitive.h"
#include "SDL2_gfxPrimitives.h"

#include "GL/glew.h"
#include "Renderer/GLCheck.h"

namespace RTE {

	void sdl_window_deleter::operator()(SDL_Window *p) { SDL_DestroyWindow(p); }
	void sdl_context_deleter::operator()(void *p) { SDL_GL_DeleteContext(p); }

	FrameMan::FrameMan() { Clear();}

	FrameMan::~FrameMan() { Destroy(); }

	void FrameMan::Clear() {
		m_NumScreens = SDL_GetNumVideoDisplays();
		m_ScreenRes = std::make_unique<SDL_Rect>(SDL_Rect{0, 0, 0, 0});
		SDL_GetDisplayUsableBounds(0, m_ScreenRes.get());

		m_MatPaletteFile.Reset();
		m_PaletteFile.Create("Base.rte/palette.bmp");

		m_ResX = 960;
		m_ResY = 540;
		m_ResMultiplier = 1;
		m_NewResX = m_ResX;
		m_NewResY = m_ResY;

		m_ResChanged = false;
		m_Fullscreen = false;
		m_UpscaledFullscreen = false;

		m_HSplit = false;
		m_VSplit = false;
		m_TwoPlayerVSplit = false;

		while (!m_TargetStack.empty())
			m_TargetStack.pop();

		m_TargetStack.push(nullptr);
	}

	bool FrameMan::IsValidResolution(int width, int height) const {
		return (width >= 640 && height >= 480);
	}

	int FrameMan::CreateWindowAndRenderer() {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		m_Window = std::unique_ptr<SDL_Window, sdl_window_deleter>(SDL_CreateWindow(c_WindowTitle,
		    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ResX, m_ResY,
		    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS));

		RTEAssert(m_Window != NULL, "Could not create Window because: " + std::string(SDL_GetError()));
		m_Context = std::unique_ptr<void, sdl_context_deleter>(SDL_GL_CreateContext(m_Window.get()));

		RTEAssert(m_Context.get(), "Failed to get context: " + std::string(SDL_GetError()));

		GLenum err = glewInit();

		RTEAssert(err == GLEW_OK, "Failed to initialize GLEW: \n\t" + std::string(reinterpret_cast<const char*>(glewGetErrorString(err))));
		m_Renderer = std::make_unique<RenderTarget>();
		std::cout << SDL_GL_SetSwapInterval(0) << std::endl;

		return static_cast<bool>(m_Window && m_Renderer);
	}

	int FrameMan::Initialize() {

		SDL_SetWindowSize(m_Window.get(), m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier);

		SDL_GetWindowSize(m_Window.get(), &m_ResX, &m_ResY);
		m_ResX /= m_ResMultiplier;
		m_ResY /= m_ResMultiplier;

		glCheck(glViewport(0,0,m_ResX, m_ResY));
		glCheck(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		glCheck(glClear(GL_COLOR_BUFFER_BIT));

		SetFullscreen(m_Fullscreen);

		m_Renderer->Create(0,0,m_ResX, m_ResY);

		m_Renderer->DrawClear();


		m_CurrentPalette = m_DefaultPalette = m_PaletteFile.GetAsPalette();
		(*m_DefaultPalette)[0].a = 0;
		m_DefaultPalette->Update();
		m_DefaultPalette->Bind();

		// m_MatPaletteFile.Create("Base.rte/palettemat.bmp");
		// m_MatPalette = m_MatPaletteFile.GetAsTexture();
		// RTEAssert(m_MatPalette.get(), "Failed to load material palette because: " + std::string(SDL_GetError()));

		m_ShaderBase8 = std::make_shared<Shader>("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Indexed8Base.frag");
		m_ShaderFill8 = std::make_shared<Shader>("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Indexed8Fill.frag");

		m_ShaderBase8->Use();
		m_ShaderBase8->SetVector4f(m_ShaderBase8->GetColorUniform(), glm::vec4(1.0f));
		m_ShaderBase8->SetMatrix4f(m_ShaderBase8->GetTransformUniform(), glm::mat4(1.0f));
		m_ShaderBase8->SetMatrix4f(m_ShaderBase8->GetProjectionUniform(), glm::mat4(1.0f));
		m_ShaderBase8->SetInt(m_ShaderBase8->GetTextureUniform(), 0);
		m_ShaderBase8->SetInt(m_ShaderBase8->GetUniformLocation("rtePalette"), 1);

		m_ShaderFill8->Use();
		m_ShaderFill8->SetVector4f(m_ShaderFill8->GetColorUniform(), glm::vec4(1.0f));
		m_ShaderFill8->SetMatrix4f(m_ShaderFill8->GetTransformUniform(), glm::mat4(1.0f));
		m_ShaderFill8->SetMatrix4f(m_ShaderFill8->GetProjectionUniform(), glm::mat4(1.0f));
		m_ShaderFill8->SetInt(m_ShaderFill8->GetTextureUniform(), 0);
		m_ShaderFill8->SetInt(m_ShaderFill8->GetUniformLocation("rtePalette"), 1);

		m_ShaderBase32 = std::make_shared<Shader>("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Rgba32Base.frag");
		m_ShaderBase32->Use();
		m_ShaderBase32->SetVector4f(m_ShaderBase32->GetColorUniform(), glm::vec4(1.0f));
		m_ShaderBase32->SetMatrix4f(m_ShaderBase32->GetTransformUniform(), glm::mat4(1.0f));
		m_ShaderBase32->SetMatrix4f(m_ShaderBase32->GetProjectionUniform(), glm::mat4(1.0f));
		m_ShaderBase32->SetInt(m_ShaderBase32->GetTextureUniform(), 0);

		m_ShaderFill32 = std::make_shared<Shader>("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Rgba32Fill.frag");
		m_ShaderFill32->Use();
		m_ShaderFill32->SetVector4f(m_ShaderFill32->GetColorUniform(), glm::vec4(1.0f));
		m_ShaderFill32->SetMatrix4f(m_ShaderFill32->GetTransformUniform(), glm::mat4(1.0f));
		m_ShaderFill32->SetMatrix4f(m_ShaderFill32->GetProjectionUniform(), glm::mat4(1.0f));
		m_ShaderFill32->SetInt(m_ShaderFill32->GetTextureUniform(), 0);

		glCheck(glUseProgram(0));
		m_ColorShader = std::make_shared<Shader>("Base.rte/Shaders/Base.vert", "Base.rte/Shaders/Rgba32VertexColor.frag");
		m_ColorShader->Use();

		glCheck(glDisable(GL_CULL_FACE));

		return 0;
	}

	void FrameMan::Destroy() {
		Clear();
	}

	void FrameMan::Update() {
		g_PerformanceMan.Update();

		g_PrimitiveMan.ClearPrimitivesQueue();
	}

	void FrameMan::ResetSplitScreens(bool hSplit, bool vSplit) {
		if (!vSplit && !hSplit)
			m_PlayerScreen.reset();
		else {
			int resX = vSplit ? m_ResX / 2 : m_ResX;
			int resY = hSplit ? m_ResY / 2 : m_ResY;

			m_PlayerScreen = std::make_unique<RenderTexture>();
			m_PlayerScreen->Create(resX, resY);
		}
	}

	int FrameMan::GetPlayerFrameBufferWidth(short) const {
		if (m_PlayerScreen)
			return m_PlayerScreen->GetW();

		return m_ResX;
	}

	int FrameMan::GetPlayerFrameBufferHeight(short) const {
		if (m_PlayerScreen)
			return m_PlayerScreen->GetH();

		return m_ResY;
	}


	void FrameMan::RenderClear() {
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		m_Renderer->DrawClear();
	}

	void FrameMan::RenderPresent() {
		glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		SDL_GL_SwapWindow(m_Window.get());
	}

	void FrameMan::UpdateScreenOffsetForSplitScreen(int playerScreen, Vector &screenOffset) const {
		switch (playerScreen) {
			case Players::PlayerTwo:
				// If both splits, or just VSplit, then in upper right quadrant
				if ((m_VSplit && !m_HSplit) || (m_VSplit && m_HSplit)) {
					screenOffset.SetXY(GetResX() / 2, 0);
				} else {
					// If only HSplit, then lower left quadrant
					screenOffset.SetXY(0, GetResY() / 2);
				}
				break;
			case Players::PlayerThree:
				// Always lower left quadrant
				screenOffset.SetXY(0, GetResY() / 2);
				break;
			case Players::PlayerFour:
				// Always lower right quadrant
				screenOffset.SetXY(GetResX() / 2, GetResY() / 2);
				break;
			default:
				// Always upper left corner
				screenOffset.SetXY(0, 0);
				break;
		}
	}

	void FrameMan::DrawScreenFlash(int playerScreen, RenderTarget *renderer) {
	}

	void FrameMan::Draw() {
		m_CurrentPalette->Bind();
		// Count how many split screens we'll need
		int screenCount = (m_HSplit ? 2 : 1) * (m_VSplit ? 2 : 1);
		// RTEAssert(screenCount <= 1 || m_PlayerScreen,
		//           "Splitscreen surface not ready when needed!");

		g_PostProcessMan.ClearScreenPostEffects();

		// These accumulate the effects for each player's screen area, and are
		// then transferred to the post-processing lists with the player screen
		// offset applied
		std::list<PostEffect> screenRelativeEffects;
		std::list<Box> screenRelativeGlowBoxes;

		const Activity *pActivity = g_ActivityMan.GetActivity();

		for (int playerScreen = 0; playerScreen < screenCount; ++playerScreen) {
			RenderTarget* renderer = (screenCount == 1) ? m_Renderer.get() : m_PlayerScreen.get();

			glm::vec2 renderSize = renderer->GetSize();

			SDLGUITexture playerGUIBitmap(guiBuffer->GetTexture());
			screenRelativeEffects.clear();
			screenRelativeGlowBoxes.clear();
			// TODO Multiplayer
			// Update the scene view to line up with a specific screen and then
			// draw it onto the intermediate screen
			g_SceneMan.Update(playerScreen);

			Vector targetPos = g_SceneMan.GetOffset(playerScreen);

			// Adjust the drawing position on the target screen for if the
			// target screen is larger than the scene in non-wrapping dimension.
			// Scene needs to be displayed centered on the target bitmap then,
			// and that has to be adjusted for when drawing to the screen
			if (!g_SceneMan.SceneWrapsX() && renderSize.x > g_SceneMan.GetSceneWidth()) {
				targetPos.m_X += (renderSize.x - g_SceneMan.GetSceneWidth()) / 2;
			}
			if (!g_SceneMan.SceneWrapsY() && renderSize.y > g_SceneMan.GetSceneHeight()) {
				targetPos.m_Y += (renderSize.y - g_SceneMan.GetSceneHeight()) / 2;
			}

#ifdef NETWORK_ENABLED
			//Try to move at the frame buffer copy time to maybe prevent
			// wonkyness
			m_TargetPos[m_NetworkFrameCurrent][playerScreen] = targetPos;
#endif

			// Draw the scene
			if (!IsInMultiplayerMode()) {
				g_SceneMan.Draw(renderer, nullptr, targetPos);
			} else {
#ifdef NETWORK_ENABLED
				clear_to_color(drawScreen, g_MaskColor);
				clear_to_color(drawScreenGUI, g_MaskColor);
				g_SceneMan.Draw(drawScreen, drawScreenGUI, targetPos, true, true);
#endif
			}

			// Get only the scene-relative post effects that affect this
			// player's screen
			if (pActivity) {
				g_PostProcessMan.GetPostScreenEffectsWrapped(targetPos, renderSize.x, renderSize.y,
				    screenRelativeEffects, pActivity->GetTeamOfPlayer(pActivity->PlayerOfScreen(playerScreen)));

				g_PostProcessMan.GetGlowAreasWrapped(targetPos, renderSize.x, renderSize.y, screenRelativeGlowBoxes);

#ifdef NETWORK_ENBALED
				if (IsInMultiplayerMode()) {
					g_PostProcessMan.SetNetworkPostEffectsList(
					    playerScreen, screenRelativeEffects);
				}
#endif
			}

			DrawScreenText(playerScreen, playerGUIBitmap);

			// The position of the current draw screen on the backbuffer
			Vector screenOffset;

			// If we are dealing with split screens, then deal with the fact
			// that we need to draw the player screens to different locations on
			// the final buffer
			if (screenCount > 1) {
				UpdateScreenOffsetForSplitScreen(playerScreen, screenOffset);
			}

			DrawScreenFlash(playerScreen, renderer);

			if (!IsInMultiplayerMode()) {
				if (m_PlayerScreen)
					m_PlayerScreen->GetTexture()->render(m_Renderer.get(), screenOffset.GetFloorIntX(), screenOffset.GetFloorIntY());

				g_PostProcessMan.AdjustEffectsPosToPlayerScreen(playerScreen, renderSize.x, renderSize.y, screenOffset, screenRelativeEffects, screenRelativeGlowBoxes);
			}
		}

		// Clears the pixels that have been revealed from the unseen layers
		g_SceneMan.ClearSeenPixels();

		glm::vec2 renderSize = m_Renderer->GetSize();

		if (!IsInMultiplayerMode()) {
			// Draw separating lines for split-screens
			if (m_HSplit) {
				LinePrimitive(-1, {0.0f,renderSize.y/2 - 1}, {renderSize.x - 1, renderSize.y/2 - 1}, g_BlackColor).Draw(m_Renderer.get());
				LinePrimitive(-1, {0.0f, renderSize.y / 2}, {renderSize.x - 1, renderSize.y / 2}, g_BlackColor).Draw(m_Renderer.get());
			}
			if (m_VSplit) {
				LinePrimitive(-1, {renderSize.x/2 - 1, 0}, {renderSize.x / 2 - 1, renderSize.y}, g_BlackColor).Draw(m_Renderer.get());
				LinePrimitive(-1, {renderSize.x / 2, 0}, {renderSize.x / 2, renderSize.y}, g_BlackColor).Draw(m_Renderer.get());
			}
#ifdef NETWORK_ENABLED
			// Replace 8 bit backbuffer contents with network received image
			// before post-processing as it is where this buffer is copied to 32
			// bit buffer
			if (GetDrawNetworkBackBuffer()) {
				m_NetworkBitmapLock[0].lock();

				blit(m_NetworkBackBufferFinal8[m_NetworkFrameReady][0],
				    m_BackBuffer8, 0, 0, 0, 0, m_BackBuffer8->w,
				    m_BackBuffer8->h);
				masked_blit(
				    m_NetworkBackBufferFinalGUI8[m_NetworkFrameReady][0],
				    m_BackBuffer8, 0, 0, 0, 0, m_BackBuffer8->w,
				    m_BackBuffer8->h);

				if (g_UInputMan.FlagAltState() || g_UInputMan.FlagCtrlState() ||
				    g_UInputMan.FlagShiftState()) {
					g_PerformanceMan.DrawCurrentPing();
				}

				m_NetworkBitmapLock[0].unlock();
			}
#endif
		}

#ifdef NETWORK_ENABLED
		if (IsInMultiplayerMode()) {
			PrepareFrameForNetwork();
		}
#endif

		if (g_ActivityMan.IsInActivity()) {
			g_PostProcessMan.PostProcess();
		}

		// Draw the console on top of everything
		g_ConsoleMan.Draw(m_Renderer.get());

#ifdef DEBUG_BUILD
		// Draw scene seam
		LinePrimitive(-1, {0,0}, {0, 1.0f * g_SceneMan.GetSceneHeight()}, 5).Draw(m_Renderer.get());
#endif

		g_DebugMan.Draw();

		// Reset the frame timer so we can measure how much it takes until next
		// frame being drawn
		g_PerformanceMan.ResetFrameTimer();
	}

	std::shared_ptr<Shader> FrameMan::GetTextureShader(BitDepth depth) {
		switch (depth) {
			case BitDepth::BPP32: {
				return m_ShaderBase32;
			} break;
			case BitDepth::Indexed8: {
				return m_ShaderBase8;
			}
			default:
				return m_ShaderBase8;
		}
	}

	std::shared_ptr<Shader> FrameMan::GetTextureShaderFill(BitDepth depth) {
		switch (depth) {
			case BitDepth::BPP32:
				return m_ShaderFill32;
				break;
			case BitDepth::Indexed8:
				return m_ShaderFill8;
			default:
				return m_ShaderFill8;
		}
	}

	std::shared_ptr<Shader> FrameMan::GetColorShader() {
		return m_ColorShader;
	}

	uint32_t FrameMan::GetPixelFormat() const {
		return SDL_GetWindowPixelFormat(m_Window.get());
	}

	int FrameMan::SwitchResolutionMultiplier(unsigned char multiplier) {
		if (multiplier <= 0 || multiplier > 4 || multiplier == m_ResMultiplier)
			return -1;

		if (m_ResX > m_ScreenRes->w / multiplier || m_ResY > m_ScreenRes->h / multiplier) {
			ShowMessageBox("Requested resolution multiplier will result in game window exceeding display bounds!\nNo change will be made!");
			return -1;
		}

		// if (SDL_RenderSetScale(m_Renderer, multiplier, multiplier) != 0) {
		// 	g_ConsoleMan.PrintString("ERROR: " + std::string(SDL_GetError()));
		// 	return 1;
		// }

		// SDL_RenderGetLogicalSize(m_Renderer, &m_ResX, &m_ResY);

		g_SettingsMan.UpdateSettingsFile();

		RenderClear();
		RenderPresent();

		return 0;
	}

	void FrameMan::SwitchToFullscreen(bool upscaled, bool endActivity) {
		SetFullscreen(true);
		m_ResMultiplier = upscaled ? 2 : 1;
		SwitchResolution(m_ScreenRes->w, m_ScreenRes->h, m_ResMultiplier, endActivity);
	}

	int FrameMan::SwitchResolution(int newResX, int newResY, int newMultiplier, bool endActivity) {
		if (!IsValidResolution(newResX, newResY) || newResX <= 0 || newResX > m_ScreenRes->w || newResY > m_ScreenRes->h) {
			return -1;
		}

		if (g_ActivityMan.GetActivity()) {
			g_ActivityMan.EndActivity();
		}


		if (!m_Fullscreen) {
			SDL_SetWindowSize(m_Window.get(), newResX * newMultiplier, newResY * newMultiplier);
			SDL_GetWindowSize(m_Window.get(), &m_ResX, &m_ResY);
			m_ResX /= newMultiplier;
			m_ResY /= newMultiplier;

			m_Renderer->SetSize(glm::vec2(m_ResX, m_ResY));

			// SDL_RenderSetScale(m_Renderer, newMultiplier, newMultiplier);

			m_NewResX = m_ResX;
			m_NewResY = m_ResY;
		} else {
			m_Renderer->SetSize(glm::vec2(newResX, newResY));
			// SDL_RenderSetLogicalSize(m_Renderer, newResX, newResY);

			// SDL_RenderSetScale(m_Renderer, newMultiplier, newMultiplier);

			m_NewResX = m_ResX = newResX;
			m_NewResY = m_ResY = newResY;
		}

		m_ResMultiplier = m_NewResMultiplier = newMultiplier;

		g_SettingsMan.UpdateSettingsFile();
		RenderClear();
		RenderPresent();

		m_ResChanged = true;

		return 0;
	}

	void FrameMan::SetFullscreen(bool fullscreen) {
		if (SDL_SetWindowFullscreen(m_Window.get(), fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) == 0) {
			m_Fullscreen = fullscreen;
			if (fullscreen)
				SDL_GetDisplayBounds(SDL_GetWindowDisplayIndex(m_Window.get()), m_ScreenRes.get());
			else
				SDL_GetDisplayUsableBounds(SDL_GetWindowDisplayIndex(m_Window.get()), m_ScreenRes.get());
		} else {
			g_ConsoleMan.PrintString("ERROR: " + std::string(SDL_GetError()));
		}
	}

	int FrameMan::CalculateTextWidth(const std::string &text, bool isSmall) {
		return isSmall ? GetSmallFont()->CalculateWidth(text) : GetLargeFont()->CalculateWidth(text);
	}

	int FrameMan::CalculateTextHeight(const std::string &text, int maxWidth, bool isSmall) {
		return isSmall ? GetSmallFont()->CalculateHeight(text, maxWidth) : GetLargeFont()->CalculateHeight(text, maxWidth);
	}

	void FrameMan::SetScreenText(std::string message, int whichScreen, int blinkInterval, long displayDuration, bool centered) {
		if (whichScreen >= 0 && whichScreen < c_MaxScreenCount && m_TextDurationTimer[whichScreen].IsPastRealMS(m_TextDuration[whichScreen])) {
			m_ScreenText[whichScreen] = message;
			m_TextDuration[whichScreen] = displayDuration;
			m_TextDurationTimer[whichScreen].Reset();
			m_TextBlinking[whichScreen] = blinkInterval;
			m_TextCentered[whichScreen] = centered;
		}
	}

	void FrameMan::ClearScreenText(int whichScreen) {
		if (whichScreen >= 0 && whichScreen < c_MaxScreenCount) {
			m_ScreenText[whichScreen].clear();
			m_TextDuration[whichScreen] = -1;
			m_TextDurationTimer[whichScreen].Reset();
			m_TextBlinking[whichScreen] = 0;
		}
	}

	GUIFont *FrameMan::GetFont(bool isSmall) {
		if (!m_GUIScreen) {
			m_GUIScreen = std::make_shared<SDLScreen>();
		}

		if (isSmall) {
			if (!m_SmallFont) {
				m_SmallFont = std::make_shared<GUIFont>("SmallFont");
				m_SmallFont->Load(m_GUIScreen.get(), "Base.rte/GUIs/Skins/FontSmall.png");
			}
			return m_SmallFont.get();
		}
		if (!m_LargeFont) {
			m_LargeFont = std::make_shared<GUIFont>("FatFont");
			m_LargeFont->Load(m_GUIScreen.get(), "Base.rte/GUIs/Skins/FontLarge.png");
		}
		return m_LargeFont.get();
	}

	void FrameMan::DrawScreenText(int playerScreen, SDLGUITexture playerGUIBitmap) {
		int textPosY = 0;
		// Only draw screen text to actual human players
		if (playerScreen < g_ActivityMan.GetActivity()->GetHumanCount()) {
			textPosY += 12;

			if (!m_ScreenText[playerScreen].empty()) {
				int bufferOrScreenWidth = IsInMultiplayerMode() ? GetPlayerFrameBufferWidth(playerScreen) : GetPlayerScreenWidth();
				int bufferOrScreenHeight = IsInMultiplayerMode() ? GetPlayerFrameBufferHeight(playerScreen) : GetPlayerScreenHeight();

				if (m_TextCentered[playerScreen]) {
					textPosY = (bufferOrScreenHeight / 2) - 52;
				}

				int screenOcclusionOffsetX = g_SceneMan.GetScreenOcclusion(playerScreen).GetRoundIntX();
				// If there's really no room to offset the text into, then don't
				if (GetPlayerScreenWidth() <= GetResX() / 2) {
					screenOcclusionOffsetX = 0;
				}

				// Draw text and handle blinking by turning on and off extra surrounding characters. Text is always drawn to keep it readable.
				if (m_TextBlinking[playerScreen] && m_TextBlinkTimer.AlternateReal(m_TextBlinking[playerScreen])) {
					GetLargeFont()->DrawAligned(&playerGUIBitmap, (bufferOrScreenWidth + screenOcclusionOffsetX) / 2, textPosY, ">>> " + m_ScreenText[playerScreen] + " <<<", GUIFont::Centre);
				} else {
					GetLargeFont()->DrawAligned(&playerGUIBitmap, (bufferOrScreenWidth + screenOcclusionOffsetX) / 2, textPosY, m_ScreenText[playerScreen], GUIFont::Centre);
				}
				textPosY += 12;
			}

			// Draw info text when in MOID or material layer draw mode
			switch (g_SceneMan.GetLayerDrawMode()) {
				case g_LayerTerrainMatter:
					GetSmallFont()->DrawAligned(&playerGUIBitmap, GetPlayerScreenWidth() / 2, GetPlayerScreenHeight() - 12, "Viewing terrain material layer\nHit Ctrl+M to cycle modes", GUIFont::Centre, GUIFont::Bottom);
					break;
				case g_LayerMOID:
					GetSmallFont()->DrawAligned(&playerGUIBitmap, GetPlayerScreenWidth() / 2, GetPlayerScreenHeight() - 12, "Viewing MovableObject ID layer\nHit Ctrl+M to cycle modes", GUIFont::Centre, GUIFont::Bottom);
					break;
				default:
					break;
			}
			// g_PerformanceMan.Draw(playerGUIBitmap);

		} else {
			// If superfluous screen (as in a three-player match), make the fourth the Observer one
			GetLargeFont()->DrawAligned(&playerGUIBitmap, GetPlayerScreenWidth() / 2, textPosY, "- Observer View -", GUIFont::Centre);
		}
	}

	void FrameMan::FlashScreen(int screen, uint32_t color, float periodMS) {
		m_FlashScreenColor[screen] = color;
		m_FlashTimer[screen].SetRealTimeLimitMS(periodMS);
		m_FlashTimer[screen].Reset();
	}
} // namespace RTE
