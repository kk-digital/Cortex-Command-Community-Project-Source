#include "FrameMan.h"

#include "System/SDLHelper.h"

#include "ContentFile.h"
#include "Timer.h"
#include "Box.h"
#include "PostProcessMan.h"
#include "PrimitiveMan.h"
#include "PerformanceMan.h"
#include "SceneMan.h"
// #include "ActivityMan.h"
// #include "ConsoleMan.h"
// #include "SettingsMan.h"
// #include "UInputMan.h"

#include "Entities/SLTerrain.h"
#include "Entities/Scene.h"

#include "GUI/GUI.h"
#include "GUI/SDLGUITexture.h"
//#include "GUI/SDLScreen.h"

#include "System/Constants.h"
#include "System/RTEError.h"

namespace RTE {

	FrameMan::FrameMan(){Clear();}

	void FrameMan::Clear() {
		m_Window = nullptr;
		m_Renderer = nullptr;

		m_NumScreens = SDL_GetNumVideoDisplays();
		SDL_GetDisplayBounds(0, m_Resolution.get());

		m_ScreenResX = m_Resolution->w;
		m_ScreenResY = m_Resolution->h;

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
		m_HSplitOverride = false;
		m_VSplitOverride = false;
	}

	int FrameMan::Initialize() {
		ValidateResolution(m_ResX, m_ResY, m_ResMultiplier);
		m_Window = SDL_CreateWindow(c_WindowTitle, SDL_WINDOWPOS_CENTERED,
		                            SDL_WINDOWPOS_CENTERED, m_ResX, m_ResY,
		                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |
		                                SDL_WINDOW_INPUT_FOCUS);

		SetFullscreen(m_Fullscreen);

		RTEAssert(m_Window != NULL, "Could not create Window because: " +
		                                std::string(SDL_GetError()));

		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
		RTEAssert(m_Renderer != NULL, "Could not create Renderer because: " +
		                                  std::string(SDL_GetError()));

		// Set integer scaling so we don't get artifacts by rendering subpixels.
		SDL_RenderSetIntegerScale(m_Renderer, SDL_TRUE);

		// Upscale the resolution to the set multiplier.
		SDL_RenderSetScale(m_Renderer, m_ResMultiplier, m_ResMultiplier);

		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_Renderer);

		return 0;
	}

	void FrameMan::Destroy() {
		if(m_Renderer)
			SDL_DestroyRenderer(m_Renderer);
		if(m_Window)
			SDL_DestroyWindow(m_Window);

		Clear();
	}

	void FrameMan::Update() {
		// g_PerformanceMan.Update();

		// g_PrimitiveMan.ClearPrimitivesList();
	}

	void FrameMan::Draw() {
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
			if (!g_SceneMan.SceneWrapsX() &&
			    drawScreen->w > g_SceneMan.GetSceneWidth()) {
				targetPos.m_X +=
				    (drawScreen->w - g_SceneMan.GetSceneWidth()) / 2;
			}
			if (!g_SceneMan.SceneWrapsY() &&
			    drawScreen->h > g_SceneMan.GetSceneHeight()) {
				targetPos.m_Y +=
				    (drawScreen->h - g_SceneMan.GetSceneHeight()) / 2;
			}

			// Try to move at the frame buffer copy time to maybe prevent
			// wonkyness
			m_TargetPos[m_NetworkFrameCurrent][playerScreen] = targetPos;

			// Draw the scene
			if (!IsInMultiplayerMode()) {
				g_SceneMan.Draw(drawScreen, drawScreenGUI, targetPos);
			} else {
				clear_to_color(drawScreen, g_MaskColor);
				clear_to_color(drawScreenGUI, g_MaskColor);
				g_SceneMan.Draw(drawScreen, drawScreenGUI, targetPos, true,
				                true);
			}

			// Get only the scene-relative post effects that affect this
			// player's screen
			if (pActivity) {
				g_PostProcessMan.GetPostScreenEffectsWrapped(
				    targetPos, drawScreen->w, drawScreen->h,
				    screenRelativeEffects,
				    pActivity->GetTeamOfPlayer(
				        pActivity->PlayerOfScreen(playerScreen)));
				g_PostProcessMan.GetGlowAreasWrapped(targetPos, drawScreen->w,
				                                     drawScreen->h,
				                                     screenRelativeGlowBoxes);

				if (IsInMultiplayerMode()) {
					g_PostProcessMan.SetNetworkPostEffectsList(
					    playerScreen, screenRelativeEffects);
				}
			}

			// TODO: Find out what keeps disabling the clipping on the draw
			// bitmap Enable clipping on the draw bitmap
			set_clip_state(drawScreen, 1);

			DrawScreenText(playerScreen, playerGUIBitmap);

			// The position of the current draw screen on the backbuffer
			Vector screenOffset;

			// If we are dealing with split screens, then deal with the fact
			// that we need to draw the player screens to different locations on
			// the final buffer
			if (screenCount > 1) {
				UpdateScreenOffsetForSplitScreen(playerScreen, screenOffset);
			}

			DrawScreenFlash(playerScreen, drawScreenGUI);

			if (!IsInMultiplayerMode()) {
				// Draw the intermediate draw splitscreen to the appropriate
				// spot on the back buffer
				blit(drawScreen, m_BackBuffer8, 0, 0,
				     screenOffset.GetFloorIntX(), screenOffset.GetFloorIntY(),
				     drawScreen->w, drawScreen->h);

				g_PostProcessMan.AdjustEffectsPosToPlayerScreen(
				    playerScreen, drawScreen, screenOffset,
				    screenRelativeEffects, screenRelativeGlowBoxes);
			}
		}

		// Clears the pixels that have been revealed from the unseen layers
		g_SceneMan.ClearSeenPixels();

		if (!IsInMultiplayerMode()) {
			// Draw separating lines for split-screens
			if (m_HSplit) {
				hline(m_BackBuffer8, 0, (m_BackBuffer8->h / 2) - 1,
				      m_BackBuffer8->w - 1, m_AlmostBlackColor);
				hline(m_BackBuffer8, 0, (m_BackBuffer8->h / 2),
				      m_BackBuffer8->w - 1, m_AlmostBlackColor);
			}
			if (m_VSplit) {
				vline(m_BackBuffer8, (m_BackBuffer8->w / 2) - 1, 0,
				      m_BackBuffer8->h - 1, m_AlmostBlackColor);
				vline(m_BackBuffer8, (m_BackBuffer8->w / 2), 0,
				      m_BackBuffer8->h - 1, m_AlmostBlackColor);
			}

			/*
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
				}*/
		}

		if (IsInMultiplayerMode()) {
			PrepareFrameForNetwork();
		}

		if (g_InActivity) {
			g_PostProcessMan.PostProcess();
		}

		// Draw the console on top of everything
		g_ConsoleMan.Draw(m_BackBuffer32);

#ifdef DEBUG_BUILD
		// Draw scene seam
		vline(m_BackBuffer8, 0, 0, g_SceneMan.GetSceneHeight(), 5);
#endif

		// Reset the frame timer so we can measure how much it takes until next
		// frame being drawn
		g_PerformanceMan.ResetFrameTimer();
	}

	void FrameMan::SetFullscreen(bool fullscreen, bool endActivity) {
		SDL_SetWindowFullscreen(m_Window,
		                        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	}

} // namespace RTE
