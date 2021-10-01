#include "RmlUIMan.h"
#include "GUI/RendererInterface.h"
#include "GUI/SystemInterface.h"
#include "GUI/FontEngineInterface.h"
#include "Menus/EventHandler.h"

#include "FrameMan.h"
#include "UInputMan.h"

#include "System/System.h"

#include "RmlUi/Core.h"
#include "RmlUi/Debugger.h"

namespace RTE {
	RmlUIMan::RmlUIMan() {
	}

	RmlUIMan::~RmlUIMan() {}

	int RmlUIMan::Initialize() {
		m_RenderInterface = std::make_unique<RenderInterface>(g_FrameMan.GetRenderer(), g_FrameMan.GetWindow());
		m_SystemInterface = std::make_unique<SystemInterface>();
		m_FontEngineInterface = std::make_unique<FontEngineInterface>(m_RenderInterface.get());

		Rml::SetRenderInterface(m_RenderInterface.get());
		Rml::SetSystemInterface(m_SystemInterface.get());
		// Rml::SetFontEngineInterface(m_FontEngineInterface.get());
		RTEAssert(Rml::Initialise(), "Failed to initialize RmlUI");

		// TODO: create font entities maybe? defo not hardcode
		// std::array<std::string, 4> fonts = {
		//     "Base.rte/RmlUI/Fonts/FontSmall.png",
		//     "Base.rte/RmlUI/Fonts/FontLarge.png",
		//     "Base.rte/RmlUI/Fonts/FontConsoleMonospace.png",
		//     "Base.rte/RmlUI/Fonts/FontMainMenu.png"};

		struct Font {
			std::string filename;
			bool fallback_face;
		};
		std::vector<Font> fonts{
		    {"LatoLatin-Regular.ttf", false},
		    {"LatoLatin-Italic.ttf", false},
		    {"LatoLatin-Bold.ttf", false},
		    {"LatoLatin-BoldItalic.ttf", false},
		    {"NotoEmoji-Regular.ttf", true}};

		for (const Font &font: fonts) {
			Rml::LoadFontFace("Base.rte/RmlUI/Fonts/" + font.filename, font.fallback_face);
		}

		m_RmlContext = Rml::CreateContext("main", Rml::Vector2i(g_FrameMan.GetResX(), g_FrameMan.GetResY()));

		if (!m_RmlContext) {
			Rml::Shutdown();
			return -1;
		}

		Rml::Debugger::Initialise(m_RmlContext);
		Rml::Debugger::SetVisible(true);
		m_RmlContext->LoadDocument("Base.rte/RmlUI/Menus/Example.rml")->Show();

		return 0;
	}

	void RmlUIMan::Destroy() {
		Rml::Shutdown();
	}

	void RmlUIMan::Update() {
		if(!g_UInputMan.GetMouseMovement().IsZero()) {
			Vector mousePosition = g_UInputMan.GetMousePosition();
			m_RmlContext->ProcessMouseMove(mousePosition.m_X, mousePosition.m_Y, KMOD_NONE);
		}

		m_RmlContext->Update();
	}

	void RmlUIMan::Draw() {
		m_RmlContext->Render();
	}

	bool RmlUIMan::RegisterEventHandler(const std::string &windowName, EventHandler *eventHandler) {
		auto eventHandlerIt = m_EventHandlers.find(windowName);
		if (eventHandlerIt != m_EventHandlers.end()) {
			return true;
		} else {
			m_EventHandlers[windowName] = eventHandler;
		}

		return false;
	}

	Rml::ElementDocument *RmlUIMan::LoadDocument(const std::string &filename) {
		// FIXME: For now only load documents from Base.rte
		Rml::ElementDocument *document = m_RmlContext->LoadDocument("Base.rte/RmlUI/Menus/" + filename + ".rml");
		if (document) {
			Rml::Element *title = document->GetElementById("title");
			if (title) {
				title->SetInnerRML(document->GetTitle());
			}

			robin_hood::unordered_map<std::string, EventHandler *>::iterator eventHandlerIt = m_EventHandlers.find(filename);
			if (eventHandlerIt != m_EventHandlers.end()) {
				m_CurrentEventHandler = eventHandlerIt->second;
			}

			document->Show();
		}
		return document;
	}

	bool RmlUIMan::LoadFont(const std::string &filename) {
		return Rml::LoadFontFace(filename);
	}

	void RmlUIMan::ProcessEvents(Rml::Event &event, const std::string &value) {
		Rml::StringList commands;
		Rml::StringUtilities::ExpandString(commands, value, ';');
		for (size_t i(0); i < commands.size(); ++i) {
			Rml::StringList values;
			Rml::StringUtilities::ExpandString(values, commands[i], ' ');
			if (values.empty()) {
				return;
			}

			if (values[0] == "goto" && values.size() > 1) {
				if (LoadDocument(values[1])) {
					event.GetTargetElement()->GetOwnerDocument()->Close();
				}
			} else if (values[0] == "load" && values.size() > 1) {
				LoadDocument(values[1]);
			} else if (values[0] == "close") {
				Rml::ElementDocument *targetDocument = nullptr;

				if (values.size() > 1) {
					targetDocument = m_RmlContext->GetDocument(values[1].c_str());
				} else {
					targetDocument = event.GetTargetElement()->GetOwnerDocument();
				}

				if (targetDocument) {
					targetDocument->Close();
				}
			} else if (values[0] == "exit") {
				System::SetQuit();
			} else if (m_CurrentEventHandler) {
				m_CurrentEventHandler->ProcessEvent(event, commands[i]);
			}
		}
	}
} // namespace RTE
