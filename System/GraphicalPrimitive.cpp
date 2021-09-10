#include "GraphicalPrimitive.h"
#include "Matrix.h"
#include "FrameMan.h"
#include "SceneMan.h"

#include "GUI.h"
#include "SDLGUITexture.h"
#include "SDLHelper.h"
#include "SDL2/SDL2_gfxPrimitives.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GraphicalPrimitive::TranslateCoordinates(Vector targetPos, const Vector &scenePos, Vector &drawLeftPos, Vector &drawRightPos) const {
		drawLeftPos = scenePos;
		drawRightPos = scenePos;

		if (g_SceneMan.SceneWrapsX()) {
			float sceneWidth = static_cast<float>(g_SceneMan.GetSceneWidth());
			if (targetPos.m_X <= sceneWidth && targetPos.m_X > sceneWidth / 2) { targetPos.m_X -= sceneWidth; }
			drawLeftPos.m_X = (drawLeftPos.m_X > 0) ? (drawLeftPos.m_X -= sceneWidth) : (drawLeftPos.m_X -= sceneWidth + targetPos.m_X);
		}
		drawLeftPos.m_X -= targetPos.m_X;
		drawRightPos.m_X -= targetPos.m_X;

		if (g_SceneMan.SceneWrapsY()) {
			float sceneHeight = static_cast<float>(g_SceneMan.GetSceneHeight());
			if (targetPos.m_Y <= sceneHeight && targetPos.m_Y > sceneHeight / 2) { targetPos.m_Y -= sceneHeight; }
			drawLeftPos.m_Y = (drawLeftPos.m_Y > 0) ? (drawLeftPos.m_Y -= sceneHeight) : (drawLeftPos.m_Y -= sceneHeight + targetPos.m_Y);
		}
		drawLeftPos.m_Y -= targetPos.m_Y;
		drawRightPos.m_Y -= targetPos.m_Y;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LinePrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			Vector drawEnd = m_EndPos - targetPos;

			lineColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), drawEnd.GetFloorIntX(), drawEnd.GetFloorIntY(), m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawEndLeft;
			Vector drawStartRight;
			Vector drawEndRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);
			TranslateCoordinates(targetPos, m_EndPos, drawEndLeft, drawEndRight);

			lineColor(renderer, drawStartLeft.GetFloorIntX(),
			          drawStartLeft.GetFloorIntY(), drawEndLeft.GetFloorIntX(),
			          drawEndLeft.GetFloorIntY(), m_Color);
			lineColor(renderer, drawStartRight.GetFloorIntX(),
			          drawStartRight.GetFloorIntY(),
			          drawEndRight.GetFloorIntX(), drawEndRight.GetFloorIntY(),
			          m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ArcPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			if (m_Thickness > 1) {
				for (int i = 0; i < m_Thickness; i++) {
					arcColor(renderer, drawStart.GetFloorIntX(),
					         drawStart.GetFloorIntY(),
					         (m_Radius - (m_Thickness / 2)) + i, m_StartAngle,
					         m_EndAngle, m_Color);
				}
			} else {
				arcColor(renderer, drawStart.GetFloorIntX(), m_Radius,
				         drawStart.GetFloorIntY(), m_StartAngle, m_EndAngle,
				         m_Color);
			}
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			if (m_Thickness > 1) {
				for (int i = 0; i < m_Thickness; i++){
					arcColor(renderer, drawStartLeft.GetFloorIntX(),
					         drawStartLeft.GetFloorIntY(),
					         (m_Radius - (m_Thickness / 2)) + i, m_StartAngle,
					         m_EndAngle, m_Color);
					arcColor(renderer, drawStartRight.GetFloorIntX(),
					         drawStartRight.GetFloorIntY(),
					         (m_Radius - (m_Thickness / 2)) + i, m_StartAngle,
					         m_EndAngle, m_Color);
				}
			} else {
				arcColor(renderer, drawStartLeft.GetFloorIntX(),
				         drawStartLeft.GetFloorIntY(), m_Radius, m_StartAngle,
				         m_EndAngle, m_Color);
				arcColor(renderer, drawStartRight.GetFloorIntX(),
				    drawStartRight.GetFloorIntY(), m_Radius,
				    m_StartAngle,
				    m_EndAngle, m_Color);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SplinePrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			Vector drawGuideA = m_GuidePointAPos - targetPos;
			Vector drawGuideB = m_GuidePointBPos - targetPos;
			Vector drawEnd = m_EndPos - targetPos;

			std::array<Sint16, 4> guidePointsX = {
			    static_cast<Sint16>(drawStart.GetFloorIntX()), static_cast<Sint16>(drawGuideA.GetFloorIntX()),
			    static_cast<Sint16>(drawGuideB.GetFloorIntX()), static_cast<Sint16>(drawEnd.GetFloorIntX())};
			std::array<Sint16, 4> guidePointsY = {
			    static_cast<Sint16>(drawStart.GetFloorIntY()), static_cast<Sint16>(drawGuideA.GetFloorIntY()),
			    static_cast<Sint16>(drawGuideB.GetFloorIntY()), static_cast<Sint16>(drawEnd.GetFloorIntY())};
			bezierColor(renderer, guidePointsX.data(), guidePointsY.data(), 4, 16, m_Color); //TODO: proper interpolation!
		} else {
			Vector drawStartLeft;
			Vector drawGuideALeft;
			Vector drawGuideBLeft;
			Vector drawEndLeft;
			Vector drawStartRight;
			Vector drawGuideARight;
			Vector drawGuideBRight;
			Vector drawEndRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);
			TranslateCoordinates(targetPos, m_GuidePointAPos, drawGuideALeft, drawGuideARight);
			TranslateCoordinates(targetPos, m_GuidePointBPos, drawGuideBLeft, drawGuideBRight);
			TranslateCoordinates(targetPos, m_EndPos, drawEndLeft, drawEndRight);

			std::array<Sint16, 4> guidePointsLeftX{
			    static_cast<Sint16>(drawStartLeft.GetFloorIntX()),
				static_cast<Sint16>(drawGuideALeft.GetFloorIntX()),
				static_cast<Sint16>(drawGuideBLeft.GetFloorIntX()),
				static_cast<Sint16>(drawEndLeft.GetFloorIntX())
			};
			std::array<Sint16, 4> guidePointsLeftY{
			    static_cast<Sint16>(drawStartLeft.GetFloorIntY()),
			    static_cast<Sint16>(drawGuideALeft.GetFloorIntY()),
			    static_cast<Sint16>(drawGuideBLeft.GetFloorIntY()),
			    static_cast<Sint16>(drawEndLeft.GetFloorIntY())
			};
			std::array<Sint16, 8> guidePointsRightX = {
			    static_cast<Sint16>(drawStartRight.GetFloorIntX()),
			    static_cast<Sint16>(drawGuideARight.GetFloorIntX()),
			    static_cast<Sint16>(drawGuideBRight.GetFloorIntX()),
			    static_cast<Sint16>(drawEndRight.GetFloorIntX())
			};

			std::array<Sint16, 4> guidePointsRightY{
			    static_cast<Sint16>(drawStartRight.GetFloorIntY()),
			    static_cast<Sint16>(drawGuideARight.GetFloorIntY()),
			    static_cast<Sint16>(drawGuideBRight.GetFloorIntY()),
			    static_cast<Sint16>(drawEndRight.GetFloorIntY())
			};
			bezierColor(renderer, guidePointsLeftX.data(), guidePointsLeftY.data(), 4, 16, m_Color); //TODO: Interpolation
			bezierColor(renderer, guidePointsRightX.data(), guidePointsRightY.data(), 4, 16, m_Color); //TODO: Interpolation
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void BoxPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			Vector drawEnd = m_EndPos - targetPos;
			rectangleColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), drawEnd.GetFloorIntX(), drawEnd.GetFloorIntY(), m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawEndLeft;
			Vector drawStartRight;
			Vector drawEndRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);
			TranslateCoordinates(targetPos, m_EndPos, drawEndLeft, drawEndRight);

			rectangleColor(renderer, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), drawEndLeft.GetFloorIntX(), drawEndLeft.GetFloorIntY(), m_Color);
			rectangleColor(renderer, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), drawEndRight.GetFloorIntX(), drawEndRight.GetFloorIntY(), m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void BoxFillPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			Vector drawEnd = m_EndPos - targetPos;
			boxColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), drawEnd.GetFloorIntX(), drawEnd.GetFloorIntY(), m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawEndLeft;
			Vector drawStartRight;
			Vector drawEndRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);
			TranslateCoordinates(targetPos, m_EndPos, drawEndLeft, drawEndRight);

			boxColor(renderer, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), drawEndLeft.GetFloorIntX(), drawEndLeft.GetFloorIntY(), m_Color);
			boxColor(renderer, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), drawEndRight.GetFloorIntX(), drawEndRight.GetFloorIntY(), m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void RoundedBoxPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (m_StartPos.m_X > m_EndPos.m_X) { std::swap(m_StartPos.m_X, m_EndPos.m_X); }
		if (m_StartPos.m_Y > m_EndPos.m_Y) { std::swap(m_StartPos.m_Y, m_EndPos.m_Y); }

		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			Vector drawEnd = m_EndPos - targetPos;

			roundedRectangleColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), drawEnd.GetFloorIntX(), drawEnd.GetFloorIntY(), m_CornerRadius, m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawEndLeft;
			Vector drawStartRight;
			Vector drawEndRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);
			TranslateCoordinates(targetPos, m_EndPos, drawEndLeft, drawEndRight);

			roundedRectangleColor(
			    renderer, drawStartLeft.GetFloorIntX(),
			    drawStartLeft.GetFloorIntY(), drawEndLeft.GetFloorIntX(),
			    drawEndLeft.GetFloorIntY(), m_CornerRadius, m_Color);

			roundedRectangleColor(
			    renderer, drawStartRight.GetFloorIntX(),
			    drawStartRight.GetFloorIntY(), drawEndRight.GetFloorIntX(),
			    drawEndRight.GetFloorIntY(), m_CornerRadius, m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void RoundedBoxFillPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (m_StartPos.m_X > m_EndPos.m_X) { std::swap(m_StartPos.m_X, m_EndPos.m_X); }
		if (m_StartPos.m_Y > m_EndPos.m_Y) { std::swap(m_StartPos.m_Y, m_EndPos.m_Y); }

		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			Vector drawEnd = m_EndPos - targetPos;

			roundedBoxColor(
			    renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(),
			    drawEnd.GetFloorIntX(), drawEnd.GetFloorIntY(), m_CornerRadius,
			    m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawEndLeft;
			Vector drawStartRight;
			Vector drawEndRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);
			TranslateCoordinates(targetPos, m_EndPos, drawEndLeft, drawEndRight);

			roundedBoxColor(
			    renderer, drawStartLeft.GetFloorIntX(),
			    drawStartLeft.GetFloorIntY(), drawEndLeft.GetFloorIntX(),
			    drawEndLeft.GetFloorIntY(), m_CornerRadius, m_Color);

			roundedBoxColor(
			    renderer, drawStartRight.GetFloorIntX(),
			    drawStartRight.GetFloorIntY(), drawEndRight.GetFloorIntX(),
			    drawEndRight.GetFloorIntY(), m_CornerRadius, m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CirclePrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			circleColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), m_Radius, m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			circleColor(renderer, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), m_Radius, m_Color);
			circleColor(renderer, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), m_Radius, m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CircleFillPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			filledCircleColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), m_Radius, m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			filledCircleColor(renderer, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), m_Radius, m_Color);
			filledCircleColor(renderer, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), m_Radius, m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EllipsePrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			ellipseColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), m_HorizRadius, m_VertRadius, m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			ellipseColor(renderer, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), m_HorizRadius, m_VertRadius, m_Color);
			ellipseColor(renderer, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), m_HorizRadius, m_VertRadius, m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EllipseFillPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			filledEllipseColor(renderer, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), m_HorizRadius, m_VertRadius, m_Color);
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			filledEllipseColor(renderer, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), m_HorizRadius, m_VertRadius, m_Color);
			filledEllipseColor(renderer, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), m_HorizRadius, m_VertRadius, m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TrianglePrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawPointA = m_PointAPos - targetPos;
			Vector drawPointB = m_PointBPos - targetPos;
			Vector drawPointC = m_PointCPos - targetPos;
			trigonColor(renderer,
						drawPointA.GetFloorIntX(), drawPointA.GetFloorIntY(),
						drawPointB.GetFloorIntX(), drawPointA.GetFloorIntY(),
						drawPointC.GetFloorIntX(), drawPointC.GetFloorIntY(),
						m_Color
						);
		} else {
			Vector drawPointALeft;
			Vector drawPointBLeft;
			Vector drawPointCLeft;
			Vector drawPointARight;
			Vector drawPointBRight;
			Vector drawPointCRight;

			TranslateCoordinates(targetPos, m_PointAPos, drawPointALeft, drawPointARight);
			TranslateCoordinates(targetPos, m_PointBPos, drawPointBLeft, drawPointBRight);
			TranslateCoordinates(targetPos, m_PointCPos, drawPointCLeft, drawPointCRight);

			trigonColor(renderer, drawPointALeft.GetFloorIntX(),
			            drawPointALeft.GetFloorIntY(), drawPointBLeft.GetFloorIntX(),
			            drawPointALeft.GetFloorIntY(), drawPointCLeft.GetFloorIntX(),
			            drawPointCLeft.GetFloorIntY(), m_Color);
			trigonColor(
			    renderer, drawPointARight.GetFloorIntX(),
			    drawPointARight.GetFloorIntY(), drawPointBRight.GetFloorIntX(),
			    drawPointARight.GetFloorIntY(), drawPointCRight.GetFloorIntX(),
			    drawPointCRight.GetFloorIntY(), m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TriangleFillPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawPointA = m_PointAPos - targetPos;
			Vector drawPointB = m_PointBPos - targetPos;
			Vector drawPointC = m_PointCPos - targetPos;
			filledTrigonColor(renderer, drawPointA.GetFloorIntX(), drawPointA.GetFloorIntY(), drawPointB.GetFloorIntX(), drawPointB.GetFloorIntY(), drawPointC.GetFloorIntX(), drawPointC.GetFloorIntY(), m_Color);
		} else {
			Vector drawPointALeft;
			Vector drawPointBLeft;
			Vector drawPointCLeft;
			Vector drawPointARight;
			Vector drawPointBRight;
			Vector drawPointCRight;

			TranslateCoordinates(targetPos, m_PointAPos, drawPointALeft, drawPointARight);
			TranslateCoordinates(targetPos, m_PointBPos, drawPointBLeft, drawPointBRight);
			TranslateCoordinates(targetPos, m_PointCPos, drawPointCLeft, drawPointCRight);

			filledTrigonColor(renderer, drawPointALeft.GetFloorIntX(), drawPointALeft.GetFloorIntY(), drawPointBLeft.GetFloorIntX(), drawPointBLeft.GetFloorIntY(), drawPointCLeft.GetFloorIntX(), drawPointCLeft.GetFloorIntY(), m_Color);
			filledTrigonColor(renderer, drawPointARight.GetFloorIntX(), drawPointARight.GetFloorIntY(), drawPointBRight.GetFloorIntX(), drawPointBRight.GetFloorIntY(), drawPointCRight.GetFloorIntX(), drawPointCRight.GetFloorIntY(), m_Color);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TextPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			SDLGUITexture playerGUIBitmap;

			if (m_IsSmall) {
				g_FrameMan.GetSmallFont()->DrawAligned(&playerGUIBitmap, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), m_Text, m_Alignment);
			} else {
				g_FrameMan.GetLargeFont()->DrawAligned(&playerGUIBitmap, drawStart.GetFloorIntX(), drawStart.GetFloorIntY(), m_Text, m_Alignment);
			}
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			SDLGUITexture playerGUIBitmap;
			if (m_IsSmall) {
				g_FrameMan.GetSmallFont()->DrawAligned(&playerGUIBitmap, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), m_Text, m_Alignment);
				g_FrameMan.GetSmallFont()->DrawAligned(&playerGUIBitmap, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), m_Text, m_Alignment);
			} else {
				g_FrameMan.GetLargeFont()->DrawAligned(&playerGUIBitmap, drawStartLeft.GetFloorIntX(), drawStartLeft.GetFloorIntY(), m_Text, m_Alignment);
				g_FrameMan.GetLargeFont()->DrawAligned(&playerGUIBitmap, drawStartRight.GetFloorIntX(), drawStartRight.GetFloorIntY(), m_Text, m_Alignment);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void BitmapPrimitive::Draw(SDL_Renderer* renderer, const Vector &targetPos) {
		if (!m_Texture) {
			return;
		}

		int flip = m_HFlipped? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		flip |= m_VFlipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			m_Texture->render(renderer, drawStart.m_X, drawStart.m_Y, m_RotAngle, flip);
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			m_Texture->render(renderer, drawStartLeft.m_X, drawStartLeft.m_Y, m_RotAngle, flip);

			m_Texture->render(renderer, drawStartRight.m_X, drawStartLeft.m_Y, m_RotAngle, flip);
		}
	}
}
