#include "RmlUIMan.h"
#include "GUI/RendererInterface.h"
#include "GUI/SystemInterface.h"
#include "GUI/FontEngineInterface.h"

#include "FrameMan.h"

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
		Rml::SetFontEngineInterface(m_FontEngineInterface.get());
		Rml::Initialise();

		m_RmlContext = Rml::CreateContext("main", Rml::Vector2i(g_FrameMan.GetResX(), g_FrameMan.GetResY()));

		if (!m_RmlContext) {
			Rml::Shutdown();
			return -1;
		}

		// TODO: create font entities maybe? defo not hardcode
		std::array<std::string, 4> fonts = {
		    "Base.rte/RmlUI/Fonts/FontSmall.png",
		    "Base.rte/RmlUI/Fonts/FontLarge.png",
		    "Base.rte/RmlUI/Fonts/FontConsoleMonospace.png",
		    "Base.rte/RmlUI/Fonts/FontMainMenu.png"};

		for (std::array<std::string, 4>::const_iterator fontIt = fonts.begin(); fontIt < fonts.end(); fontIt++) {
			Rml::LoadFontFace(*fontIt);
		}

#ifdef DEBUG
		Rml::Debugger::Initialise(m_RmlContext);
#endif

		return 0;
	}

	void RmlUIMan::Destroy() {
		Rml::Shutdown();
	}

	void RmlUIMan::Update() {
		m_RmlContext->Update();
	}

	void RmlUIMan::Draw() {
		m_RmlContext->Render();
	}

	bool RmlUIMan::RegisterEventHandler() { return false; }

	Rml::ElementDocument *RmlUIMan::LoadDocument(const std::string &filename) {
		Rml::ElementDocument *document = m_RmlContext->LoadDocument(filename);
		if(document){
			Rml::Element* title = document->GetElementById("title");
			if(title){
				title->SetInnerRML(document->GetTitle());
			}
		}
		return document;
	}

	bool RmlUIMan::LoadFont(const std::string &filename) {
		return Rml::LoadFontFace(filename);
	}
} // namespace RTE
