#ifndef _RTESURFACE_
#define _RTESURFACE_
extern "C" {
	struct SDL_Surface;
}

namespace RTE {
	enum class BitDepth {
		Indexed8,

		BPP32,
	};

	struct sdl_surface_deleter {
		void operator()(SDL_Surface *p);
	};

	class Surface {
		friend class ContentFile;
	public:
		Surface();
		virtual ~Surface();

		/// <summary>
		/// Create an empty surface with a defined pixelformat of size Width x Height.
		/// </summary>
		/// <param name="width">
		/// The width to set.
		/// </param>
		/// <param name="height">
		/// The height to set.
		/// </param>
		/// <param name="format">
		/// The format this surface will be created with.
		/// </param>
		/// <returns>
		/// True if successful.
		/// </returns>
		virtual bool Create(int width, int height, uint32_t format);

		/// <summary>
		/// Returns the stored SDL_Surface. Ownership is not transferred.
		/// </summary>
		/// <returns>
		/// Non owning pointer to the pixels of this surface.
		/// </returns>
		SDL_Surface* GetPixels() { return m_Pixels.get(); }

		/// <summary>
		/// Get the width in pixels of this surface.
		/// </summary>
		/// <returns>
		/// Returnss the width of the surface.
		/// </returns>
		int getW() const { return m_Width; }

		/// <summary>
		/// Get the height in pixels of this surface.
		/// </summary>
		/// <returns>
		/// Returns the height of this surface.
		/// </returns>
		int getH() const { return m_Height; }

		glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height);}

		uint32_t GetPixel(int x, int y);

		void blit(Surface& target, int x, int y, double angle=0, float scaleX=1.0f, float scaleY=1.0f) const;

		void blitColor(Surface& target, uint32_t color, int x, int y, double angle=0, float scaleX=1.0f, float scaleY=1.0f) const;

	private:
		int m_Width;
		int m_Height;
		int m_BPP;
		std::unique_ptr<SDL_Surface, sdl_surface_deleter> m_Pixels;
		std::vector<unsigned long> m_Palette;
	};

}
#endif
