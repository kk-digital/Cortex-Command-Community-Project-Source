#include "CameraMan.h"

#include "Activity.h"
#include "FrameMan.h"
#include "Scene.h"
#include "SceneMan.h"
#include "SLTerrain.h"
#include "NetworkClient.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::Clear() {
		m_ScreenShakeStrength = 1.0F;
		m_ScreenShakeDecay = 50.0F;
		m_MaxScreenShakeTime = 1.0F;
		m_DefaultShakePerUnitOfGibEnergy = 0.001F;
		m_DefaultShakePerUnitOfRecoilEnergy = 0.5F;
		m_DefaultShakeFromRecoilMaximum = 0.0F;

		for (Screen &screen : m_Screens) {
			screen.Offset.Reset();
			screen.DeltaOffset.Reset();
			screen.ScrollTarget.Reset();
			screen.ScreenTeam = Activity::NoTeam;
			screen.ScrollSpeed = 0.1F;
			screen.ScrollTimer.Reset();
			screen.ScreenOcclusion.Reset();
			screen.TargetWrapped = false;
			screen.SeamCrossCount[Axes::X] = 0;
			screen.SeamCrossCount[Axes::Y] = 0;
			screen.ScreenShakeMagnitude = 0.0F;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::Update(int screenId) {
		Screen &screen = m_Screens[screenId];
		const SLTerrain *terrain = g_SceneMan.GetScene()->GetTerrain();

		// Don't let our screen shake beyond our max.
		screen.ScreenShakeMagnitude = std::min(screen.ScreenShakeMagnitude, m_ScreenShakeDecay * m_MaxScreenShakeTime);

		// Reduce screen shake over time
		screen.ScreenShakeMagnitude -= m_ScreenShakeDecay * static_cast<float>(screen.ScrollTimer.GetElapsedRealTimeS());
		screen.ScreenShakeMagnitude = std::max(screen.ScreenShakeMagnitude, 0.0F);

		// Feedback was that the best screen-shake strength was between 25% and 40% of default.
		// As such, we want the default setting to reflect that, instead the default setting being 30%.
		// So just hard-coded multiply to make 100% in settings correspond to 30% here (much easier than rebalancing everything).
		const float screenShakeScale = 0.3F;

		Vector screenShakeOffset(1.0F, 0.0F);
		screenShakeOffset.RadRotate(RandomNormalNum() * c_PI);
		screenShakeOffset *= screen.ScreenShakeMagnitude * m_ScreenShakeStrength * screenShakeScale;

		if (g_TimerMan.DrawnSimUpdate()) {
			// Adjust for wrapping if the scroll target jumped a seam this frame, as reported by whatever screen set it (the scroll target) this frame. This is to avoid big, scene-wide jumps in scrolling when traversing the seam.
			if (screen.TargetWrapped) {
				if (terrain->WrapsX()) {
					int wrappingScrollDirection = (screen.ScrollTarget.GetFloorIntX() < (terrain->GetBitmap()->w / 2)) ? 1 : -1;
					screen.Offset.SetX(screen.Offset.GetX() - (static_cast<float>(terrain->GetBitmap()->w * wrappingScrollDirection)));
					screen.SeamCrossCount[Axes::X] += wrappingScrollDirection;
				}
				if (terrain->WrapsY()) {
					int wrappingScrollDirection = (screen.ScrollTarget.GetFloorIntY() < (terrain->GetBitmap()->h / 2)) ? 1 : -1;
					screen.Offset.SetY(screen.Offset.GetY() - (static_cast<float>(terrain->GetBitmap()->h * wrappingScrollDirection)));
					screen.SeamCrossCount[Axes::Y] += wrappingScrollDirection;
				}
			}
			screen.TargetWrapped = false;
		}

		Vector oldOffset(screen.Offset);

		Vector offsetTarget;
		if (g_FrameMan.IsInMultiplayerMode()) {
			offsetTarget.SetX(screen.ScrollTarget.GetX() - static_cast<float>(g_FrameMan.GetPlayerFrameBufferWidth(screenId) / 2));
			offsetTarget.SetY(screen.ScrollTarget.GetY() - static_cast<float>(g_FrameMan.GetPlayerFrameBufferHeight(screenId) / 2));
		} else {
			offsetTarget.SetX(screen.ScrollTarget.GetX() - static_cast<float>(g_FrameMan.GetResX() / (g_FrameMan.GetVSplit() ? 4 : 2)));
			offsetTarget.SetY(screen.ScrollTarget.GetY() - static_cast<float>(g_FrameMan.GetResY() / (g_FrameMan.GetHSplit() ? 4 : 2)));
		}
		// Take the occlusion of the screens into account so that the scroll target is still centered on the terrain-visible portion of the screen.
		offsetTarget -= (screen.ScreenOcclusion / 2);

		Vector newOffset = screen.Offset;
		if (offsetTarget.GetFloored() != screen.Offset.GetFloored()) {
			Vector scrollVec(offsetTarget - screen.Offset);
			float scrollProgress = std::min(1.0F, static_cast<float>(screen.ScrollSpeed * screen.ScrollTimer.GetElapsedRealTimeMS() * 0.05F));
			newOffset += scrollVec * scrollProgress;
		}

		newOffset += screenShakeOffset;
		SetOffset(newOffset, screenId);

		screen.DeltaOffset = screen.Offset - oldOffset;
		screen.ScrollTimer.Reset();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::SetOffset(const Vector &offset, int screenId) {
		m_Screens[screenId].Offset = offset.GetFloored();
		CheckOffset(screenId);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Vector CameraMan::GetUnwrappedOffset(int screenId) const {
		const Screen &screen = m_Screens[screenId];
		const SLTerrain *terrain = g_SceneMan.GetScene()->GetTerrain();
		return Vector(screen.Offset.GetX() + static_cast<float>(terrain->GetBitmap()->w * screen.SeamCrossCount[Axes::X]), screen.Offset.GetY() + static_cast<float>(terrain->GetBitmap()->h * screen.SeamCrossCount[Axes::Y]));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::SetScroll(const Vector &center, int screenId) {
		Screen& screen = m_Screens[screenId];
		if (g_FrameMan.IsInMultiplayerMode()) {
			screen.Offset.SetXY(static_cast<float>(center.GetFloorIntX() - (g_FrameMan.GetPlayerFrameBufferWidth(screenId) / 2)), static_cast<float>(center.GetFloorIntY() - (g_FrameMan.GetPlayerFrameBufferHeight(screenId) / 2)));
		} else {
			screen.Offset.SetXY(static_cast<float>(center.GetFloorIntX() - (g_FrameMan.GetResX() / 2)), static_cast<float>(center.GetFloorIntY() - (g_FrameMan.GetResY() / 2)));
		}
		CheckOffset(screenId);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::SetScrollTarget(const Vector &targetCenter, float speed, bool targetWrapped, int screenId) {
		Screen &screen = m_Screens[screenId];
		// See if it would make sense to automatically wrap
		if (!targetWrapped) {
			const SLTerrain *terrain = g_SceneMan.GetScene()->GetTerrain();
			// If the difference is more than half the scene width, then wrap
			if ((terrain->WrapsX() && std::fabs(targetCenter.GetX() - screen.ScrollTarget.GetX()) > static_cast<float>(terrain->GetBitmap()->w / 2)) ||
				(terrain->WrapsY() && std::fabs(targetCenter.GetY() - screen.ScrollTarget.GetY()) > static_cast<float>(terrain->GetBitmap()->h / 2))) {
				targetWrapped = true;
			}
		}
		screen.ScrollTarget.SetXY(targetCenter.GetX(), targetCenter.GetY());
		screen.ScrollSpeed = speed;

		// Don't override a set wrapping, it will be reset to false upon a drawn frame.
		screen.TargetWrapped = screen.TargetWrapped || targetWrapped;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Vector CameraMan::GetScrollTarget(int screenId) const {
		return g_NetworkClient.IsConnectedAndRegistered() ? g_NetworkClient.GetFrameTarget() : m_Screens[screenId].ScrollTarget;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float CameraMan::TargetDistanceScalar(const Vector &point) const {
		if (!g_SceneMan.GetScene()) {
			return 0.0F;
		}

		float screenRadius = static_cast<float>(std::max(g_FrameMan.GetPlayerScreenWidth(), g_FrameMan.GetPlayerScreenHeight())) / 2.0F;
		float sceneRadius = static_cast<float>(std::max(g_SceneMan.GetScene()->GetWidth(), g_SceneMan.GetScene()->GetHeight())) / 2.0F;

		// Avoid divide by zero problems if scene and screen radius are the same.
		if (screenRadius == sceneRadius) {
			sceneRadius += 100.0F;
		}
		float closestScalar = 1.0F;

		for (const Screen &screen : m_Screens) {
			float distance = g_SceneMan.ShortestDistance(point, screen.ScrollTarget).GetMagnitude();
			float scalar = 0.0F;

			// Check if we're off the screen and then fall off.
			if (distance > screenRadius) {
				// Get ratio of how close to the very opposite of the scene the point is.
				scalar = 0.5F + 0.5F * (distance - screenRadius) / (sceneRadius - screenRadius);
			} else {
				scalar = 0.0F;
			}

			// See if this screen's distance scalar is the closest one yet.
			if (scalar < closestScalar) {
				closestScalar = scalar;
			}
		}

		// Return the scalar that was shows the closest scroll target of any current screen to the point.
		return closestScalar;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::CheckOffset(int screenId) {
		RTEAssert(g_SceneMan.GetScene(), "Trying to check offset before there is a scene or terrain!");

		const SLTerrain *terrain = g_SceneMan.GetScene()->GetTerrain();
		RTEAssert(terrain, "Trying to get terrain matter before there is a scene or terrain!");

		Screen &screen = m_Screens[screenId];

		if (!terrain->WrapsX() && screen.Offset.GetX() < 0) {
			screen.Offset.SetX(0.0F);
		}

		if (!terrain->WrapsY() && screen.Offset.GetY() < 0) {
			screen.Offset.SetY(0.0F);
		}

		Vector frameSize = GetFrameSize(screenId);
		int frameWidth = frameSize.GetFloorIntX();
		int frameHeight = frameSize.GetFloorIntY();

		if (!terrain->WrapsX() && screen.Offset.GetFloorIntX() >= terrain->GetBitmap()->w - frameWidth) {
			screen.Offset.SetX(static_cast<float>(terrain->GetBitmap()->w - frameWidth));
		}
		if (!terrain->WrapsY() && screen.Offset.GetFloorIntY() >= terrain->GetBitmap()->h - frameHeight) {
			screen.Offset.SetY(static_cast<float>(terrain->GetBitmap()->h - frameHeight));
		}

		// TODO: Figure out why this dupe here. Doesn't seem to affect anything when commented out.
		//if (!terrain->WrapsX() && screen.Offset.GetFloorIntX() >= terrain->GetBitmap()->w - frameWidth) {
		//	screen.Offset.SetX(static_cast<float>(terrain->GetBitmap()->w - frameWidth));
		//}
		//if (!terrain->WrapsY() && screen.Offset.GetFloorIntY() >= terrain->GetBitmap()->h - frameHeight) {
		//	screen.Offset.SetY(static_cast<float>(terrain->GetBitmap()->h - frameHeight));
		//}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Vector CameraMan::GetFrameSize(int screenId) {
		int frameWidth = g_FrameMan.GetResX();
		int frameHeight = g_FrameMan.GetResY();

		if (g_FrameMan.IsInMultiplayerMode()) {
			frameWidth = g_FrameMan.GetPlayerFrameBufferWidth(screenId);
			frameHeight = g_FrameMan.GetPlayerFrameBufferHeight(screenId);
		} else {
			frameWidth = frameWidth / (g_FrameMan.GetVSplit() ? 2 : 1);
			frameHeight = frameHeight / (g_FrameMan.GetHSplit() ? 2 : 1);
		}

		return Vector(static_cast<float>(frameWidth), static_cast<float>(frameHeight));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void CameraMan::AddScreenShake(float magnitude, const Vector &position) {
		for (int screenId = 0; screenId < g_FrameMan.GetScreenCount(); ++screenId) {
			Screen &screen = m_Screens[screenId];

			Vector frameSize = GetFrameSize(screenId);

			Box screenBox(screen.Offset, frameSize.GetX(), frameSize.GetY());
			std::list<Box> wrappedBoxes;
			g_SceneMan.WrapBox(screenBox, wrappedBoxes);

			float closestDistanceFromScreen = std::numeric_limits<float>::max();
			for (const Box &box : wrappedBoxes) {
				// Determine how far the position is from the box.
				Vector closestPointOnBox = box.GetWithinBox(position);
				Vector diff = closestPointOnBox - position;
				closestDistanceFromScreen = std::min(closestDistanceFromScreen, diff.GetMagnitude());
			}

			// Beyond this many screens distance, no shake will be applied.
			const float screenShakeFalloff = 0.3F;

			float screenDistance = std::max(frameSize.GetX(), frameSize.GetY()) * screenShakeFalloff;
			float screenShakeMultipler = std::max(1.0F - (closestDistanceFromScreen / screenDistance), 0.0F);
			screen.ScreenShakeMagnitude += magnitude * screenShakeMultipler;
		}
	}
}