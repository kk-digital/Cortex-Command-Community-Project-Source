#include "SLBackground.h"
#include "FrameMan.h"
#include "SceneMan.h"
#include "SettingsMan.h"

namespace RTE {

	ConcreteClassInfo(SLBackground, SceneLayer, 0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SLBackground::Clear() {
		m_Bitmaps.clear();
		m_FrameCount = 1;
		m_Frame = 0;
		m_SpriteAnimMode = SpriteAnimMode::NOANIM;
		m_SpriteAnimDuration = 1000;
		m_SpriteAnimIsReversingFrames = false;
		m_SpriteAnimTimer.Reset();
		m_IsAnimatedManually = false;
		m_AutoScrollX = false;
		m_AutoScrollY = false;
		m_AutoScrollStep.Reset();
		m_AutoScrollStepInterval = 0;
		m_AutoScrollStepTimer.Reset();
		m_AutoScrollOffset.Reset();
		m_FillColorLeft = ColorKeys::g_MaskColor;
		m_FillColorRight = ColorKeys::g_MaskColor;
		m_FillColorUp = ColorKeys::g_MaskColor;
		m_FillColorDown = ColorKeys::g_MaskColor;

		m_LayerScaleFactors = { Vector(1.0F, 1.0F), Vector(1.0F, 1.0F), Vector(2.0F, 2.0F) };
		m_IgnoreAutoScale = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int SLBackground::Create() {
		SceneLayer::Create();

		m_BitmapFile.GetAsAnimation(m_Bitmaps, m_FrameCount);
		m_MainBitmap = m_Bitmaps.at(0);

		// Sampled color at the edges of the layer that can be used to fill gap if the layer isn't large enough to cover a target bitmap.
		if (!m_WrapX) {
			m_FillColorLeft = _getpixel(m_MainBitmap, 0, m_MainBitmap->h / 2);
			m_FillColorRight = _getpixel(m_MainBitmap, m_MainBitmap->w - 1, m_MainBitmap->h / 2);
		}
		if (!m_WrapY) {
			m_FillColorUp = _getpixel(m_MainBitmap, m_MainBitmap->w / 2, 0);
			m_FillColorDown = _getpixel(m_MainBitmap, m_MainBitmap->w / 2, m_MainBitmap->h - 1);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int SLBackground::Create(const SLBackground &reference) {
		SceneLayer::Create(reference);

		// The main bitmap is created and owned by SceneLayer because it can be modified. We need to destroy it to avoid a leak because the bitmaps we'll be using here are owned by ContentFile static maps and are unmodifiable.
		destroy_bitmap(m_MainBitmap);
		m_MainBitmapOwned = false;

		m_Bitmaps.clear();
		m_Bitmaps = reference.m_Bitmaps;
		m_MainBitmap = m_Bitmaps.at(0);

		m_FillColorLeft = reference.m_FillColorLeft;
		m_FillColorRight = reference.m_FillColorRight;
		m_FillColorUp = reference.m_FillColorUp;
		m_FillColorDown = reference.m_FillColorDown;

		m_FrameCount = reference.m_FrameCount;
		m_SpriteAnimMode = reference.m_SpriteAnimMode;
		m_SpriteAnimDuration = reference.m_SpriteAnimDuration;

		m_AutoScrollX = reference.m_AutoScrollX;
		m_AutoScrollY = reference.m_AutoScrollY;
		m_AutoScrollStep = reference.m_AutoScrollStep;
		m_AutoScrollStepInterval = reference.m_AutoScrollStepInterval;

		m_LayerScaleFactors = reference.m_LayerScaleFactors;
		m_IgnoreAutoScale = reference.m_IgnoreAutoScale;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int SLBackground::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "FrameCount") {
			reader >> m_FrameCount;
		} else if (propName == "SpriteAnimMode") {
			m_SpriteAnimMode = static_cast<SpriteAnimMode>(std::stoi(reader.ReadPropValue()));
			if (m_SpriteAnimMode < SpriteAnimMode::NOANIM || m_SpriteAnimMode > SpriteAnimMode::ALWAYSPINGPONG) { reader.ReportError("Invalid SLBackground sprite animation mode!"); }
			if (m_FrameCount > 1) {
				// If animation mode is set to something other than ALWAYSLOOP but only has 2 frames, override it because it's pointless
				if ((m_SpriteAnimMode == SpriteAnimMode::ALWAYSRANDOM || m_SpriteAnimMode == SpriteAnimMode::ALWAYSPINGPONG) && m_FrameCount == 2) { m_SpriteAnimMode = SpriteAnimMode::ALWAYSLOOP; }
			} else {
				m_SpriteAnimMode = SpriteAnimMode::NOANIM;
			}
		} else if (propName == "SpriteAnimDuration") {
			reader >> m_SpriteAnimDuration;
		} else if (propName == "IsAnimatedManually") {
			reader >> m_IsAnimatedManually;
		} else if (propName == "DrawTransparent") {
			reader >> m_DrawTrans;
		} else if (propName == "ScrollRatio") {
			// Actually read the ScrollInfo, not the ratio. The ratios will be initialized later.
			reader >> m_ScrollInfo;
		} else if (propName == "ScaleFactor") {
			reader >> m_ScaleFactor;
			SetScaleFactor(m_ScaleFactor);
		} else if (propName == "IgnoreAutoScaling") {
			reader >> m_IgnoreAutoScale;
		} else if (propName == "OriginPointOffset") {
			reader >> m_OriginOffset;
		} else if (propName == "AutoScrollX") {
			reader >> m_AutoScrollX;
		} else if (propName == "AutoScrollY") {
			reader >> m_AutoScrollY;
		} else if (propName == "AutoScrollStepInterval") {
			reader >> m_AutoScrollStepInterval;
		} else if (propName == "AutoScrollStep") {
			reader >> m_AutoScrollStep;
		} else {
			return SceneLayer::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int SLBackground::Save(Writer &writer) const {
		SceneLayer::Save(writer);

		writer.NewPropertyWithValue("FrameCount", m_FrameCount);
		writer.NewPropertyWithValue("SpriteAnimMode", m_SpriteAnimMode);
		writer.NewPropertyWithValue("SpriteAnimDuration", m_SpriteAnimDuration);
		writer.NewPropertyWithValue("IsAnimatedManually", m_IsAnimatedManually);
		writer.NewPropertyWithValue("DrawTransparent", m_DrawTrans);
		writer.NewPropertyWithValue("ScrollRatio", m_ScrollInfo);
		writer.NewPropertyWithValue("ScaleFactor", m_ScaleFactor);
		writer.NewPropertyWithValue("IgnoreAutoScaling", m_IgnoreAutoScale);
		writer.NewPropertyWithValue("OriginPointOffset", m_OriginOffset);
		writer.NewPropertyWithValue("AutoScrollX", m_AutoScrollX);
		writer.NewPropertyWithValue("AutoScrollY", m_AutoScrollY);
		writer.NewPropertyWithValue("AutoScrollStepInterval", m_AutoScrollStepInterval);
		writer.NewPropertyWithValue("AutoScrollStep", m_AutoScrollStep);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SLBackground::InitScaleFactors() {
		if (!m_IgnoreAutoScale) {
			m_LayerScaleFactors.at(LayerAutoScaleMode::AutoScaleOff) = m_ScaleFactor;

			float fitScreenScaleFactor = 1;
			if (g_SceneMan.GetSceneHeight() > g_FrameMan.GetPlayerScreenHeight()) {
				fitScreenScaleFactor = std::clamp(static_cast<float>(g_FrameMan.GetPlayerScreenHeight()) / static_cast<float>(m_MainBitmap->h), 1.0F, 2.0F);
			} else if (g_SceneMan.GetSceneHeight() > m_MainBitmap->h) {
				fitScreenScaleFactor = std::clamp(static_cast<float>(g_SceneMan.GetSceneHeight()) / static_cast<float>(m_MainBitmap->h), 1.0F, 2.0F);
			}
			m_LayerScaleFactors.at(LayerAutoScaleMode::FitScreen).SetXY(fitScreenScaleFactor, fitScreenScaleFactor);

			switch (g_SettingsMan.GetSceneBackgroundAutoScaleMode()) {
				case LayerAutoScaleMode::FitScreen:
					SetScaleFactor(m_LayerScaleFactors.at(LayerAutoScaleMode::FitScreen));
					break;
				case LayerAutoScaleMode::AlwaysUpscaled:
					SetScaleFactor(m_LayerScaleFactors.at(LayerAutoScaleMode::AlwaysUpscaled));
					break;
				default:
					SetScaleFactor(m_LayerScaleFactors.at(LayerAutoScaleMode::AutoScaleOff));
					break;
			}
			m_ScrollInfo *= m_ScaleFactor;
			InitScrollRatios();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SLBackground::Update() {
		if (!m_IsAnimatedManually && m_SpriteAnimMode != SpriteAnimMode::NOANIM) {
			int frameTime = m_SpriteAnimDuration / m_FrameCount;
			int prevFrame = m_Frame;

			if (m_SpriteAnimTimer.GetElapsedSimTimeMS() > frameTime) {
				switch (m_SpriteAnimMode) {
					case SpriteAnimMode::ALWAYSLOOP:
						m_Frame = ((m_Frame + 1) % m_FrameCount);
						m_SpriteAnimTimer.Reset();
						break;
					case SpriteAnimMode::ALWAYSRANDOM:
						while (m_Frame == prevFrame) {
							m_Frame = RandomNum<int>(0, m_FrameCount - 1);
						}
						m_SpriteAnimTimer.Reset();
						break;
					case SpriteAnimMode::ALWAYSPINGPONG:
						if (m_Frame == m_FrameCount - 1) {
							m_SpriteAnimIsReversingFrames = true;
						} else if (m_Frame == 0) {
							m_SpriteAnimIsReversingFrames = false;
						}
						m_SpriteAnimIsReversingFrames ? m_Frame-- : m_Frame++;
						m_SpriteAnimTimer.Reset();
						break;
					default:
						break;
				}
			}
		}
		m_MainBitmap = m_Bitmaps.at(m_Frame);

		if (IsAutoScrolling()) {
			if (m_AutoScrollStepTimer.GetElapsedSimTimeMS() > m_AutoScrollStepInterval) {
				if (m_WrapX && m_AutoScrollX) { m_AutoScrollOffset.SetX(m_AutoScrollOffset.GetX() + m_AutoScrollStep.GetX()); }
				if (m_WrapY && m_AutoScrollY) { m_AutoScrollOffset.SetY(m_AutoScrollOffset.GetY() + m_AutoScrollStep.GetY()); }
				WrapPosition(m_AutoScrollOffset);
				m_AutoScrollStepTimer.Reset();
			}
			m_Offset.SetXY(std::floor((m_Offset.GetX() * m_ScrollRatio.GetX()) + m_AutoScrollOffset.GetX()), std::floor((m_Offset.GetY() * m_ScrollRatio.GetY()) + m_AutoScrollOffset.GetY()));
			WrapPosition(m_Offset);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void SLBackground::Draw(BITMAP *targetBitmap, Box &targetBox, const Vector &scrollOverride, bool offsetNeedsScrollRatioAdjustment) {
		SceneLayer::Draw(targetBitmap, targetBox, scrollOverride, !IsAutoScrolling());

		int bitmapWidth = m_ScaledDimensions.GetFloorIntX();
		int bitmapHeight = m_ScaledDimensions.GetFloorIntY();
		int targetBoxCornerX = targetBox.GetCorner().GetFloorIntX();
		int targetBoxCornerY = targetBox.GetCorner().GetFloorIntY();
		int targetBoxWidth = static_cast<int>(targetBox.GetWidth());
		int targetBoxHeight = static_cast<int>(targetBox.GetHeight());

		set_clip_rect(targetBitmap, targetBoxCornerX, targetBoxCornerY, targetBoxCornerX + targetBoxWidth - 1, targetBoxCornerY + targetBoxHeight - 1);

		// Detect if non-wrapping layer dimensions can't cover the whole target area with its main bitmap. If so, fill in the gap with appropriate solid color sampled from the hanging edge.
		if (!m_WrapX && bitmapWidth <= targetBoxWidth) {
			if (m_FillColorLeft != ColorKeys::g_MaskColor && m_Offset.GetFloorIntX() != 0) { rectfill(targetBitmap, targetBoxCornerX, targetBoxCornerY, targetBoxCornerX - m_Offset.GetFloorIntX(), targetBoxCornerY + targetBoxHeight, m_FillColorLeft); }
			if (m_FillColorRight != ColorKeys::g_MaskColor) { rectfill(targetBitmap, targetBoxCornerX + bitmapWidth - m_Offset.GetFloorIntX(), targetBoxCornerY, targetBoxCornerX + targetBoxWidth, targetBoxCornerY + targetBoxHeight, m_FillColorRight); }
		}
		if (!m_WrapY && bitmapHeight <= targetBoxHeight) {
			if (m_FillColorUp != ColorKeys::g_MaskColor && m_Offset.GetFloorIntY() != 0) { rectfill(targetBitmap, targetBoxCornerX, targetBoxCornerY, targetBoxCornerX + targetBoxWidth, targetBoxCornerY - m_Offset.GetFloorIntY(), m_FillColorUp); }
			if (m_FillColorDown != ColorKeys::g_MaskColor) { rectfill(targetBitmap, targetBoxCornerX, targetBoxCornerY + bitmapHeight - m_Offset.GetFloorIntY(), targetBoxCornerX + targetBoxWidth, targetBoxCornerY + targetBoxHeight, m_FillColorDown); }
		}
		// Reset the clip rect back to the entire target bitmap.
		set_clip_rect(targetBitmap, 0, 0, targetBitmap->w - 1, targetBitmap->h - 1);
	}
}