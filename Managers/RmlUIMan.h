#include "System/Singleton.h"
#define g_RmlUIMan RmlUIMan::Instance()

namespace Rml {
	class Context;
	class ElementDocument;
	class Event;
}

namespace RTE {
	class RenderInterface;
	class FontEngineInterface;
	class SystemInterface;
	class EventListener;
	class RmlUIMan : public Singleton<RmlUIMan> {
	public:
		RmlUIMan();
		~RmlUIMan();

		int Initialize();

		void Destroy();

		void Update();

		void Draw();

		bool RegisterEventHandler();

		Rml::ElementDocument* LoadDocument(const std::string &filename);

		bool LoadFont(const std::string &filename);

		void ProcessEvents(Rml::Event& event, const std::string& value);

	private:
		std::unique_ptr<RenderInterface> m_RenderInterface;
		std::unique_ptr<FontEngineInterface> m_FontEngineInterface;
		std::unique_ptr<SystemInterface> m_SystemInterface;

		Rml::Context *m_RmlContext;
		robin_hood::unordered_map<std::string, EventListener*> m_Listeners;
	};
} // namespace RTE
