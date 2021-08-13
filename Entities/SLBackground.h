#ifndef _RTESLBACKGROUND_
#define _RTESLBACKGROUND_

#include "SceneLayer.h"
#include "Timer.h"

namespace RTE {

	/// <summary>
	/// 
	/// </summary>
	class SLBackground : public SceneLayer {

	public:

		EntityAllocation(SLBackground)
		SerializableOverrideMethods
		ClassInfoGetters

#pragma region Creation
		/// <summary>
		/// 
		/// </summary>
		SLBackground() { Clear(); }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int Create() override;

		/// <summary>
		/// Creates a SLBackground to be identical to another, by deep copy.
		/// </summary>
		/// <param name="reference">A reference to the SLBackground to deep copy.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int Create(const SLBackground &reference);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		~SLBackground() override { Destroy(true); }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="notInherited"></param>
		void Destroy(bool notInherited = false) override { if (!notInherited) { SceneLayer::Destroy(); } Clear(); }
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool IsAutoScrolling() const { return (m_WrapX && m_AutoScrollX) || (m_WrapY && m_AutoScrollY); }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool GetAutoScrollX() const { return m_AutoScrollX; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="autoScroll"></param>
		void SetAutoScrollX(bool autoScroll) { m_AutoScrollX = autoScroll; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool GetAutoScrollY() const { return m_AutoScrollY; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="autoScroll"></param>
		void SetAutoScrollY(bool autoScroll) { m_AutoScrollY = autoScroll; }
#pragma endregion

#pragma region Concrete Methods

#pragma endregion

#pragma region Virtual Override Methods
		/// <summary>
		/// Updates the state of this SLTerrain. Supposed to be done every frame.
		/// </summary>
		void Update() override;

		/// <summary>
		/// Draws this SLTerrain's foreground's current scrolled position to a bitmap.
		/// </summary>
		/// <param name="targetBitmap">The bitmap to draw to.</param>
		/// <param name="targetBox">The box on the target bitmap to limit drawing to, with the corner of box being where the scroll position lines up.</param>
		/// <param name="scrollOverride">If a non-{-1,-1} vector is passed, the internal scroll offset of this is overridden with it. It becomes the new source coordinates.</param>
		void Draw(BITMAP *targetBitmap, Box &targetBox, const Vector &scrollOverride = Vector(-1, -1)) override;
#pragma endregion

	protected:

		static Entity::ClassInfo m_sClass;

		bool m_AutoScrollX;
		bool m_AutoScrollY;
		Vector m_AutoScrollStep;
		int m_AutoScrollStepInterval;
		Timer m_AutoScrollStepTimer;

		std::vector<BITMAP *> m_Bitmaps;
		int m_FrameCount;
		int m_Frame;
		Timer m_SpriteAnimTimer;

		// Keep track of animation direction (mainly for ALWAYSPINGPONG), true is decreasing frame, false is increasing frame
		bool m_SpriteAnimIsReversingFrames;

		int m_SpriteAnimDuration;
		int m_SpriteAnimMode;

		Vector m_AutoScrollOffset;

	private:

		enum LayerAutoScaleMode { AutoScaleOff, FitScreen, AlwaysUpscaled, LayerAutoScaleModeCount };

		std::array<Vector, LayerAutoScaleMode::LayerAutoScaleModeCount> m_LayerScaleFactors;

		/// <summary>
		/// 
		/// </summary>
		void InitScaleFactors();

		/// <summary>
		/// Clears all the member variables of this SLBackground, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear();

		// Disallow the use of some implicit methods.
		SLBackground(const SLBackground &reference) = delete;
		SLBackground & operator=(const SLBackground &rhs) = delete;
	};
}
#endif