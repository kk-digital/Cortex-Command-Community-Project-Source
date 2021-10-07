#ifndef _RTE_RENDERER_
#define _RTE_RENDERER_
namespace RTE {
	class Drawable;
	class Renderer {
	public:
		Renderer();
		virtual ~Renderer();
		virtual void Draw(Drawable& drawable);
	};
}
#endif
