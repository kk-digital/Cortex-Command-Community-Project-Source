#include "PrimitiveMan.h"
#include "Entity.h"
#include "MOSprite.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PrimitiveMan::DrawBitmapPrimitive(int player, const Vector &centerPos, Entity *entity, float rotAngle, int frame, bool hFlipped, bool vFlipped) {
		const MOSprite *moSprite = dynamic_cast<MOSprite *>(entity);
		if (moSprite) {
			std::shared_ptr<Texture> bitmap = moSprite->GetSpriteFrame(frame);
			if (bitmap) { m_ScheduledPrimitives.push_back(std::make_unique<BitmapPrimitive>(player, centerPos, bitmap, rotAngle, hFlipped, vFlipped)); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PrimitiveMan::DrawPrimitives(int player, SDL_Renderer* renderer, const Vector &targetPos) const {
		for (const std::unique_ptr<GraphicalPrimitive> &primitive : m_ScheduledPrimitives) {
			if (primitive->m_Player == player || primitive->m_Player == -1) { primitive->Draw(renderer, targetPos); }
		}
	}
}
