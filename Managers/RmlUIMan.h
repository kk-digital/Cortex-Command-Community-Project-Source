#include "System/Singleton.h"
#define g_RmlUIMan RmlUIMan::Instance()

namespace Rml {
	class Context;
}

namespace RTE {
	class RenderInterface;
	class FontEngineInterface;
	class SystemInterface;
	class RmlUIMan : public Singleton<RmlUIMan> {
	public:
		RmlUIMan();
		~RmlUIMan();

		int Initialize();

		void Destroy();

		void Update();

		void Draw();

		bool RegisterEventHandler();

		bool LoadDocument();

		bool LoadFont(const std::string& filename);

	private:
		std::unique_ptr<RenderInterface> m_RenderInterface;
		std::unique_ptr<FontEngineInterface> m_FontEngineInterface;
		std::unique_ptr<SystemInterface> m_SystemInterface;

		Rml::Context *m_RmlContext;
	};
} // namespace RTE
