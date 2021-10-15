#ifndef _RTEVERTEX_
#define _RTEVERTEX_
namespace RTE {
	class Vertex {
	public:
		Vertex();
		~Vertex();

		Vertex(glm::vec2 position);

		Vertex(glm::vec2 position, glm::vec2 texUV);

		Vertex(glm::vec2 position, glm::u8vec4 color);

		Vertex(float x, float y);

		Vertex(float x, float y, float u, float v);

		Vertex(float x, float y, glm::u8vec4 color);

		Vertex(float x, float y, uint32_t color);

		Vertex(float x, float y, int r = 255, int g = 0, int b = 255, int a = 255);

		glm::vec2 pos;
		glm::vec2 texUV;
		glm::u8vec4 color;
	};
} // namespace RTE
#endif
