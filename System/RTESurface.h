extern "C"{
	struct SDL_Surface;
	void SDL_FreeSurface(SDL_Surface*);
}

namespace RTE{
	class Surface{
	public:
		Surface(int width, int height);
		void setPixel(int x, int y, uint32_t color);
		uint32_t getPixel(int x, int y);
		// std::unique_ptr<SDL_Surface, decltype(SDL_FreeSurface)> getAsSDLSurface();

		int getW() {return w;}
		int getH() {return h;}

		int w;
		int h;
	private:
		std::vector<uint32_t> m_Pixels;
		Surface() = delete;
	};

	typedef std::unique_ptr<Surface> UniqueSurface;
}
