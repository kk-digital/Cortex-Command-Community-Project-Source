#include "GraphicalPrimitive.h"
#include "Matrix.h"
#include "FrameMan.h"
#include "SceneMan.h"
#include "RTERenderer.h"
#include "glm/gtc/matrix_transform.hpp"

#include "GUI.h"
#include "SDLGUITexture.h"

#include "SDLHelper.h"

namespace RTE {

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GraphicalPrimitive::TranslateCoordinates(Vector targetPos, const Vector &scenePos, Vector &drawLeftPos, Vector &drawRightPos) const {
		drawLeftPos = scenePos;
		drawRightPos = scenePos;

		if (g_SceneMan.SceneWrapsX()) {
			float sceneWidth = static_cast<float>(g_SceneMan.GetSceneWidth());
			if (targetPos.m_X <= sceneWidth && targetPos.m_X > sceneWidth / 2) {
				targetPos.m_X -= sceneWidth;
			}
			drawLeftPos.m_X = (drawLeftPos.m_X > 0) ? (drawLeftPos.m_X -= sceneWidth) : (drawLeftPos.m_X -= sceneWidth + targetPos.m_X);
		}
		drawLeftPos.m_X -= targetPos.m_X;
		drawRightPos.m_X -= targetPos.m_X;

		if (g_SceneMan.SceneWrapsY()) {
			float sceneHeight = static_cast<float>(g_SceneMan.GetSceneHeight());
			if (targetPos.m_Y <= sceneHeight && targetPos.m_Y > sceneHeight / 2) {
				targetPos.m_Y -= sceneHeight;
			}
			drawLeftPos.m_Y = (drawLeftPos.m_Y > 0) ? (drawLeftPos.m_Y -= sceneHeight) : (drawLeftPos.m_Y -= sceneHeight + targetPos.m_Y);
		}
		drawLeftPos.m_Y -= targetPos.m_Y;
		drawRightPos.m_Y -= targetPos.m_Y;
	}

	void GraphicalPrimitive::Draw(RenderTarget *renderer, const Vector &targetPos, std::optional<RenderState> renderState) {
		RenderState state{};
		if (renderState) {
			state = *renderState;
		} else {
			state.m_Shader = g_FrameMan.GetColorShader();
			state.m_Color = g_FrameMan.GetDefaultPalette()->at(m_Color);
			state.m_Color /= 255.0f;
		}

		state.m_ModelTransform = glm::mat4(1);
		// glm::translate(state.m_ModelTransform, glm::vec3(static_cast<glm::vec2>(targetPos), 0));
		state.m_PrimitiveType = m_DrawType;
		state.m_Vertices = m_Vertices;

		renderer->Draw(state);
	}

