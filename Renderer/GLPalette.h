#ifndef _RTEPALETTE_
#define _RTEPALETTE_
struct SDL_Palette;
namespace RTE {
	class Palette {
	public:
		static constexpr int PALETTESIZE = 256;

		Palette(const std::array<glm::u8vec4, PALETTESIZE> &data);
		~Palette();

		unsigned int GetTextureID() { return m_Texture; }

		SDL_Palette *GetAsPalette();

		glm::u8vec4& at(size_t index) {return m_Palette.at(index); }

		unsigned char GetIndexFromColor(const glm::vec4& color);

		void Bind();

		void Update();

	private:
		unsigned int m_Texture;

		std::array<glm::u8vec4, PALETTESIZE> m_Palette;

		struct sdl_palette_deleter {
			void operator()(SDL_Palette *p);
		};
		std::unique_ptr<SDL_Palette, sdl_palette_deleter> m_SDLPalette;
	public:
		glm::u8vec4 & operator[] (size_t index) {
			return m_Palette[index];
		}
	};
} // namespace RTE
#endif
