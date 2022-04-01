#include "GUI.h"
#include "SDLGUITexture.h"

#include "RenderTexture.h"
#include "GL/glew.h"

#include "GraphicalPrimitive.h"
#include "Color.h"
#include "FrameMan.h"

namespace RTE {
	int SDLGUITexture::guicount = 0;
	SDLGUITexture::SDLGUITexture(std::shared_ptr<GLTexture> pTexture) : m_ServerUpdated(false), m_ClientUpdated(false) {
		assert(pTexture);
		m_Texture = pTexture;
		m_ClipRect = nullptr;

		m_Width = pTexture->GetW();
		m_Height = pTexture->GetH();
		guicount++;
	}

	SDLGUITexture::SDLGUITexture(int width, int height) : m_ServerUpdated(false), m_ClientUpdated(false) {
		m_Texture = MakeTexture();
		m_Texture->Create(width, height);

		m_Width = width;
		m_Height = height;
		m_ClipRect = nullptr;
		guicount++;
	}

	SDLGUITexture::SDLGUITexture(const SDLGUITexture &ref) {
		m_TextureFile = ref.m_TextureFile;
		m_Texture = ref.m_Texture;
		m_ColorKey = ref.m_ColorKey;
		m_Width = ref.m_Width;
		m_Height = ref.m_Height;
		m_ClipRect = std::make_unique<GUIRect>();
		SetRect(m_ClipRect.get(), 0, 0, m_Width, m_Height);
	}

	SDLGUITexture::SDLGUITexture(const std::string &filename) { Create(filename); }

	SDLGUITexture::~SDLGUITexture() {
		guicount--;
	};

	void SDLGUITexture::SetBitmap(std::shared_ptr<GLTexture> newTexture) {
		assert(newTexture);
		m_Texture = newTexture;
		m_Width = m_Texture->GetW();
		m_Height = m_Texture->GetH();
		m_ClipRect = nullptr;
		m_ColorKey = 0;
	}

	bool SDLGUITexture::Create(const std::string &filename) {
		m_TextureFile.SetDataPath(filename);
		m_Texture = m_TextureFile.GetAsTexture();
		m_Width = m_Texture->GetW();
		m_Height = m_Texture->GetH();
		return m_Texture.get();
	}

	bool SDLGUITexture::Create(int width, int height, int) {
		m_Width = width;
		m_Height = height;
		m_Texture = MakeTexture();
		m_Texture->Create(width, height);

		return true;
	}

	void SDLGUITexture::Destroy() {
		m_Texture = nullptr;
		m_Width = -1;
		m_Height = -1;
		m_ColorKey = 0;
		m_TextureFile.Reset();
	}

	void SDLGUITexture::Draw(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect) {
		SDLGUITexture *dest = dynamic_cast<SDLGUITexture *>(pDestBitmap);
		assert(dest);
		assert(m_Texture && dest->m_Texture);

		RenderTexture tempTarget;
		tempTarget.Create(dest->m_Texture);
		dest->m_ServerUpdated = true;

		if (m_ClientUpdated) {
			m_Texture->Upload();
		}

		if (dest->m_ClientUpdated) {
			dest->m_Texture->Upload();
		}

		if (dest->m_ClipRect) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(dest->m_ClipRect->x, dest->m_ClipRect->y, dest->m_ClipRect->w, dest->m_ClipRect->h);
		}

		if (pRect) {
			m_Texture->render(&tempTarget, {pRect->x, pRect->y, pRect->w, pRect->h}, {x, y, pRect->w, pRect->h});
		} else {
			m_Texture->render(&tempTarget, x, y);
		}