	PointPrimitive::PointPrimitive(int player, const Vector &position, unsigned long color) {
		m_Vertices = std::make_shared<VertexArray>(std::vector<Vertex>{{static_cast<glm::vec2>(m_StartPos)}});
		m_Color = color;
		m_StartPos = position;
		m_Player = player;
		m_DrawType = PrimitiveType::Point;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LinePrimitive::LinePrimitive(int player, const Vector &startPos, const Vector &endPos, unsigned long color) {
		m_StartPos = startPos;
		m_EndPos = endPos;
		m_Player = player;
		m_Color = color;
		m_DrawType = PrimitiveType::Line;

		std::vector<Vertex> vertices;
		vertices.emplace_back(m_StartPos);
		vertices.emplace_back(m_EndPos);
		m_Vertices = std::make_shared<VertexArray>(vertices);
	}
#if 0
	void LinePrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ArcPrimitive::ArcPrimitive(int player, const Vector &centerPos, float startAngle, float endAngle, int radius, int thickness, unsigned long color) {
		m_Color = color;
		m_StartPos = centerPos;
		m_Player = player;

		m_DrawType = thickness > 1 ? PrimitiveType::TriangleStrip : PrimitiveType::LineStrip;

		std::vector<Vertex> vertices;
		int arcResolution{10 * radius / 4};
		if (arcResolution == 0) {
			++arcResolution;
		}

		if (thickness > 1 && radius > 1) {
			arcResolution *= 2;
		}

		float angleStep = (endAngle - startAngle) / arcResolution;

		if (thickness > radius)
			thickness = radius;

		bool outerLine = true;
		for (int i = 0; i < arcResolution; ++i) {
			glm::vec2 position;
			if (outerLine) {
				position = {radius * glm::cos(startAngle + i * angleStep), radius * glm::sin(startAngle + i * angleStep)};
			} else {
				position = {(radius - thickness) * glm::cos(startAngle + i * angleStep), (radius - thickness) * glm::sin(startAngle + i * angleStep)};
			}

			vertices.emplace_back(position + centerPos);
			if (thickness > 1)
				outerLine = !outerLine;
		}
		m_Vertices = std::make_shared<VertexArray>(vertices);
	}

#if 0
	void ArcPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SplinePrimitive::SplinePrimitive(int player, const Vector &startPos, const Vector &guideA, const Vector &guideB, const Vector &endPos, unsigned long color) {
		m_Player = player;
		m_StartPos = startPos;
		m_EndPos = endPos;
		m_Color = color;
		m_Vertices = std::make_shared<VertexArray>(std::vector<Vertex>{});
		m_DrawType = PrimitiveType::Point;
	}

#if 0
	void SplinePrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BoxPrimitive::BoxPrimitive(int player, const Vector &topLeftPos, const Vector &bottomRightPos, unsigned long color) {
		m_Player = player;
		m_StartPos = topLeftPos;
		m_EndPos = bottomRightPos;
		m_Color = color;

		std::vector<Vertex> quad;

		quad.emplace_back(topLeftPos);
		quad.emplace_back(glm::vec2(topLeftPos.m_X, bottomRightPos.m_Y));
		quad.emplace_back(bottomRightPos);
		quad.emplace_back(glm::vec2(bottomRightPos.m_X, topLeftPos.m_Y));

		m_DrawType = PrimitiveType::LineLoop;
		m_Vertices = std::make_shared<VertexArray>(quad);
	}
#if 0
	void BoxPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BoxFillPrimitive::BoxFillPrimitive(int player, const Vector &topLeftPos, const Vector &bottomRightPos, unsigned long color) {
		m_Player = player;
		m_StartPos = topLeftPos;
		m_EndPos = bottomRightPos;
		m_Color = color;

		std::vector<Vertex> quad;

		quad.emplace_back(topLeftPos);
		quad.emplace_back(glm::vec2(topLeftPos.m_X, bottomRightPos.m_Y));
		quad.emplace_back(glm::vec2(bottomRightPos.m_X, topLeftPos.m_Y));
		quad.emplace_back(bottomRightPos);

		m_DrawType = PrimitiveType::TriangleStrip;
		m_Vertices = std::make_shared<VertexArray>(quad);
	}

#if 0
	void BoxFillPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RoundedBoxPrimitive::RoundedBoxPrimitive(int player, const Vector &topLeftPos, const Vector &bottomRightPos, int cornerRadius, unsigned long color) {
		m_Player = player;
		m_StartPos = topLeftPos;
		m_EndPos = bottomRightPos;
		m_Color = color;
		m_DrawType = PrimitiveType::LineLoop;

		std::vector<Vertex> vertices;

		vertices.emplace_back(topLeftPos + glm::vec2(cornerRadius, 0));
		int arcResolution{cornerRadius / 6};

		float startAngle = glm::pi<float>() / 2;

		auto arcVertices = [](auto startAngle, auto endAngle, auto resolution, auto radius, auto center, auto &vertices) {
			float angleStep = (endAngle - startAngle) / resolution;
			for (int i = 0; i < resolution; ++i) {
				glm::vec2 arc(radius * glm::cos(startAngle + i * angleStep), radius * glm::sin(startAngle + i * angleStep));

				vertices.emplace_back(arc + center);
			}
		};
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, topLeftPos + glm::vec2(cornerRadius), vertices);
		vertices.emplace_back(glm::vec2(bottomRightPos.m_X - cornerRadius, topLeftPos.m_Y));
		startAngle += glm::pi<float>();
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, glm::vec2(topLeftPos.m_X, bottomRightPos.m_Y) + glm::vec2(cornerRadius, -cornerRadius), vertices);
		startAngle += glm::pi<float>();
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, bottomRightPos + glm::vec2(-cornerRadius), vertices);
		startAngle += glm::pi<float>();
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, glm::vec2(bottomRightPos.m_X, topLeftPos.m_Y) + glm::vec2(-cornerRadius, cornerRadius), vertices);

		m_Vertices = std::make_shared<VertexArray>(vertices);
	}

