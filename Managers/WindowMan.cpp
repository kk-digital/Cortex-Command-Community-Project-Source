#include "WindowMan.h"
#include "SettingsMan.h"
#include "FrameMan.h"
#include "UInputMan.h"
#include "ConsoleMan.h"

#include "SDL.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_query.hpp"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::Clear() {
		m_Window.reset();
		m_MultiWindows.clear();
		m_Renderer.reset();
		m_MultiRenderers.clear();
		m_ScreenTexture.reset();
		m_MultiDisplayTextures.clear();
		m_TextureOffsets.clear();

		m_NumScreens = SDL_GetNumVideoDisplays();
		m_MaxResX = 0;
		m_MaxResY = 0;
		m_PrimaryScreenResX = 0;
		m_PrimaryScreenResY = 0;
		m_ResX = c_DefaultResX;
		m_ResY = c_DefaultResY;
		m_ResMultiplier = 1;

		m_EnableVSync = false;
		m_Fullscreen = false;

		m_ResChanged = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	WindowMan::WindowMan() {
		Clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::Initialize() {
		m_NumScreens = SDL_GetNumVideoDisplays();

		SDL_Rect primaryDisplayBounds;
		SDL_GetDisplayBounds(0, &primaryDisplayBounds);

		m_PrimaryScreenResX = primaryDisplayBounds.w;
		m_PrimaryScreenResY = primaryDisplayBounds.h;

		for (int i = 0; i < m_NumScreens; ++i) {
			SDL_Rect res;
			if (SDL_GetDisplayBounds(i, &res) != 0) {
				g_ConsoleMan.PrintString("ERROR: Failed to get resolution of display " + std::to_string(i));
				continue;
			}

			if (res.x + res.w > m_MaxResX) {
				m_MaxResX = res.x + res.w;
			}
			if (res.y + res.h > m_MaxResY) {
				m_MaxResY = res.y + res.h;
			}
		}

		ValidateResolution(m_ResX, m_ResY, m_ResMultiplier, m_Fullscreen);

		const char *windowTitle = "Cortex Command Community Project";

		m_Window = std::unique_ptr<SDL_Window, SDLWindowDeleter>(SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier, SDL_WINDOW_SHOWN));
		if (!m_Window) {
			ShowMessageBox("Unable to create window because: " + std::string(SDL_GetError()) + "!\n\nTrying to revert to defaults!");

			m_ResX = c_DefaultResX;
			m_ResY = c_DefaultResY;
			m_ResMultiplier = 1;
			m_Fullscreen = false;

			m_Window = std::unique_ptr<SDL_Window, SDLWindowDeleter>(SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier, SDL_WINDOW_SHOWN));
			if (!m_Window) {
				RTEAbort("Failed to create a window because: " + std::string(SDL_GetError()));
			}
		}

		if (m_Fullscreen) {
			if (m_NumScreens == 1) {
				SDL_SetWindowFullscreen(m_Window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);
			} else {
				if (!SetWindowMultiFullscreen(m_ResX, m_ResY, m_ResMultiplier)) {
					SDL_SetWindowFullscreen(m_Window.get(), SDL_FALSE);
					m_ResX = c_DefaultResX;
					m_ResY = c_DefaultResY;
					m_ResMultiplier = 1;
					m_Fullscreen = false;
					SDL_SetWindowSize(m_Window.get(), m_ResX, m_ResY);
					SDL_SetWindowPosition(m_Window.get(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED);
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::CreateRenderers(int backbufferWidth, int backbufferHeight) {
		int renderFlags = SDL_RENDERER_ACCELERATED;
		if (g_WindowMan.VSyncEnabled()) {
			renderFlags |= SDL_RENDERER_PRESENTVSYNC;
		}

		m_Renderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(SDL_CreateRenderer(g_WindowMan.GetWindow(), -1, renderFlags));
		if (!m_Renderer) {
			m_Renderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>(SDL_CreateRenderer(g_WindowMan.GetWindow(), -1, SDL_RENDERER_SOFTWARE));
		}
		RTEAssert(m_Renderer.get(), "Failed to initialize renderer, are you sure this is a computer?");
		SDL_RenderSetIntegerScale(m_Renderer.get(), SDL_TRUE);

		if (m_TextureOffsets.empty()) {
			m_ScreenTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(SDL_CreateTexture(m_Renderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, backbufferWidth, backbufferHeight));
			SDL_RenderSetLogicalSize(m_Renderer.get(), backbufferWidth, backbufferHeight);
		} else {
			m_MultiDisplayTextures.resize(m_TextureOffsets.size());
			for (size_t i = 0; i < m_MultiDisplayTextures.size(); ++i) {
				SDL_Renderer* renderer;
				if (i == 0) {
					renderer = m_Renderer.get();
				} else {
					renderer = m_MultiRenderers[i - 1].get();
				}
				m_MultiDisplayTextures[i] = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(
					SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_TextureOffsets[i].w, m_TextureOffsets[i].h));
				if (!m_MultiDisplayTextures[i]) {
					RTEAbort("Failed to create texture for multi-display: " + std::string(SDL_GetError()));
				}
				SDL_RenderSetLogicalSize(renderer, m_TextureOffsets[i].w, m_TextureOffsets[i].h);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ValidateResolution(int &resX, int &resY, int &resMultiplier, bool &newFullscreen) const {
		bool settingsNeedOverwrite = false;

		if (resX * resMultiplier > m_MaxResX || resY * resMultiplier > m_MaxResY) {
			settingsNeedOverwrite = true;
			resX = m_MaxResX / resMultiplier;
			resY = m_MaxResY / resMultiplier;
			newFullscreen = false;
			ShowMessageBox("Resolution too high to fit display, overriding to fit!");

		}
		if (m_NumScreens == 1) {
			float currentAspectRatio = static_cast<float>(resX) / static_cast<float>(resY);
			if (currentAspectRatio < 1 || currentAspectRatio > 4) {
				settingsNeedOverwrite = true;
				resX = c_DefaultResX;
				resY = c_DefaultResY;
				resMultiplier = 1;
				newFullscreen = false;
				ShowMessageBox("Abnormal aspect ratio detected! Reverting to defaults!");
			}
		}
		if (settingsNeedOverwrite) { g_SettingsMan.SetSettingsNeedOverwrite(); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::DisplaySwitchOut() const {
		g_UInputMan.DisableMouseMoving(true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::DisplaySwitchIn() const {
		g_UInputMan.DisableMouseMoving(false);
		if (!m_MultiWindows.empty()) {
			SDL_RaiseWindow(m_Window.get());
			for (auto &window : m_MultiWindows) {
				SDL_RaiseWindow(window.get());
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ChangeResolution(int newResX, int newResY, bool upscaled, bool newFullscreen) {
		int newResMultiplier = upscaled ? 2 : 1;

		if (m_ResX == newResX && m_ResY == newResY && m_ResMultiplier == newResMultiplier && m_Fullscreen == newFullscreen) {
			return;
		}

		m_MultiRenderers.clear();
		m_MultiDisplayTextures.clear();
		m_TextureOffsets.clear();
		m_MultiWindows.clear();

		if (!newFullscreen) {
			SDL_RestoreWindow(m_Window.get());
		}

		ValidateResolution(newResX, newResY, newResMultiplier, newFullscreen);

		if (newFullscreen &&
			((m_NumScreens > 1 && !SetWindowMultiFullscreen(newResX, newResY, newResMultiplier)) ||
			SDL_SetWindowFullscreen(m_Window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)) {

			SDL_SetWindowSize(m_Window.get(), m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier);
			if (SDL_SetWindowFullscreen(m_Window.get(), m_Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0) {
				RTEAbort("Unable to set back to previous resolution because: " + std::string(SDL_GetError()) + "!");
			}
			g_ConsoleMan.PrintString("ERROR: Failed to switch to new resolution, reverted back to previous setting!");
			//set_palette(m_Palette);
			return;
		} else if (!newFullscreen) {
			SDL_SetWindowFullscreen(m_Window.get(), 0);
			SDL_RestoreWindow(m_Window.get());
			SDL_SetWindowBordered(m_Window.get(), SDL_TRUE);
			SDL_SetWindowSize(m_Window.get(), newResX * newResMultiplier, newResY * newResMultiplier);
			int displayIndex = SDL_GetWindowDisplayIndex(m_Window.get());
			SDL_SetWindowPosition(m_Window.get(), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex));
		}
		m_Fullscreen = newFullscreen;
		m_ResX = newResX;
		m_ResY = newResY;
		m_ResMultiplier = newResMultiplier;

		//set_palette(m_Palette);
		//RecreateBackBuffers();

		g_ConsoleMan.PrintString("SYSTEM: Switched to different resolution.");
		g_SettingsMan.UpdateSettingsFile();

		m_ResChanged = true;
		ClearFrame();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ChangeResolutionMultiplier(int newMultiplier) {
		if (newMultiplier <= 0 || newMultiplier > 4 || newMultiplier == m_ResMultiplier) {
			return;
		}

		if (m_ResX > m_MaxResX / newMultiplier || m_ResY > m_MaxResY / newMultiplier) {
			ShowMessageBox("Requested resolution multiplier will result in game window exceeding display bounds!\nNo change will be made!\n\nNOTE: To toggle fullscreen, use the button in the Options & Controls Menu!");
			return;
		}

		m_MultiRenderers.clear();
		m_MultiDisplayTextures.clear();
		m_TextureOffsets.clear();
		m_MultiWindows.clear();

		m_Fullscreen = (m_ResX * newMultiplier == m_MaxResX && m_ResY * newMultiplier == m_MaxResY);

		if (m_Fullscreen) {
			if (m_NumScreens > 1) {
				if (!SetWindowMultiFullscreen(m_ResX, m_ResY, newMultiplier)) {
					m_Fullscreen = (m_ResX * m_ResMultiplier == m_MaxResX && m_ResY * m_ResMultiplier == m_MaxResY);
					SDL_SetWindowSize(m_Window.get(), m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier);
					if (SDL_SetWindowFullscreen(m_Window.get(), m_Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0) {
						RTEAbort("Unable to set back to previous windowed mode multiplier because: " + std::string(SDL_GetError()) + "!");
					}
					g_ConsoleMan.PrintString("ERROR: Failed to switch to new windowed mode multiplier, reverted back to previous setting!");
					//set_palette(m_Palette);
					return;
				}
			} else if (SDL_SetWindowFullscreen(m_Window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP) != 0) {
				m_Fullscreen = (m_ResX * m_ResMultiplier == m_MaxResX && m_ResY * m_ResMultiplier == m_MaxResY);
				SDL_SetWindowSize(m_Window.get(), m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier);
				if (SDL_SetWindowFullscreen(m_Window.get(), m_Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0) {
					RTEAbort("Unable to set back to previous windowed mode multiplier because: " + std::string(SDL_GetError()) + "!");
				}
				g_ConsoleMan.PrintString("ERROR: Failed to switch to new windowed mode multiplier, reverted back to previous setting!");
				//set_palette(m_Palette);
				return;
			}
		} else {
			SDL_SetWindowFullscreen(m_Window.get(), 0);
			SDL_RestoreWindow(m_Window.get());
			SDL_SetWindowBordered(m_Window.get(), SDL_TRUE);
			SDL_SetWindowSize(m_Window.get(), m_ResX * newMultiplier, m_ResY * newMultiplier);
			int displayIndex = SDL_GetWindowDisplayIndex(m_Window.get());
			SDL_SetWindowPosition(m_Window.get(), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex));
		}
		m_ResMultiplier = newMultiplier;

		//set_palette(m_Palette);
		//RecreateBackBuffers();

		g_ConsoleMan.PrintString("SYSTEM: Switched to different windowed mode multiplier.");
		g_SettingsMan.UpdateSettingsFile();

		ClearFrame();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glm::vec4 WindowMan::GetViewportLetterbox(int resX, int resY, int windowW, int windowH) {
		float aspectRatio = resX / static_cast<float>(resY);
		int width = windowW;
		int height = width / aspectRatio + 0.5f;

		if (height > windowH) {
			height = windowH;
			width = height * aspectRatio + 0.5f;
		}

		int offsetX = (windowW / 2) - (width / 2);
		int offsetY = (windowH / 2) - (height / 2);
		return glm::vec4(offsetX, windowH - offsetY - height, width, height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool WindowMan::SetWindowMultiFullscreen(int &resX, int &resY, int resMultiplier) {
		m_MultiRenderers.clear();
		m_MultiDisplayTextures.clear();
		m_TextureOffsets.clear();
		m_MultiWindows.clear();

		int windowW = resX * resMultiplier;
		int windowH = resY * resMultiplier;

		int windowDisplay = SDL_GetWindowDisplayIndex(m_Window.get());

		SDL_Rect windowDisplayBounds;
		SDL_GetDisplayBounds(windowDisplay, &windowDisplayBounds);
		std::vector<std::pair<int, SDL_Rect>> displayBounds(m_NumScreens);
		for (int i = 0; i < m_NumScreens; ++i) {
			displayBounds[i].first = i;
			SDL_GetDisplayBounds(i, &displayBounds[i].second);
		}
		std::stable_sort(displayBounds.begin(), displayBounds.end(), [](auto left, auto right) { return left.second.x < right.second.x; });
		std::stable_sort(displayBounds.begin(), displayBounds.end(), [](auto left, auto right) { return left.second.y < right.second.y; });
		std::vector<std::pair<int, SDL_Rect>>::iterator displayPos = std::find_if(displayBounds.begin(), displayBounds.end(), [windowDisplay](auto display) {return display.first == windowDisplay; });

		int index = displayPos - displayBounds.begin();

		int actualResX = 0;
		int actualResY = 0;
		int topLeftX = windowDisplayBounds.x;
		int topLeftY = windowDisplayBounds.y;


		for (; index < m_NumScreens && (actualResY < resY * resMultiplier || actualResX < resX * resMultiplier); ++index) {
			if (displayBounds[index].second.x < topLeftX || displayBounds[index].second.y < topLeftY ||
				displayBounds[index].second.x - topLeftX > resX * resMultiplier || displayBounds[index].second.y - topLeftY > resY * resMultiplier) {
				continue;
			}
			if (actualResX < displayBounds[index].second.x - topLeftX + displayBounds[index].second.w) {
				actualResX = displayBounds[index].second.x - topLeftX + displayBounds[index].second.w;
			}
			if (actualResY < displayBounds[index].second.y - topLeftY + displayBounds[index].second.h) {
				actualResY = displayBounds[index].second.y - topLeftY + displayBounds[index].second.h;
			}
			if (index != displayPos - displayBounds.begin()) {
				m_MultiWindows.emplace_back(SDL_CreateWindow("",
					displayBounds[index].second.x,
					displayBounds[index].second.y,
					displayBounds[index].second.w,
					displayBounds[index].second.h,
					SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SKIP_TASKBAR));
				if (!m_MultiWindows.back()) {
					actualResX = -1;
					actualResY = -1;
					break;
				}
				m_MultiRenderers.emplace_back(SDL_CreateRenderer(
					m_MultiWindows.back().get(),
					-1,
					SDL_RENDERER_ACCELERATED));
				if (!m_MultiRenderers.back()) {
					actualResX = -1;
					actualResY = -1;
					break;
				}
			}
			m_TextureOffsets.emplace_back(SDL_Rect{
				(displayBounds[index].second.x - topLeftX) / resMultiplier,
				(displayBounds[index].second.y - topLeftY) / resMultiplier,
				displayBounds[index].second.w / resMultiplier,
				displayBounds[index].second.h / resMultiplier });
		}
		if (actualResX < resX * resMultiplier || actualResY < resY * resMultiplier) {
			int maxResX = displayBounds.back().second.x - topLeftX + displayBounds.back().second.w;
			int maxResY = displayBounds.back().second.y - topLeftY + displayBounds.back().second.h;
			ShowMessageBox("Won't be able to fit the desired resolution onto the displays. Maximum resolution from here is: " + std::to_string(maxResX) + "x" + std::to_string(maxResY) + "\n Please move the window to the display you want to be the top left corner and try again.");
		}
		if (actualResX == -1 || actualResY == -1) {
			m_MultiWindows.clear();
			m_MultiRenderers.clear();
			m_MultiDisplayTextures.clear();
			m_TextureOffsets.clear();
			return false;
		}
		//CBA to do figure out letterboxing for multiple displays, so just fix the resolution.
		if (actualResX != resX * resMultiplier || actualResY != resY * resMultiplier) {
			ShowMessageBox("Desired reolution would lead to letterboxing, adjusting to fill entire displays.");
			resX = actualResX / resMultiplier;
			resY = actualResY / resMultiplier;
		}

		SDL_SetWindowFullscreen(m_Window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ClearFrame() {
		SDL_RenderClear(m_Renderer.get());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::UploadFrame() {
		//if (m_WantScreenDump) {
		//	SaveBitmap(ScreenDump, m_ScreenDumpName);
		//	m_WantScreenDump = false;
		//}

		if (m_TextureOffsets.empty()) {
			SDL_UpdateTexture(m_ScreenTexture.get(), NULL, g_FrameMan.GetBackBuffer32()->line[0], g_FrameMan.GetBackBuffer32()->w * 4);

			SDL_RenderCopy(m_Renderer.get(), m_ScreenTexture.get(), NULL, NULL);
			SDL_RenderPresent(m_Renderer.get());
		} else {
			SDL_Renderer *renderer;
			for (size_t i = 0; i < m_TextureOffsets.size(); ++i) {
				if (i == 0) {
					renderer = m_Renderer.get();
				} else {
					renderer = m_MultiRenderers[i - 1].get();
				}
				SDL_UpdateTexture(m_MultiDisplayTextures[i].get(), NULL, g_FrameMan.GetBackBuffer32()->line[0] + (m_TextureOffsets[i].x + g_FrameMan.GetBackBuffer32()->w * m_TextureOffsets[i].y) * 4, g_FrameMan.GetBackBuffer32()->w * 4);

				SDL_RenderCopy(renderer, m_MultiDisplayTextures[i].get(), NULL, NULL);
				SDL_RenderPresent(renderer);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SDLWindowDeleter::operator()(SDL_Window *window) const {
		SDL_DestroyWindow(window);
	}

	void SDLRendererDeleter::operator()(SDL_Renderer *renderer) const {
		SDL_DestroyRenderer(renderer);
	}

	void SDLTextureDeleter::operator()(SDL_Texture *texture) const {
		SDL_DestroyTexture(texture);
	}
}