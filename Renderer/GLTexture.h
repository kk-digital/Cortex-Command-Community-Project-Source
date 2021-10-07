#ifndef _RTE_GLTEXTURE_
#define _RTE_GLTEXTURE_
#include "GL/gl.h"

extern "C" {
struct SDL_Surface;
}

namespace RTE {
	class Shader;
	enum class Shading {
		Base,
		Fill,
		Custom
	};

	class sdl_surface_deleter {
		void operator()(SDL_Surface *p);
	};

	class GLTexture {
	public:
		GLTexture();
		~GLTexture();

		void Clear();

		int getW() const { return m_Width; }
		int getH() const { return m_Height; }

		void render(void *, float x, float y);

		void render(void *, glm::vec2 pos);

		void render(void *, float x, float y, double angle);

		void render(void *, glm::vec2 pos, double angle);

		void render(void *, glm::vec2 pos, glm::vec2 scale);

		void render(void *, glm::vec2 pos, double angle, glm::vec2 scale);

		void setShading(Shading shader) { m_Shading = shader; }

		Shading getShading() const { return m_Shading; }

		void setBlendMode(uint32_t blendMode) { m_BlendMode = blendMode; }
		uint32_t getBlendMode() const { return m_BlendMode; }

		void setColorMod(const glm::vec3 &colorMod) { m_ColorMod = glm::vec4(colorMod, m_ColorMod.a); }

		glm::vec3 getColorMod() const { return m_ColorMod.rgb(); }

		void setAlphaMod(float alphaMod) { m_ColorMod.a = alphaMod; }

		float getAlphaMod() const { return m_ColorMod.a; }

		uint32_t getPixel(int x, int y) const;

		void setPixel(int x, int y, uint32_t color);

		void clearAll(uint32_t color = 0);

	private:
		GLuint m_VBO;
		GLuint m_TextureID;
		int m_BPP;

		int m_Width;
		int m_Height;

		glm::vec4 m_ColorMod;

		uint32_t m_Format;
		uint32_t m_BlendMode;

		Shading m_Shading;
		Shader *m_ShaderBase;
		Shader *m_ShaderFill;
		glm::vec4 m_FillColor;
		Shader *m_ShaderCustom;

		std::unique_ptr<SDL_Surface, sdl_surface_deleter> m_Pixels;

		Shader* GetCurrentShader();
	};
} // namespace RTE
#endif