#if 0
	void RoundedBoxPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RoundedBoxFillPrimitive::RoundedBoxFillPrimitive(int player, const Vector &topLeftPos, const Vector &bottomRightPos, int cornerRadius, unsigned long color) {
		m_Player = player;
		m_StartPos = topLeftPos;
		m_EndPos = bottomRightPos;
		m_Color = color;
		m_DrawType = PrimitiveType::TriangleFan;

		std::vector<Vertex> vertices;

		vertices.emplace_back(topLeftPos + 0.5f * static_cast<glm::vec2>(bottomRightPos - topLeftPos));
		vertices.emplace_back(topLeftPos + glm::vec2(cornerRadius, 0));
		int arcResolution{cornerRadius / 6};

		float startAngle = glm::pi<float>() / 2;

		auto arcVertices = [](auto startAngle, auto endAngle, auto resolution, auto radius, auto center, auto &vertices) {
			float angleStep = (endAngle - startAngle) / resolution;
			for (int i = 0; i < resolution; ++i) {
				glm::vec2 arc(radius * glm::cos(startAngle + i * angleStep), radius * glm::sin(startAngle + i * angleStep));

				vertices.emplace_back(arc + center);
			}
		};
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, topLeftPos + glm::vec2(cornerRadius), vertices);
		vertices.emplace_back(glm::vec2(bottomRightPos.m_X - cornerRadius, topLeftPos.m_Y));
		startAngle += glm::pi<float>();
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, glm::vec2(topLeftPos.m_X, bottomRightPos.m_Y) + glm::vec2(cornerRadius, -cornerRadius), vertices);
		startAngle += glm::pi<float>();
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, bottomRightPos + glm::vec2(-cornerRadius), vertices);
		startAngle += glm::pi<float>();
		arcVertices(startAngle, startAngle + glm::pi<float>(), arcResolution, cornerRadius, glm::vec2(bottomRightPos.m_X, topLeftPos.m_Y) + glm::vec2(-cornerRadius, cornerRadius), vertices);

		m_Vertices = std::make_shared<VertexArray>(vertices);
	}

#if 0
	void RoundedBoxFillPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CirclePrimitive::CirclePrimitive(int player, const Vector &centerPos, int radius, unsigned long color) {
		m_StartPos = centerPos;
		m_Player = player;
		m_Color = color;
		m_DrawType = PrimitiveType::LineLoop;

		std::vector<Vertex> vertices;

		int resolution{std::min(radius, 20)};

		float angleStep{2 * glm::pi<float>() / resolution};

		for (int i = 0; i < resolution; ++i) {
			glm::vec2 point(radius * glm::cos(i * angleStep), radius * glm::sin(i * angleStep));
			vertices.emplace_back(point + centerPos);
		}

		m_Vertices = std::make_shared<VertexArray>(vertices);
	}
#if 0
	void CirclePrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CircleFillPrimitive::CircleFillPrimitive(int player, const Vector &centerPos, int radius, unsigned long color) {
		m_StartPos = centerPos;
		m_Player = player;
		m_Color = color;
		m_DrawType = PrimitiveType::TriangleFan;

		std::vector<Vertex> vertices;
		vertices.emplace_back(centerPos);

		int resolution{std::max(radius, 20)};
		float angleStep{2 * glm::pi<float>() / resolution};

		for (int i = 0; i <= resolution; ++i) {
			glm::vec2 point(radius * glm::cos(i * angleStep), radius * glm::sin(i * angleStep));
			vertices.emplace_back(point + centerPos);
		}
		m_Vertices = std::make_shared<VertexArray>(vertices);
	}

