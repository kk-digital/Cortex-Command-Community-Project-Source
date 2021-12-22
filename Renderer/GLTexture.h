#ifndef _RTE_GLTEXTURE_
#define _RTE_GLTEXTURE_

#include "BlendMode.h"
#include "Surface.h"
#include "RenderState.h"

namespace RTE {
	class Shader;
	enum class Shading {
		Base,
		Fill,
		Custom
	};

	enum class TextureAccess {
		Static,
		Straming
	};

	class RenderTarget;
	class GLTexture : public Surface, public std::enable_shared_from_this<GLTexture> {
		friend class ContentFile;

	public:
		virtual ~GLTexture();

		virtual bool Create(int width, int height, BitDepth format = BitDepth::Indexed8, std::optional<std::shared_ptr<Palette>> palette = std::nullopt) override;

		void render(RenderTarget *renderer, float x, float y, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, glm::vec2 pos, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, float x, float y, float angle, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, glm::vec2 pos, float angle, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, glm::vec2 pos, glm::vec2 scale, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, glm::vec2 pos, float angle, glm::vec2 scale, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, glm::vec2 pos, float angle, glm::vec2 center, glm::vec2 scale, std::optional<RenderState> state = std::nullopt);

		void render(RenderTarget *renderer, glm::vec4 src, glm::vec4 dest, std::optional<RenderState> state = std::nullopt);
		void render(RenderTarget *renderer, glm::vec4 dest, std::optional<RenderState> state = std::nullopt);

		void Update(std::optional<glm::vec4> region = std::nullopt);

		void setShading(Shading shader) { m_Shading = shader; }

		Shading getShading() const { return m_Shading; }

		void SetBaseShader(std::shared_ptr<Shader> shaderBase) { m_ShaderBase = shaderBase; }

		void SetFillShader(std::shared_ptr<Shader> shaderFill) { m_ShaderFill = shaderFill; }

		void SetCustomShader(std::shared_ptr<Shader> shaderCustom) { m_ShaderCustom = shaderCustom; }

		void setColorMod(const glm::vec3 &colorMod) { m_ColorMod = glm::vec4(colorMod, m_ColorMod.a); }

		void setColorMod(int r, int g, int b) { m_ColorMod = glm::vec4(r / 255.0, g / 255.0, b / 255.0, m_ColorMod.a); }

		void setColorMod(uint32_t rgb);

		glm::vec3 getColorMod() const { return m_ColorMod.rgb(); }

		void setAlphaMod(float alphaMod) { m_ColorMod.a = alphaMod; }

		float getAlphaMod() const { return m_ColorMod.a; }

		void ClearColor(uint32_t color = 0);

		void Bind();

		unsigned int GetTextureID() { return m_TextureID; }

		bool SaveToDisk(const std::string &filename);

	private:
		using Surface::Create;
		unsigned int m_TextureID; //!< The OpenGL texture handle associated with this texture.

		glm::vec4 m_ColorMod; //!< Color multiplied in the shader stage, used for fill color as well.

		Shading m_Shading; //!< Which shader to use while drawing.
		std::shared_ptr<Shader> m_ShaderBase; //!< Base shader appropriate for the bitdepth, also used as fallback in case other shaders are unset. MUST BE SET BEFORE DRAWING.
		std::shared_ptr<Shader> m_ShaderFill; //!< Fill shader, used for drawing silhouettes, color is determined by color mod.
		std::shared_ptr<Shader> m_ShaderCustom; //!< Custom shader, TODO: user defineable?.

		/// <summary>
		/// Returns the appropriate shader for the current shading.
		/// </summary>
		/// <returns>
		/// Shared pointer to a shader object used for rendering.
		/// </returns>
		std::shared_ptr<Shader> GetCurrentShader();

	private:
		GLTexture();
		GLTexture(std::shared_ptr<GLTexture> ref);
		bool InitializeTextureObject(int width, int height);

		template <class... Args>
		friend std::unique_ptr<GLTexture> MakeTexture(Args &&...args) {
			return std::unique_ptr<GLTexture>(new GLTexture(std::forward<Args>(args)...));
		}

		void Clear();
	};
	typedef std::shared_ptr<GLTexture> SharedTexture;

	/// <summary>
	/// Factory function for creating smart pointers of GLTexture.
	/// </summary>
	template <class... Args>
	std::unique_ptr<GLTexture> MakeTexture(Args &&...args);
} // namespace RTE
#endif
