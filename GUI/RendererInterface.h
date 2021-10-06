#ifndef _RTERENDERINTERFACE_
#define _RTERENDERINTERFACE_
#include "RmlUi/Core/RenderInterface.h"

#include "System/SDLTexture.h"

extern "C" {
struct SDL_Renderer;
struct SDL_Window;
}

namespace RTE {
	class RmlRenderInterface : public Rml::RenderInterface {
	public:
		RmlRenderInterface(SDL_Renderer *renderer, SDL_Window *window);
		~RmlRenderInterface() override;

		/// <summary>
		/// Called by RmlUi when it wants to render geometry that the application does not wish to optimise. Note that
		/// RmlUi renders everything as triangles.
		/// </summary>
		/// <param name="vertices"> The geometry's vertex data. </param>
		/// <param name="num_vertices"> The number of vertices passed to the function. </param>
		/// <param name="indices"> The geometry's index data. </param>
		/// <param name="num_indices"> The number of indices passed to the function. This will always be a multiple of three. </param>
		/// <param name="texture"> The texture to be applied to the geometry. This may be nullptr, in which case the geometry is untextured. </param>
		/// <param name="translation"> The translation to apply to the geometry. </param>
		void RenderGeometry(Rml::Vertex *vertices, int num_vertices, int *indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f &translation) override;

		/// <summary> Called by RmlUi when it wants to enable or disable scissoring to clip content. </summary>
		/// <param name="enable"> True if scissoring is to enabled, false if it is to be disabled. </param>
		void EnableScissorRegion(bool enable) override;

		/// <summary> Called by RmlUi when it wants to change the scissor region. </summary>
		/// <param name="x"> The left-most pixel to be rendered. All pixels to the left of this should be clipped. </param>
		/// <param name="y"> The top-most pixel to be rendered. All pixels to the top of this should be clipped. </param>
		/// <param name="width"> The width of the scissored region. All pixels to the right of (x + width) should be clipped. </param>
		/// <param name="height"> The height of the scissored region. All pixels to below (y + height) should be clipped. </param>
		void SetScissorRegion(int x, int y, int width, int height) override;

		/// <summary> Called by RmlUi when a texture is required by the library. </summary>
		/// <param name="texture_handle"> The handle to write the texture handle for the loaded texture to. </param>
		/// <param name="texture_dimensions"> The variable to write the dimensions of the loaded texture. </param>
		/// <param name="source"> The application-defined image source, joined with the path of the referencing document. </param>
		/// <return> True if the load attempt succeeded and the handle and dimensions are valid, false if not. </return>
		bool LoadTexture(Rml::TextureHandle &texture_handle, Rml::Vector2i &texture_dimensions, const std::string &source) override;

		/// <summary> Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels. </summary>
		/// <param name="texture_handle"> The handle to write the texture handle for the generated texture to. </param>
		/// <param name="source"> The raw 8-bit texture data. Each pixel is made up of four 8-bit values, indicating red, green, blue and alpha in that order. </param>
		/// <param name="source_dimensions"> The dimensions, in pixels, of the source data. </param>
		/// <return> True if the texture generation succeeded and the handle is valid, false if not. </return>
		bool GenerateTexture(Rml::TextureHandle &texture_handle, const Rml::byte *source, const Rml::Vector2i &source_dimensions) override;

		/// <summary> Called by RmlUi when a loaded texture is no longer required. </summary>
		/// <param name="texture"> The texture handle to release. </param>
		void ReleaseTexture(Rml::TextureHandle texture) override;

		SharedTexture GetTextureFromHandle(Rml::TextureHandle texture) { return m_LoadedTextures.at(reinterpret_cast<size_t>(texture)); }

	private:
		SDL_Renderer *m_Renderer; //!< System rendering context. NOT OWNED.
		SDL_Window *m_Window; //!< System window. NOT OWNED.

		std::vector<SharedTexture> m_LoadedTextures;
	};
} // namespace RTE
#endif