#if 0
	void CircleFillPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	EllipsePrimitive::EllipsePrimitive(int player, const Vector &centerPos, int horizRadius, int vertRadius, unsigned long color) {
		m_StartPos = centerPos;
		m_Player = player;
		m_Color = color;
		m_DrawType = PrimitiveType::LineLoop;

		int resolution{(horizRadius + vertRadius) / 20};
		float angleStep = 2 * glm::pi<float>() / resolution;

		std::vector<Vertex> vertices;

		for (int i = 0; i < resolution; ++i) {
			glm::vec2 point(horizRadius * glm::cos(i * angleStep), vertRadius * glm::sin(i * angleStep));

			vertices.emplace_back(point + centerPos);
		}

		m_Vertices = std::make_shared<VertexArray>(vertices);
	}

#if 0
	void EllipsePrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EllipseFillPrimitive::EllipseFillPrimitive(int player, const Vector &centerPos, int horizRadius, int vertRadius, unsigned long color) {
		m_StartPos = centerPos;
		m_Player = player;
		m_Color = color;
		m_DrawType = PrimitiveType::TriangleFan;

		int resolution{(horizRadius + vertRadius) / 20};
		float angleStep = 2 * glm::pi<float>() / resolution;

		std::vector<Vertex> vertices;
		vertices.emplace_back(centerPos);

		for (int i = 0; i < resolution; ++i) {
			glm::vec2 point(horizRadius * glm::cos(i * angleStep), vertRadius * glm::sin(i * angleStep));

			vertices.emplace_back(point + centerPos);
		}

		m_Vertices = std::make_shared<VertexArray>(vertices);
	}

#if 0
	void EllipseFillPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TrianglePrimitive::TrianglePrimitive(int player, const Vector &pointA, const Vector &pointB, const Vector &pointC, unsigned long color) {
		m_Player = player;
		m_Color = color;
		m_StartPos = pointA;
		m_EndPos = pointC;
		m_DrawType = PrimitiveType::LineLoop;

		std::vector<Vertex> tri;
		tri.emplace_back(pointA);
		tri.emplace_back(pointB);
		tri.emplace_back(pointC);

		m_Vertices = std::make_shared<VertexArray>(tri);
	}
#if 0
	void TrianglePrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TriangleFillPrimitive::TriangleFillPrimitive(int player, const Vector &pointA, const Vector &pointB, const Vector &pointC, unsigned long color) {
		m_Player = player;
		m_Color = color;
		m_StartPos = pointA;
		m_EndPos = pointC;
		m_DrawType = PrimitiveType::Triangle;

		std::vector<Vertex> tri;
		tri.emplace_back(pointA);
		tri.emplace_back(pointB);
		tri.emplace_back(pointC);

		m_Vertices = std::make_shared<VertexArray>(tri);
	}

#if 0
	void TriangleFillPrimitive::Draw(RenderTarget* renderer, const Vector &targetPos) {
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
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TextPrimitive::Draw(RenderTarget *renderer, const Vector &targetPos, std::optional<RenderState> renderState) {
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

	void BitmapPrimitive::Draw(RenderTarget *renderer, const Vector &targetPos, std::optional<RenderState> renderState) {
		if (!m_Texture) {
			return;
		}

		glm::vec2 flip(m_HFlipped ? -1 : 1, m_VFlipped ? -1 : 1);

		if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) {
			Vector drawStart = m_StartPos - targetPos;
			m_Texture->render(renderer, drawStart, m_RotAngle, flip);
		} else {
			Vector drawStartLeft;
			Vector drawStartRight;

			TranslateCoordinates(targetPos, m_StartPos, drawStartLeft, drawStartRight);

			m_Texture->render(renderer, drawStartLeft, m_RotAngle, flip);

			m_Texture->render(renderer, drawStartRight, m_RotAngle, flip);
		}
	}
} // namespace RTE