		if (dest->m_ClipRect) {
			glDisable(GL_SCISSOR_TEST);
		}
	}

	void SDLGUITexture::DrawTrans(GUIBitmap *pDestBitmap, int x, int y, GUIRect *pRect) {
		Draw(pDestBitmap, x, y, pRect);
	}

	void SDLGUITexture::DrawTransScaled(GUIBitmap *pDestBitmap, int x, int y, int width, int height) {
		SDLGUITexture *dest = dynamic_cast<SDLGUITexture *>(pDestBitmap);
		assert(dest);
		assert(m_Texture && dest->m_Texture);

		RenderTexture tempTarget;
		tempTarget.Create(dest->m_Texture);
		dest->m_ServerUpdated = true;

		if (m_ClientUpdated) {
			m_Texture->Upload();
		}

		if (dest->m_ClientUpdated) {
			dest->m_Texture->Upload();
		}

		if (dest->m_ClipRect) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(dest->m_ClipRect->x, dest->m_ClipRect->y, dest->m_ClipRect->w, dest->m_ClipRect->h);
		}

		m_Texture->render(&tempTarget, {x, y, width, height});

		if (dest->m_ClipRect) {
			glDisable(GL_SCISSOR_TEST);
		}
	}

	void SDLGUITexture::DrawLine(int x1, int y1, int x2, int y2, unsigned long color) {
		glm::vec4 c{((color << 24) & 0xff) / 255.f,
			((color << 16) & 0xff) / 255.f,
			((color << 8) & 0xff) / 255.f,
			((color)&0xff) / 255.f};

		RenderState overrideColor;
		overrideColor.m_Color = c;
		overrideColor.m_BlendMode = BlendModes::None;
		overrideColor.m_Shader = g_FrameMan.GetColorShader();

		RenderTexture tempTarget;
		tempTarget.SetTexture(m_Texture);

		LinePrimitive(-1, Vector(x1, y1), Vector(x2, y2), 0).Draw(&tempTarget, {0,0}, overrideColor);
	}

	void SDLGUITexture::DrawRectangle(int x, int y, int width, int height, unsigned long color, bool filled) {
		glm::vec4 c{((color << 24) & 0xff) / 255.f,
		    ((color << 16) & 0xff) / 255.f,
		    ((color << 8) & 0xff) / 255.f,
		    ((color)&0xff) / 255.f};

		RenderState overrideColor;
		overrideColor.m_Color = c;
		overrideColor.m_BlendMode = BlendModes::None;
		overrideColor.m_Shader = g_FrameMan.GetColorShader();

		RenderTexture tempTarget;
		tempTarget.SetTexture(m_Texture);

		BoxPrimitive(-1, Vector(x, y), Vector(x + width, y + height), 0).Draw(&tempTarget, {0, 0}, overrideColor);
	}

	unsigned long SDLGUITexture::GetPixel(int x, int y) {
		if (m_ServerUpdated) {
			m_Texture->Download();
			m_ServerUpdated = false;
		}
		return m_Texture->GetPixel(x, y);
	}

	void SDLGUITexture::SetPixel(int x, int y, unsigned long color) {
		if (m_ServerUpdated) {
			m_Texture->Download();
			m_ServerUpdated = false;
		}
		m_Texture->SetPixel(x, y, color);
		m_ClientUpdated = true;
	}

	void SDLGUITexture::GetClipRect(GUIRect *rect) const {
		if (!m_ClipRect) {
			rect->w = m_Width;
			rect->h = m_Height;
			rect->x = 0;
			rect->y = 0;
		}

		rect->x = m_ClipRect->x;
		rect->y = m_ClipRect->y;
		rect->w = m_ClipRect->w;
		rect->h = m_ClipRect->h;
	}

	void SDLGUITexture::SetClipRect(GUIRect *rect) {
		if (!rect) {
			m_ClipRect = nullptr;
			return;
		}
		if (!m_ClipRect) {
			m_ClipRect = std::make_unique<GUIRect>();
		}

		SetRect(m_ClipRect.get(), rect->x, rect->y, rect->w, rect->h);
	}

	void SDLGUITexture::AddClipRect(GUIRect *rect) {
		if (!rect) {
			m_ClipRect = nullptr;
			return;
		}
		if (!m_ClipRect) {
			m_ClipRect = std::make_unique<GUIRect>(*rect);
		}

		int tempLeft = std::max(m_ClipRect->x, rect->x);
		int tempTop = std::max(m_ClipRect->y, rect->y);
		int tempRight = std::min(tempLeft + m_ClipRect->w, tempLeft + rect->w);
		int tempBottom = std::min(tempTop + m_ClipRect->h, tempTop + rect->w);

		m_ClipRect->x = tempLeft;
		m_ClipRect->y = tempTop;
		m_ClipRect->w = tempRight - tempLeft;
		m_ClipRect->h = tempBottom - tempTop;
	}

} // namespace RTE
