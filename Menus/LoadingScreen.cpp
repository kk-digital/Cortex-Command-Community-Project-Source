#include "LoadingScreen.h"
#include "FrameMan.h"
#include "SceneLayer.h"
#include "Writer.h"

#include "GUI.h"
#include "SDLScreen.h"
#include "SDLGUITexture.h"
#include "SDLInput.h"
#include "GUICollectionBox.h"
#include "GUIListBox.h"

#include "System/System.h"
#include "Renderer/RenderTexture.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingScreen::Clear() {
		m_LoadingLogWriter = nullptr;
		m_ProgressListboxBitmap = nullptr;
		m_ProgressListboxPosX = 0;
		m_ProgressListboxPosY = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingScreen::Create(SDLScreen *guiScreen, SDLInput *guiInput, bool progressReportDisabled) {
		// GUIControlManager loadingScreenManager;
		// RTEAssert(loadingScreenManager.Create(guiScreen, guiInput, "Base.rte/GUIs/Skins/Menus", "LoadingScreenSkin.ini"), "Failed to create GUI Control Manager and load it from Base.rte/GUIs/Skins/Menus/LoadingScreenSkin.ini");
		// loadingScreenManager.Load("Base.rte/GUIs/LoadingGUI.ini");

		int loadingSplashOffset = 0;
		if (!progressReportDisabled) {
			// CreateProgressReportListbox(&loadingScreenManager);
			loadingSplashOffset = m_ProgressListboxPosX / 4;
		}
		SceneLayer loadingSplash;
		loadingSplash.Create(ContentFile("Base.rte/GUIs/Title/LoadingSplash.png"), false, Vector(), true, false, Vector(1.0F, 0));
		loadingSplash.SetOffset(Vector(static_cast<float>(((loadingSplash.GetTexture()->GetW() - g_FrameMan.GetResX()) / 2) + loadingSplashOffset), 0));

		Box loadingSplashTargetBox(Vector(0, static_cast<float>((g_FrameMan.GetResY() - loadingSplash.GetTexture()->GetH()) / 2)), static_cast<float>(g_FrameMan.GetResX()), static_cast<float>(loadingSplash.GetTexture()->GetH()));
		g_FrameMan.RenderClear();
		loadingSplash.Draw(g_FrameMan.GetRenderer(), loadingSplashTargetBox);

#ifdef LOADINGUI
		if (!progressReportDisabled) { m_ProgressListboxBitmap->render(g_FrameMan.GetRenderer(), m_ProgressListboxPosX, m_ProgressListboxPosY); }
#endif
		g_FrameMan.RenderPresent();

		if (!m_LoadingLogWriter) {
			m_LoadingLogWriter = std::make_unique<Writer>("LogLoading.txt");
			if (!m_LoadingLogWriter->WriterOK()) {
				ShowMessageBox("Failed to instantiate the Loading Log writer!\nModule loading will proceed without being logged!");
				m_LoadingLogWriter.reset();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingScreen::CreateProgressReportListbox(GUIControlManager *parentControlManager) {
		#ifdef LOADINGGUI
		dynamic_cast<GUICollectionBox *>(parentControlManager->GetControl("root"))->SetSize(g_FrameMan.GetResX(), g_FrameMan.GetResY());
		GUIListBox *listBox = dynamic_cast<GUIListBox *>(parentControlManager->GetControl("ProgressBox"));

		// Make the box a bit bigger if there's room in higher, HD resolutions.
		if (g_FrameMan.GetResX() >= c_DefaultResX) { listBox->Resize((g_FrameMan.GetResX() / 3) - 12, listBox->GetHeight()); }

		listBox->SetPositionRel(g_FrameMan.GetResX() - listBox->GetWidth() - 12, (g_FrameMan.GetResY() / 2) - (listBox->GetHeight() / 2));
		listBox->ClearList();

		if (!m_ProgressListboxBitmap) {
			listBox->SetVisible(false);
			m_ProgressListboxBitmap = std::make_shared<Texture>(g_FrameMan.GetRenderer(), listBox->GetWidth(), listBox->GetHeight());
			clear_to_color(m_ProgressListboxBitmap, 54);
			rect(m_ProgressListboxBitmap, 0, 0, listBox->GetWidth() - 1, listBox->GetHeight() - 1, 33);
			rect(m_ProgressListboxBitmap, 1, 1, listBox->GetWidth() - 2, listBox->GetHeight() - 2, 33);
			m_ProgressListboxPosX = listBox->GetXPos();
			m_ProgressListboxPosY = listBox->GetYPos();
		}
		#endif
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingScreen::Destroy() {
		Clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingScreen::LoadingSplashProgressReport(const std::string &reportString, bool newItem) {
		if (System::IsLoggingToCLI()) { System::PrintLoadingToCLI(reportString, newItem); }

#ifdef LOADINGGUI
		if (newItem) {
			// Write out the last line to the log file before starting a new one and scroll the bitmap upwards.
			if (g_LoadingScreen.m_LoadingLogWriter) { g_LoadingScreen.m_LoadingLogWriter->NewLineString(reportString, false); }
			if (g_LoadingScreen.m_ProgressListboxBitmap) { blit(g_LoadingScreen.m_ProgressListboxBitmap, g_LoadingScreen.m_ProgressListboxBitmap, 2, 12, 2, 2, g_LoadingScreen.m_ProgressListboxBitmap->w - 3, g_LoadingScreen.m_ProgressListboxBitmap->h - 12); }
		}

		if (g_LoadingScreen.m_ProgressListboxBitmap) {
			AllegroBitmap drawBitmap(g_LoadingScreen.m_ProgressListboxBitmap);

			// Clear current line.
			rectfill(g_LoadingScreen.m_ProgressListboxBitmap, 2, g_LoadingScreen.m_ProgressListboxBitmap->h - 12, g_LoadingScreen.m_ProgressListboxBitmap->w - 3, g_LoadingScreen.m_ProgressListboxBitmap->h - 3, 54);
			// Print new line.
			g_FrameMan.GetSmallFont()->DrawAligned(&drawBitmap, 5, g_LoadingScreen.m_ProgressListboxBitmap->h - 12, reportString, GUIFont::Left);
			// Lines are drawing over the right edge of the box and we can't cap the width from DrawAligned here so redraw the right edge.
			vline(g_LoadingScreen.m_ProgressListboxBitmap, g_LoadingScreen.m_ProgressListboxBitmap->w - 2, g_LoadingScreen.m_ProgressListboxBitmap->h - 12, g_LoadingScreen.m_ProgressListboxBitmap->h - 2, 33);
			vline(g_LoadingScreen.m_ProgressListboxBitmap, g_LoadingScreen.m_ProgressListboxBitmap->w - 1, g_LoadingScreen.m_ProgressListboxBitmap->h - 12, g_LoadingScreen.m_ProgressListboxBitmap->h - 2, 33);

			blit(g_LoadingScreen.m_ProgressListboxBitmap, g_FrameMan.GetBackBuffer32(), 0, 0, g_LoadingScreen.m_ProgressListboxPosX, g_LoadingScreen.m_ProgressListboxPosY, g_LoadingScreen.m_ProgressListboxBitmap->w, g_LoadingScreen.m_ProgressListboxBitmap->h);

			g_FrameMan.FlipFrameBuffers();
		}
#endif
	}
}
