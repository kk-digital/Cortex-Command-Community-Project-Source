#include "System/Singleton.h"
#include "RmlUi/Core/Types.h"
#define g_RmlUIMan RmlUIMan::Instance()

namespace Rml {
	class Context;
	class ElementDocument;
	class Event;
} // namespace Rml

namespace RTE {
	class RmlRenderInterface;
	class RmlFontEngineInterface;
	class SystemInterface;
	class EventHandler;
	class RmlUIMan : public Singleton<RmlUIMan> {
	public:
		RmlUIMan();
		~RmlUIMan();

		int Initialize();

		void Destroy();

		void Update();

		void Draw();

		bool RegisterEventHandler(const std::string &windowName, EventHandler *eventHandler);

		Rml::ElementDocument *LoadDocument(const std::string &filename);

		bool LoadFont(const std::string &filename);

		void ProcessEvents(Rml::Event &event, const std::string &value);

		Rml::Context *GetContext() { return m_RmlContext; }

	private:
		std::unique_ptr<RmlRenderInterface> m_RenderInterface;
		std::unique_ptr<RmlFontEngineInterface> m_FontEngineInterface;
		std::unique_ptr<SystemInterface> m_SystemInterface;

		EventHandler *m_CurrentEventHandler;

		robin_hood::unordered_map<std::string, EventHandler *> m_EventHandlers;

		Rml::Context *m_RmlContext;
		robin_hood::unordered_map<std::string, EventHandler *> m_Listeners;
	};
} // namespace RTE
