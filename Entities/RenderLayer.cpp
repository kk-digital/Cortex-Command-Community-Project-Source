#include "RenderLayer.h"

#include "Managers/FrameMan.h"
#include "System/SDLHelper.h"

namespace RTE {

	RenderLayer::RenderLayer(){
	}
	RenderLayer::~RenderLayer() = default;

	int RenderLayer::Create(int width, int height, Vector offset, bool wrapX,
	                        bool wrapY, Vector scrollInfo) {
		m_DrawTrans = true;
		m_WrapX = wrapX;
		m_WrapY = wrapY;
		m_ScrollInfo = scrollInfo;
		m_Offset = offset;

		m_pMainTexture = std::make_shared<Texture>(g_FrameMan.GetRenderer(), width, height);

		if (m_WrapX)
			m_ScrollRatio.m_X = m_ScrollInfo.m_X;
		else if (m_ScrollInfo.m_X == -1.0 || m_ScrollInfo.m_X == 1.0)
			m_ScrollRatio.m_X = 1.0;
		else if (m_ScrollInfo.m_X == g_FrameMan.GetPlayerScreenWidth())
			m_ScrollRatio.m_X =
			    m_pMainTexture->getW() - g_FrameMan.GetPlayerScreenWidth();
		else if (m_pMainTexture->getW() == g_FrameMan.GetPlayerScreenWidth())
			m_ScrollRatio.m_X =
			    1.0f /
			    (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerScreenWidth());
		else
			m_ScrollRatio.m_X =
			    (float)(m_pMainTexture->getW() -
			            g_FrameMan.GetPlayerScreenWidth()) /
			    (float)(m_ScrollInfo.m_X - g_FrameMan.GetPlayerScreenWidth());

		if (m_WrapY)
			m_ScrollRatio.m_Y = m_ScrollInfo.m_Y;
		else if (m_ScrollInfo.m_Y == -1.0 || m_ScrollInfo.m_Y == 1.0)
			m_ScrollRatio.m_Y = 1.0;
		else if (m_ScrollInfo.m_Y == g_FrameMan.GetPlayerScreenHeight())
			m_ScrollRatio.m_Y =
			    m_pMainTexture->getH() - g_FrameMan.GetPlayerScreenHeight();
		else if (m_pMainTexture->getH() == g_FrameMan.GetPlayerScreenHeight())
			m_ScrollRatio.m_Y =
			    1.0f /
			    (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerScreenHeight());
		else
			m_ScrollRatio.m_Y =
			    (float)(m_pMainTexture->getH() -
			            g_FrameMan.GetPlayerScreenHeight()) /
			    (float)(m_ScrollInfo.m_Y - g_FrameMan.GetPlayerScreenHeight());

		// Establish the scaled dimensions of this
		m_ScaledDimensions.SetXY(m_pMainTexture->getW() * m_ScaleFactor.m_X,
		                         m_pMainTexture->getH() * m_ScaleFactor.m_Y);

		m_FillLeftColor = g_AlphaZero;

		m_FillRightColor = g_AlphaZero;

		m_FillUpColor = g_AlphaZero;

		m_FillDownColor = g_AlphaZero;

		return 0;
	}

	uint32_t RenderLayer::GetPixel(const int x, const int y){
		uint32_t color;
		SDL_Rect pos {x,y,1,1};
		SDL_RenderReadPixels(g_FrameMan.GetRenderer(), &pos, SDL_PIXELFORMAT_RGBA32, &color, sizeof(uint32_t));
		return color;
	}

} // namespace RTE
