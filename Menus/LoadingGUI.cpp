#include "LoadingGUI.h"
#include "Writer.h"
#include "SceneLayer.h"
#include "SettingsMan.h"
#include "PresetMan.h"
#include "FrameMan.h"
#include "UInputMan.h"

#include "GUI/GUI.h"
#include "GUI/GUICollectionBox.h"
#include "GUI/GUIListBox.h"
#include "GUI/SDLScreen.h"
#include "GUI/SDLGUITexture.h"
#include "GUI/SDLInput.h"

#include "System/System.h"
#include "System/SDLHelper.h"

namespace RTE {

	std::unique_ptr<SceneLayer> LoadingGUI::s_LoadingSplash;
	std::unique_ptr<Box> LoadingGUI::s_SplashBox;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LoadingGUI::LoadingGUI() { Clear(); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingGUI::Clear() {
		m_ControlManager = nullptr;
		m_GUIInput = nullptr;
		m_GUIScreen = nullptr;
		m_LoadingLogWriter = nullptr;
		m_ProgressListboxBitmap = nullptr;
		m_PosX = 0;
		m_PosY = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingGUI::InitLoadingScreen() {
		// g_FrameMan.LoadPalette("Base.rte/palette.bmp");

		m_GUIInput = std::make_unique<SDLInput>(-1);
		m_GUIScreen = std::make_unique<SDLScreen>();

		if (!m_ControlManager) {
			m_ControlManager = std::make_unique<GUIControlManager>();
			if (!m_ControlManager->Create(m_GUIScreen.get(), m_GUIInput.get(), "Base.rte/GUIs/Skins/MainMenu", "LoadingSkin.ini")) {
				RTEAbort("Failed to create GUI Control Manager and load it from Base.rte/GUIs/Skins/MainMenu/LoadingSkin.ini");
			}
			m_ControlManager->Load("Base.rte/GUIs/LoadingGUI.ini");
		}
		if (!g_SettingsMan.DisableLoadingScreen()) { CreateProgressReportListbox(); }

		s_LoadingSplash = std::make_unique<SceneLayer>();
		s_LoadingSplash->Create(ContentFile("Base.rte/GUIs/Title/LoadingSplash.png"), false, Vector(), true, false, Vector(1.0F, 0));

		// Hardcoded offset to make room for the loading box only if DisableLoadingScreen is false.
		int loadingSplashOffset = g_SettingsMan.DisableLoadingScreen() ? 14 : 120;
		s_LoadingSplash->SetOffset(Vector(static_cast<float>(((s_LoadingSplash->GetTexture()->getW() - g_FrameMan.GetResX()) / 2) + loadingSplashOffset), 0));

		g_FrameMan.RenderClear();
		// Draw onto wrapped strip centered vertically on the screen
		s_SplashBox = std::make_unique<Box>(Vector(0, static_cast<float>((g_FrameMan.GetResY() - s_LoadingSplash->GetTexture()->getW()) / 2)), static_cast<float>(g_FrameMan.GetResX()), static_cast<float>(s_LoadingSplash->GetTexture()->getH()));
		s_LoadingSplash->Draw(g_FrameMan.GetRenderer(), *s_SplashBox);

		g_FrameMan.RenderPresent();

		std::cout << "Loading Screen should now be visible" << std::endl;

		// Overwrite Settings.ini after all the managers are created to fully populate the file. Up until this moment Settings.ini is populated only with minimal required properties to run.
		// When the overwrite happens there is a short delay which causes the screen to remain black, so this is done here after the flip to mask that black screen.
		if (g_SettingsMan.SettingsNeedOverwrite()) { g_SettingsMan.UpdateSettingsFile(); }

		if (!m_LoadingLogWriter) {
			m_LoadingLogWriter.reset(new Writer("LogLoading.txt"));
			if (!m_LoadingLogWriter->WriterOK()) {
				ShowMessageBox("Failed to instantiate the Loading Log writer!\nModule loading will proceed without being logged!");
				m_LoadingLogWriter.reset();
			}
		}

		g_PresetMan.LoadAllDataModules();

		Destroy();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingGUI::CreateProgressReportListbox() {
		// Place and clear the sectionProgress box
		dynamic_cast<GUICollectionBox *>(m_ControlManager->GetControl("root"))->SetSize(g_FrameMan.GetResX(), g_FrameMan.GetResY());
		GUIListBox *listBox = dynamic_cast<GUIListBox *>(m_ControlManager->GetControl("ProgressBox"));

		// Make the box a bit bigger if there's room in higher, HD resolutions
		if (g_FrameMan.GetResX() >= 960) { listBox->Resize((g_FrameMan.GetResX() / 3) - 12, listBox->GetHeight()); }

		// Make the loading progress box fill the right third of the screen
		listBox->SetPositionRel(g_FrameMan.GetResX() - listBox->GetWidth() - 12, (g_FrameMan.GetResY() / 2) - (listBox->GetHeight() / 2));
		listBox->ClearList();

		if (!m_ProgressListboxBitmap) {
			listBox->SetVisible(false);
			m_ProgressListboxBitmap = std::make_unique<Texture>(g_FrameMan.GetRenderer(), listBox->GetWidth(), listBox->GetHeight(), SDL_TEXTUREACCESS_STREAMING);
			m_ProgressListboxBitmap->clearAll(0x603e20ff); // TODO: Magic numbers

			SDL_Rect fill{0, 0, listBox->GetWidth() - 1, listBox->GetHeight() - 1};

			m_ProgressListboxBitmap->fillRect(fill, 0x854f27FF);
			fill.x++;
			fill.y++;
			fill.w--;
			fill.h--;
			m_ProgressListboxBitmap->fillRect(fill, 33);
			m_PosX = listBox->GetXPos();
			m_PosY = listBox->GetYPos();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingGUI::Destroy() {
		Clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LoadingGUI::LoadingSplashProgressReport(const std::string &reportString, bool newItem) {
		if (System::IsLoggingToCLI()) { System::PrintLoadingToCLI(reportString, newItem); }

// TODO: y u no work ;(
		g_UInputMan.Update();
		// g_FrameMan.RenderClear();
		s_LoadingSplash->Draw(g_FrameMan.GetRenderer(), *s_SplashBox);
		g_FrameMan.RenderPresent();

		if (newItem) {
			// Write out the last line to the log file before starting a new one and scroll the bitmap upwards.
			if (g_LoadingGUI.m_LoadingLogWriter) { *g_LoadingGUI.m_LoadingLogWriter << reportString << "\n"; }
#if 0
			if (g_LoadingGUI.m_ProgressListboxBitmap) {
				blit(g_LoadingGUI.m_ProgressListboxBitmap, g_LoadingGUI.m_ProgressListboxBitmap, 2, 12, 2, 2, g_LoadingGUI.m_ProgressListboxBitmap->getW() - 3, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 12); }
#endif
		}
		//TODO: If someone really wants load vis then please work this out (keep for reference)
#if 0
		if (g_LoadingGUI.m_ProgressListboxBitmap) {
			AllegroBitmap drawBitmap(g_LoadingGUI.m_ProgressListboxBitmap);

			// Clear current line.
			rectfill(g_LoadingGUI.m_ProgressListboxBitmap, 2, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 12, g_LoadingGUI.m_ProgressListboxBitmap->getW() - 3, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 3, 54);
			// Print new line
			g_FrameMan.GetSmallFont()->DrawAligned(&drawBitmap, 5, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 12, reportString.c_str(), GUIFont::Left);
			// DrawAligned - MaxWidth is useless here, so we're just drawing lines manually.
			vline(g_LoadingGUI.m_ProgressListboxBitmap, g_LoadingGUI.m_ProgressListboxBitmap->getW() - 2, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 12, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 2, 33);
			vline(g_LoadingGUI.m_ProgressListboxBitmap, g_LoadingGUI.m_ProgressListboxBitmap->getW() - 1, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 12, g_LoadingGUI.m_ProgressListboxBitmap->getH() - 2, 33);

			// Draw onto current frame buffer.
			blit(g_LoadingGUI.m_ProgressListboxBitmap, g_FrameMan.GetBackBuffer32(), 0, 0, g_LoadingGUI.m_PosX, g_LoadingGUI.m_PosY, g_LoadingGUI.m_ProgressListboxBitmap->getW(), g_LoadingGUI.m_ProgressListboxBitmap->getH());

			g_FrameMan.FlipFrameBuffers();
		}
#endif
	}
}
