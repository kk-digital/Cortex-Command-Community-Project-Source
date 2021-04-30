#ifndef _RTERENDERLAYER_
#define _RTERENDERLAYER_

#include "SceneLayer.h"

#include "System/SDLTexture.h"

namespace RTE{

	/// <summary>
	/// A SceneLayer that contains a render Texture
	/// </summary>
	class RenderLayer: public SceneLayer{
	public:
		RenderLayer();
		~RenderLayer();

		int Create(ContentFile bitmapFile, bool drawTrans, Vector offset,
		           bool wrapX, bool wrapY, Vector scrollInfo) = delete;

		int Create(Texture &pTexture, bool drawTrans, Vector offset, bool wrapX,
		           bool wrapY, Vector scrollInfo) = delete;

		int Create(const SceneLayer &reference) = delete;

		int Create(int width, int height, Vector offset, bool wrapX, bool wrapY, Vector scrollInfo);

		int LoadData() {return -1;}

 		int SaveData(std::string path){return -1;}

		int ClearData(){return -1;}

		bool IsFileData() const { return false; }

		void Reset() {};

		void Destroy() = delete;

		size_t GetTextureHash() const = delete;

		void LockTexture() {}
		void UnlockTexture() {}

		uint32_t GetPixel(const int, const int);

		void SetPixel(const int, const int, const uint32_t) = delete;

	};
}

#endif
