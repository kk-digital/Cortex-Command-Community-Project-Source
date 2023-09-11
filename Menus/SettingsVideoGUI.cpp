#include "SettingsVideoGUI.h"
#include "WindowMan.h"
#include "FrameMan.h"
#include "ActivityMan.h"

#include "GUIInputWrapper.h"
#include "GUI.h"
#include "GUICollectionBox.h"
#include "GUILabel.h"
#include "GUIButton.h"
#include "GUIRadioButton.h"
#include "GUICheckbox.h"
#include "GUIComboBox.h"
#include "GUITextBox.h"

#include "SDL_video.h"
#include <format>

namespace RTE {

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SettingsVideoGUI::SettingsVideoGUI(GUIControlManager *parentControlManager) : m_GUIControlManager(parentControlManager) {
		m_NewResX = g_WindowMan.GetResX();
		m_NewResY = g_WindowMan.GetResY();
		m_NewResMultiplier = g_WindowMan.GetResMultiplier();
		m_NewFullscreen = g_WindowMan.IsFullscreen();

		m_VideoSettingsBox = dynamic_cast<GUICollectionBox *>(m_GUIControlManager->GetControl("CollectionBoxVideoSettings"));

		m_ResolutionQuickToggleButtons[ResolutionQuickChangeType::Windowed] = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonQuickWindowed"));
		m_ResolutionQuickToggleButtons[ResolutionQuickChangeType::Fullscreen] = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonQuickBorderless"));
		m_ResolutionQuickToggleButtons[ResolutionQuickChangeType::UpscaledFullscreen] = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonQuickUpscaledBorderless"));

		m_TwoPlayerSplitscreenHSplitRadioButton = dynamic_cast<GUIRadioButton *>(m_GUIControlManager->GetControl("RadioSplitscreenHoriz"));
		m_TwoPlayerSplitscreenVSplitRadioButton = dynamic_cast<GUIRadioButton *>(m_GUIControlManager->GetControl("RadioSplitscreenVert"));
		m_TwoPlayerSplitscreenVSplitRadioButton->SetCheck(g_FrameMan.GetTwoPlayerVSplit());

		m_EnableVSyncCheckbox = dynamic_cast<GUICheckbox *>(m_GUIControlManager->GetControl("CheckboxEnableVSync"));
		m_EnableVSyncCheckbox->SetCheck(g_WindowMan.GetVSyncEnabled());

		m_FullscreenCheckbox = dynamic_cast<GUICheckbox *>(m_GUIControlManager->GetControl("CheckboxFullscreen"));
		m_FullscreenCheckbox->SetCheck(m_NewFullscreen);

		m_IgnoreMultiDisplaysCheckbox = dynamic_cast<GUICheckbox *>(m_GUIControlManager->GetControl("CheckboxIgnoreMultiDisplays"));
		m_IgnoreMultiDisplaysCheckbox->SetCheck(g_WindowMan.GetIgnoreMultiDisplays());
		m_IgnoreMultiDisplaysCheckbox->SetVisible(m_IgnoreMultiDisplaysCheckbox->GetVisible() && SDL_GetNumVideoDisplays() > 1);

		m_PresetResolutionRadioButton = dynamic_cast<GUIRadioButton *>(m_GUIControlManager->GetControl("RadioPresetResolution"));
		m_CustomResolutionRadioButton = dynamic_cast<GUIRadioButton *>(m_GUIControlManager->GetControl("RadioCustomResolution"));

		m_ResolutionChangeDialogBox = dynamic_cast<GUICollectionBox *>(m_GUIControlManager->GetControl("ResolutionChangeDialog"));
		m_ResolutionChangeDialogBox->SetVisible(false);

		const GUICollectionBox *settingsRootBox = dynamic_cast<GUICollectionBox *>(m_GUIControlManager->GetControl("CollectionBoxSettingsBase"));
		m_ResolutionChangeDialogBox->SetPositionAbs(settingsRootBox->GetXPos() + ((settingsRootBox->GetWidth() - m_ResolutionChangeDialogBox->GetWidth()) / 2), settingsRootBox->GetYPos() + ((settingsRootBox->GetHeight() - m_ResolutionChangeDialogBox->GetHeight()) / 2));

		m_ResolutionChangeConfirmButton = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonConfirmResolutionChange"));
		m_ResolutionChangeCancelButton = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonCancelResolutionChange"));

		CreatePresetResolutionBox();
		CreateCustomResolutionBox();

		if (m_PresetResolutionComboBox->GetSelectedIndex() < 0) {
			m_CustomResolutionRadioButton->SetCheck(true);
			m_PresetResolutionBox->SetVisible(false);
			m_CustomResolutionBox->SetVisible(true);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::CreatePresetResolutionBox() {
		m_PresetResolutionBox = dynamic_cast<GUICollectionBox *>(m_GUIControlManager->GetControl("CollectionPresetResolution"));
		m_PresetResolutionComboBox = dynamic_cast<GUIComboBox *>(m_GUIControlManager->GetControl("ComboPresetResolution"));
		m_PresetResolutionApplyButton = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonApplyPresetResolution"));
		m_PresetResolutionMessageLabel = dynamic_cast<GUILabel *>(m_GUIControlManager->GetControl("LabelPresetResolutonValidation"));
		m_PresetResolutionMessageLabel->SetVisible(false);

		PopulateResolutionsComboBox();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::CreateCustomResolutionBox() {
		m_CustomResolutionBox = dynamic_cast<GUICollectionBox *>(m_GUIControlManager->GetControl("CollectionCustomResolution"));
		m_CustomResolutionBox->SetVisible(false);

		m_CustomResolutionWidthTextBox = dynamic_cast<GUITextBox *>(m_GUIControlManager->GetControl("TextboxCustomWidth"));
		m_CustomResolutionWidthTextBox->SetNumericOnly(true);
		m_CustomResolutionWidthTextBox->SetMaxNumericValue(g_WindowMan.GetMaxResX());
		m_CustomResolutionWidthTextBox->SetMaxTextLength(4);
		m_CustomResolutionWidthTextBox->SetText(std::to_string(static_cast<int>(g_WindowMan.GetWindowResX())));

		m_CustomResolutionHeightTextBox = dynamic_cast<GUITextBox *>(m_GUIControlManager->GetControl("TextboxCustomHeight"));
		m_CustomResolutionHeightTextBox->SetNumericOnly(true);
		m_CustomResolutionHeightTextBox->SetMaxNumericValue(g_WindowMan.GetMaxResY());
		m_CustomResolutionHeightTextBox->SetMaxTextLength(4);
		m_CustomResolutionHeightTextBox->SetText(std::to_string(static_cast<int>(g_WindowMan.GetWindowResY())));

		m_CustomResolutionMultiplierComboBox = dynamic_cast<GUIComboBox *>(m_GUIControlManager->GetControl("ComboboxResolutionMultiplier"));
		PopulateResMultplierComboBox();
#if __cpp_lib_format >= 201907L
		m_CustomResolutionMultiplierComboBox->SetText(std::format("{:.3g}x", m_NewResMultiplier));
#else
		m_CustomResolutionMultiplier->SetText(std::to_string(m_NewResMultiplier));
#endif

		m_CustomResolutionApplyButton = dynamic_cast<GUIButton *>(m_GUIControlManager->GetControl("ButtonApplyCustomResolution"));
		m_CustomResolutionMessageLabel = dynamic_cast<GUILabel *>(m_GUIControlManager->GetControl("LabelCustomResolutionValidation"));
		m_CustomResolutionMessageLabel->SetVisible(false);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::SetEnabled(bool enable) const {
		m_VideoSettingsBox->SetVisible(enable);
		m_VideoSettingsBox->SetEnabled(enable);

		if (enable) {
			m_CustomResolutionWidthTextBox->SetText(std::to_string(g_WindowMan.GetWindowResX()));
			m_CustomResolutionHeightTextBox->SetText(std::to_string(g_WindowMan.GetWindowResY()));
#if __cpp_lib_format >= 201907L
		m_CustomResolutionMultiplierComboBox->SetText(std::format("{:.3g}x", m_NewResMultiplier));
#else
		m_CustomResolutionMultiplier->SetText(std::to_string(m_NewResMultiplier));
#endif
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUICollectionBox * SettingsVideoGUI::GetActiveDialogBox() const {
		return (m_ResolutionChangeDialogBox->GetEnabled() && m_ResolutionChangeDialogBox->GetVisible()) ? m_ResolutionChangeDialogBox : nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::CloseActiveDialogBox() const {
		if (m_ResolutionChangeDialogBox->GetEnabled() && m_ResolutionChangeDialogBox->GetVisible()) {
			m_ResolutionChangeDialogBox->SetVisible(false);
			m_VideoSettingsBox->SetEnabled(true);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool SettingsVideoGUI::IsSupportedResolution(int width, int height) const {
		if ((width >= c_MinResX && height >= c_MinResY) && (width <= g_WindowMan.GetMaxResX() && height <= g_WindowMan.GetMaxResY())) {
			// Filter wacky resolutions that are taller than wide and some other dumb ones.
			if ((height > width)) {
				return false;
			}
			return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::PopulateResolutionsComboBox() {
		m_PresetResolutions.clear();

		// Get a list of modes from the graphics deriver. This is so we don't need to populate the list manually, and have all the reasonable resolutions.
		std::vector<SDL_DisplayMode> modeList;
		std::set<PresetResolutionRecord> resRecords;

		int displayIndex = SDL_GetWindowDisplayIndex(g_WindowMan.GetWindow());

		for (int i = 0; i < SDL_GetNumDisplayModes(displayIndex); ++i) {
			SDL_DisplayMode mode;
			if (SDL_GetDisplayMode(displayIndex, i, &mode) == 0) {
				modeList.push_back(mode);

				if ((SDL_BITSPERPIXEL(mode.format) == 32 || SDL_BITSPERPIXEL(mode.format) == 24) && IsSupportedResolution(mode.w, mode.h)) {
					resRecords.emplace(mode.w, mode.h, false);
				}
			} else {
				(void)SDL_GetError();
			}
		}

		if (modeList.empty()) {
			m_PresetResolutionComboBox->SetVisible(false);
			m_PresetResolutionApplyButton->SetVisible(false);

			m_PresetResolutionMessageLabel->SetText("Failed to get the preset resolution list from the graphics driver!\nPlease use the custom resolution controls instead.");
			m_PresetResolutionMessageLabel->SetVisible(true);
			m_PresetResolutionMessageLabel->CenterInParent(true, true);
			return;
		}

		// Manually add qHD (960x540) to the list because it's rarely present in drivers.
		resRecords.emplace(960, 540, false);

		std::set<PresetResolutionRecord> upscaledResRecords;
		for (const PresetResolutionRecord &resRecord: resRecords) {
			PresetResolutionRecord upscaledResRecord(resRecord.Width * 2, resRecord.Height * 2, true);
			if (IsSupportedResolution(upscaledResRecord.Width, upscaledResRecord.Height)) {
				upscaledResRecords.emplace(upscaledResRecord);
			}
		}
		resRecords.merge(upscaledResRecords);
		m_PresetResolutions.assign(resRecords.begin(), resRecords.end());

		for (int i = 0; i < m_PresetResolutions.size(); ++i) {
			const PresetResolutionRecord &resRecord = m_PresetResolutions[i];
			m_PresetResolutionComboBox->AddItem(resRecord.GetDisplayString());
			if (m_PresetResolutionComboBox->GetSelectedIndex() < 0 && (resRecord.Width == g_WindowMan.GetResX() * g_WindowMan.GetResMultiplier()) && (resRecord.Height == g_WindowMan.GetResY() * g_WindowMan.GetResMultiplier()) && (resRecord.Upscaled == g_WindowMan.GetResMultiplier() > 1)) {
				m_PresetResolutionComboBox->SetSelectedIndex(i);
			}
		}
	}

	void SettingsVideoGUI::PopulateResMultplierComboBox() {
		float maxResX = g_WindowMan.GetIgnoreMultiDisplays() ? g_WindowMan.GetPrimaryWindowDisplayWidth() : g_WindowMan.GetMaxResX();
		float maxResY = g_WindowMan.GetIgnoreMultiDisplays() ? g_WindowMan.GetPrimaryWindowDisplayHeight() : g_WindowMan.GetMaxResY();

		float maximumResMultiplier = std::floor((std::min<float>(maxResX / c_MinResX, maxResY / c_MinResY) * 2.f) + 0.5f) / 2.0f;
		m_CustomResolutionMultiplierComboBox->ClearList();

		for (float resMultiplier = 1.0f; resMultiplier <= maximumResMultiplier; resMultiplier += 0.5) {
#if __cpp_lib_format >= 201907L
			m_CustomResolutionMultiplierComboBox->AddItem(std::format("{:.3g}x", resMultiplier));
#else
			m_CustomResolutionMultiplier->AddItem(std::to_string(resMultiplier));
#endif
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::UpdateCustomResolutionLimits() {
		g_WindowMan.MapDisplays();

		int newMaxResX = g_WindowMan.GetMaxResX();
		int newMaxResY = g_WindowMan.GetMaxResY();

		m_CustomResolutionWidthTextBox->SetMaxNumericValue(newMaxResX);
		if (std::string widthText = m_CustomResolutionWidthTextBox->GetText(); widthText.empty() || (!widthText.empty() && std::stoi(widthText) > newMaxResX)) {
			m_CustomResolutionWidthTextBox->SetText(std::to_string(newMaxResX));
		}

		m_CustomResolutionHeightTextBox->SetMaxNumericValue(newMaxResY);
		if (std::string heightText = m_CustomResolutionHeightTextBox->GetText(); heightText.empty() || (!heightText.empty() && std::stoi(heightText) > newMaxResY)) {
			m_CustomResolutionHeightTextBox->SetText(std::to_string(newMaxResY));
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::ApplyNewResolution(bool displaysWereMapped) {
		if (g_ActivityMan.GetActivity() && (g_ActivityMan.GetActivity()->GetActivityState() == Activity::Running || g_ActivityMan.GetActivity()->GetActivityState() == Activity::Editing)) {
			m_ResolutionChangeDialogBox->SetVisible(true);
			m_VideoSettingsBox->SetEnabled(false);
		} else {
			m_ResolutionChangeDialogBox->SetVisible(false);
			m_VideoSettingsBox->SetEnabled(true);
			g_WindowMan.ChangeResolution(m_NewResX, m_NewResY, m_NewResMultiplier, m_NewFullscreen, displaysWereMapped);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::ApplyQuickChangeResolution(ResolutionQuickChangeType resolutionChangeType) {
		g_WindowMan.MapDisplays();

		switch (resolutionChangeType) {
			case ResolutionQuickChangeType::Windowed:
				m_NewResMultiplier = 1.0f;
				m_NewResX = c_DefaultResX;
				m_NewResY = c_DefaultResY;
				m_NewFullscreen = false;
				break;
			case ResolutionQuickChangeType::Fullscreen:
				m_NewResMultiplier = 1.0f;
				m_NewResX = g_WindowMan.GetMaxResX();
				m_NewResY = g_WindowMan.GetMaxResY();
				m_NewFullscreen = true;
				break;
			case ResolutionQuickChangeType::UpscaledFullscreen:
				m_NewResMultiplier = 2.0f;
				m_NewResX = g_WindowMan.GetMaxResX() / 2.0f;
				m_NewResY = g_WindowMan.GetMaxResY() / 2.0f;
				m_NewFullscreen = true;
				break;
			default:
				RTEAbort("Invalid resolution quick change type passed to SettingsVideoGUI::ApplyQuickChangeResolution!");
				break;
		}
		g_GUISound.ButtonPressSound()->Play();
		ApplyNewResolution(true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::ApplyPresetResolution() {
		int presetResListEntryID = m_PresetResolutionComboBox->GetSelectedIndex();
		if (presetResListEntryID >= 0) {
			m_NewResMultiplier = m_PresetResolutions.at(presetResListEntryID).Upscaled ? 2.0f : 1.0f;

			m_NewResX = m_PresetResolutions.at(presetResListEntryID).Width / m_NewResMultiplier;
			m_NewResY = m_PresetResolutions.at(presetResListEntryID).Height / m_NewResMultiplier;

			g_GUISound.ButtonPressSound()->Play();
			m_NewFullscreen = m_FullscreenCheckbox->GetCheck();
			ApplyNewResolution();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::ApplyCustomResolution() {
		m_CustomResolutionMessageLabel->SetVisible(false);
		m_NewFullscreen = m_FullscreenCheckbox->GetCheck();

		float newResMultiplier(.0f);
		int exitCode = sscanf(m_CustomResolutionMultiplierComboBox->GetText().c_str(), "%f%*s", &newResMultiplier);

		if (exitCode == EOF || newResMultiplier == 0) {
			m_CustomResolutionMessageLabel->SetText("Resolution Multiplier is not a valid number!");
			m_CustomResolutionMessageLabel->SetVisible(true);
			return;
		}

		m_NewResMultiplier = newResMultiplier;

		int newResX = g_WindowMan.GetResX();
		int newResY = g_WindowMan.GetResY();

		if (std::string newResXString = m_CustomResolutionWidthTextBox->GetText(); !newResXString.empty()) {
			newResX = std::stoi(newResXString);
		} else {
			m_CustomResolutionWidthTextBox->SetText(std::to_string(newResX));
		}

		if (std::string newResYString = m_CustomResolutionHeightTextBox->GetText(); !newResYString.empty()) {
			newResY = std::stoi(newResYString);
		} else {
			m_CustomResolutionHeightTextBox->SetText(std::to_string(newResY));
		}

		m_NewResX = newResX / m_NewResMultiplier;
		m_NewResY = newResY / m_NewResMultiplier;

		bool invalidResolution = false;

		if (m_NewResX * m_NewResMultiplier < c_MinResX || m_NewResY * m_NewResMultiplier < c_MinResY) {
			m_CustomResolutionMessageLabel->SetText("Resolution width or height lower than the minimum (" + std::to_string(c_MinResX) + "x" + std::to_string(c_MinResY) + ") is not supported.");
			invalidResolution = true;
		}
		if (newResX < c_DangerResX || newResY < c_DangerResY) {
			m_CustomResolutionMessageLabel->SetText("Resolution is dangerously low!");
			invalidResolution = true;
		} else if (m_NewResX < c_DangerResX || m_NewResY < c_DangerResY) {
			m_CustomResolutionMessageLabel->SetText("Resolution multiplier is too high for the specified resolution.");
			invalidResolution = true;
		}
		g_GUISound.ButtonPressSound()->Play();

		if (invalidResolution) {
			m_CustomResolutionMessageLabel->SetVisible(true);
			return;
		} else {
			ApplyNewResolution();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SettingsVideoGUI::HandleInputEvents(GUIEvent &guiEvent) {
		if (guiEvent.GetType() == GUIEvent::Command) {
			if (guiEvent.GetMsg() == GUIButton::Pushed) {
				if (guiEvent.GetControl() == m_ResolutionQuickToggleButtons[ResolutionQuickChangeType::Windowed]) {
					ApplyQuickChangeResolution(ResolutionQuickChangeType::Windowed);
				} else if (guiEvent.GetControl() == m_ResolutionQuickToggleButtons[ResolutionQuickChangeType::Fullscreen]) {
					ApplyQuickChangeResolution(ResolutionQuickChangeType::Fullscreen);
				} else if (guiEvent.GetControl() == m_ResolutionQuickToggleButtons[ResolutionQuickChangeType::UpscaledFullscreen]) {
					ApplyQuickChangeResolution(ResolutionQuickChangeType::UpscaledFullscreen);
				} else if (guiEvent.GetControl() == m_PresetResolutionApplyButton) {
					ApplyPresetResolution();
				} else if (guiEvent.GetControl() == m_CustomResolutionApplyButton) {
					ApplyCustomResolution();
				} else if (guiEvent.GetControl() == m_ResolutionChangeConfirmButton) {
					g_GUISound.ButtonPressSound()->Play();
					// Must end any running activity otherwise have to deal with recreating all the GUI elements in GameActivity because it crashes when opening the BuyMenu. Easier to just end it.
					g_ActivityMan.EndActivity();
					ApplyNewResolution();
				} else if (guiEvent.GetControl() == m_ResolutionChangeCancelButton) {
					g_GUISound.ButtonPressSound()->Play();
					m_ResolutionChangeDialogBox->SetVisible(false);
					m_VideoSettingsBox->SetEnabled(true);
				}
			}
		} else if (guiEvent.GetType() == GUIEvent::Notification) {
			// Clicking off focused textboxes does not remove their focus and they will still capture keyboard input, so remove focus when clicking CollectionBoxes.
			if (guiEvent.GetMsg() == GUICollectionBox::Clicked && !m_VideoSettingsBox->HasFocus() && (guiEvent.GetControl() == m_VideoSettingsBox || guiEvent.GetControl() == m_CustomResolutionBox)) {
				if (m_CustomResolutionWidthTextBox->GetText().empty()) {
					m_CustomResolutionWidthTextBox->SetText(std::to_string(g_WindowMan.GetWindowResX()));
				}
				if (m_CustomResolutionHeightTextBox->GetText().empty()) {
					m_CustomResolutionHeightTextBox->SetText(std::to_string(g_WindowMan.GetWindowResY()));
				}
				m_VideoSettingsBox->SetFocus();
			}

			if (guiEvent.GetControl() == m_PresetResolutionComboBox) {
				if (guiEvent.GetMsg() == GUIComboBox::Dropped) {
					m_PresetResolutionBox->Resize(m_PresetResolutionBox->GetWidth(), 165);
				} else if (guiEvent.GetMsg() == GUIComboBox::Closed) {
					m_PresetResolutionBox->Resize(m_PresetResolutionBox->GetWidth(), 40);
				}
			}

			if (guiEvent.GetControl() == m_CustomResolutionMultiplierComboBox) {
				if (guiEvent.GetMsg() == GUIComboBox::Dropped) {
					m_CustomResolutionBox->Resize(m_CustomResolutionBox->GetWidth(), 165);
				} else if (guiEvent.GetMsg() == GUIComboBox::Closed) {
					m_CustomResolutionBox->Resize(m_CustomResolutionBox->GetWidth(), 40);
				}
			}

			if (guiEvent.GetMsg() == GUICheckbox::Changed) {
				if (guiEvent.GetControl() == m_EnableVSyncCheckbox) {
					g_WindowMan.SetVSyncEnabled(m_EnableVSyncCheckbox->GetCheck());
				} else if (guiEvent.GetControl() == m_IgnoreMultiDisplaysCheckbox) {
					g_WindowMan.SetIgnoreMultiDisplays(m_IgnoreMultiDisplaysCheckbox->GetCheck());
					UpdateCustomResolutionLimits();
				}
			} else if (guiEvent.GetMsg() == GUIRadioButton::Pushed) {
				if (guiEvent.GetControl() == m_TwoPlayerSplitscreenHSplitRadioButton) {
					g_GUISound.ButtonPressSound()->Play();
					g_FrameMan.SetTwoPlayerVSplit(false);
				} else if (guiEvent.GetControl() == m_TwoPlayerSplitscreenVSplitRadioButton) {
					g_GUISound.ButtonPressSound()->Play();
					g_FrameMan.SetTwoPlayerVSplit(true);
				} else if (guiEvent.GetControl() == m_PresetResolutionRadioButton) {
					g_GUISound.ButtonPressSound()->Play();
					m_PresetResolutionBox->SetVisible(true);
					m_CustomResolutionBox->SetVisible(false);
				} else if (guiEvent.GetControl() == m_CustomResolutionRadioButton) {
					g_GUISound.ButtonPressSound()->Play();
					m_PresetResolutionBox->SetVisible(false);
					CreateCustomResolutionBox();
					m_CustomResolutionBox->SetVisible(true);
				}
			}
		}
	}
} // namespace RTE