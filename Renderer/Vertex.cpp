#include "Vertex.h"

namespace RTE {
	Vertex::Vertex() :
	    pos(0, 0), texUV(0, 0), color(0) {}
	Vertex::~Vertex() {}

	Vertex::Vertex(glm::vec2 position) :
	    pos(position), texUV(0, 0), color(0) {}

	Vertex::Vertex(glm::vec2 position, glm::vec2 textureUV) :
	    pos(position), texUV(textureUV), color(0) {}

	Vertex::Vertex(glm::vec2 position, glm::u8vec4 color) :
	    pos(position), texUV(0, 0), color(color) {}

	Vertex::Vertex(glm::vec2 position, uint32_t color) :
	    pos(position), texUV(0, 0), color((color >> 16) & 0xFF, (color >> 8) & 0xff, (color)&0xff, (color >> 24) & 0xFF) {}

	Vertex::Vertex(float x, float y) :
	    pos(x, y), texUV(0), color(0) {}

	Vertex::Vertex(float x, float y, float u, float v) :
	    pos(x, y), texUV(u, v), color(0) {}

	Vertex::Vertex(float x, float y, glm::u8vec4 color) :
	    pos(x, y), color(color) {}

	Vertex::Vertex(float x, float y, uint32_t color) :
	    pos(x, y), texUV(0, 0), color((color >> 16) & 0xFF, (color >> 8) & 0xff, (color)&0xff, (color >> 24) & 0xFF) {}

	Vertex::Vertex(float x, float y, int r, int g, int b, int a) :
	    pos(x, y), texUV(0, 0), color(r, g, b, a) {}
} // namespace RTE
