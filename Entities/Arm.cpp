#include "Arm.h"
#include "HDFirearm.h"
#include "ThrownDevice.h"
#include "AHuman.h"
#include "PresetMan.h"
#include "SceneMan.h"

namespace RTE {

	ConcreteClassInfo(Arm, Attachable, 50);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Arm::Clear() {
		m_MaxLength = 0;
		m_MoveSpeed = 0;

		m_HandDefaultIdleOffset.Reset();
		m_HandIdleRotation = 0;

		m_HandCurrentOffset.Reset();

		m_HandTargets = {};
		m_HandMovementDelayTimer.Reset();
		m_HandMovementDelayTimer.SetSimTimeLimitMS(0);
		m_HandHasReachedCurrentTarget = false;

		m_HandSpriteFile.Reset();
		m_HandSpriteBitmap = nullptr;

		m_GripStrength = 0;
		m_ThrowStrength = 25.0F;

		m_HeldDevice = nullptr;
		m_HeldDeviceThisArmIsTryingToSupport = nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Arm::Create() {
		if (Attachable::Create() < 0) {
			return -1;
		}

		// Ensure Arms don't get flagged as inheriting RotAngle, since they never do and always set their RotAngle for themselves.
		m_InheritsRotAngle = false;

		// Ensure Arms don't collide with terrain when attached since their expansion/contraction is frame based so atom group doesn't know how to account for it.
		SetCollidesWithTerrainWhileAttached(false);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Arm::Create(const Arm &reference) {
		if (reference.m_HeldDevice) {
			m_ReferenceHardcodedAttachableUniqueIDs.insert(reference.m_HeldDevice->GetUniqueID());
		}

		Attachable::Create(reference);

		m_MaxLength = reference.m_MaxLength;
		m_MoveSpeed = reference.m_MoveSpeed;

		m_HandDefaultIdleOffset = reference.m_HandDefaultIdleOffset;
		m_HandIdleRotation = reference.m_HandIdleRotation;

		m_HandCurrentOffset = reference.m_HandCurrentOffset;

		m_HandTargets = reference.m_HandTargets;
		m_HandMovementDelayTimer = reference.m_HandMovementDelayTimer;
		m_HandHasReachedCurrentTarget = reference.m_HandHasReachedCurrentTarget;
		
		m_HandSpriteFile = reference.m_HandSpriteFile;
		m_HandSpriteBitmap = m_HandSpriteFile.GetAsBitmap();
		RTEAssert(m_HandSpriteBitmap, "Failed to load hand bitmap in Arm::Create.");

		m_GripStrength = reference.m_GripStrength;
		m_ThrowStrength = reference.m_ThrowStrength;

		if (reference.m_HeldDevice) {
			SetHeldDevice(dynamic_cast<HeldDevice *>(reference.m_HeldDevice->Clone()));
		}

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Arm::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "MaxLength") {
			reader >> m_MaxLength;
		} else if (propName == "MoveSpeed") {
			reader >> m_MoveSpeed;
		} else if (propName == "HandDefaultIdleOffset" || propName == "IdleOffset") {
			reader >> m_HandDefaultIdleOffset;
		} else if (propName == "HandSprite" || propName == "Hand") {
			reader >> m_HandSpriteFile;
			m_HandSpriteBitmap = m_HandSpriteFile.GetAsBitmap();
		} else if (propName == "GripStrength") {
			reader >> m_GripStrength;
		} else if (propName == "ThrowStrength") {
			reader >> m_ThrowStrength;
		} else if (propName == "HeldDevice") {
			SetHeldDevice(dynamic_cast<HeldDevice *>(g_PresetMan.ReadReflectedPreset(reader)));
		} else {
			return Attachable::ReadProperty(propName, reader);
		}

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Arm::Save(Writer &writer) const {
		Attachable::Save(writer);

		writer.NewPropertyWithValue("MaxLength", m_MaxLength);
		writer.NewPropertyWithValue("MoveSpeed", m_MoveSpeed);
		writer.NewPropertyWithValue("HandIdleOffset", m_HandDefaultIdleOffset);
		writer.NewPropertyWithValue("HandSprite", m_HandSpriteFile);
		writer.NewPropertyWithValue("GripStrength", m_GripStrength);
		writer.NewPropertyWithValue("ThrowStrength", m_ThrowStrength);
		writer.NewPropertyWithValue("HeldDevice", m_HeldDevice);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::SetHandCurrentPos(const Vector &newHandPos) {
		SetHandCurrentOffset(g_SceneMan.ShortestDistance(m_JointPos, newHandPos, g_SceneMan.SceneWrapsX() || g_SceneMan.SceneWrapsY()));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Arm::AddHandTarget(const std::string &description, const Vector &handTargetPositionToAdd, float delayAtTarget) {
		Vector handTargetOffsetToAdd = g_SceneMan.ShortestDistance(m_JointPos, handTargetPositionToAdd, g_SceneMan.SceneWrapsX() || g_SceneMan.SceneWrapsY());
		if (!handTargetOffsetToAdd.IsZero()) {
			handTargetOffsetToAdd.ClampMagnitude(m_MaxLength / 2.0F, m_MaxLength);
			if (!m_HandTargets.empty()) {
				if ((description == m_HandTargets.back().Description) || (handTargetOffsetToAdd - m_HandTargets.back().TargetOffset).MagnitudeIsLessThan(m_MaxLength / 10.0F)) {
					m_HandTargets.back() = { description, handTargetOffsetToAdd, std::max(m_HandTargets.back().DelayAtTarget, delayAtTarget) };
					return;
				}
			} else {
				m_HandHasReachedCurrentTarget = false;
			}
			m_HandTargets.emplace(description, handTargetOffsetToAdd, delayAtTarget);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::SetHeldDevice(HeldDevice *newHeldDevice) {
		if (m_HeldDevice && m_HeldDevice->IsAttached()) { RemoveAndDeleteAttachable(m_HeldDevice); }
		m_HeldDevice = newHeldDevice;

		if (newHeldDevice != nullptr) {
			AddAttachable(newHeldDevice);

			m_HardcodedAttachableUniqueIDsAndSetters.try_emplace(
				newHeldDevice->GetUniqueID(),
				[](MOSRotating *parent, Attachable *attachable) {
					HeldDevice *castedAttachable = dynamic_cast<HeldDevice *>(attachable);
					RTEAssert(!attachable || castedAttachable, "Tried to pass incorrect Attachable subtype " + (attachable ? attachable->GetClassName() : "") + " to SetHeldDevice.");
					dynamic_cast<Arm *>(parent)->SetHeldDevice(castedAttachable);
				}
			);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HeldDevice * Arm::SwapHeldDevice(HeldDevice * newHeldDevice) {
		Attachable *previousHeldDevice = RemoveAttachable(m_HeldDevice, false, false);
		SetHeldDevice(newHeldDevice);
		return dynamic_cast<HeldDevice *>(previousHeldDevice);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::Update() {
		Attachable::PreUpdate();

		bool armHasParent = IsAttached();
		if (!armHasParent) {
			RemoveAttachable(m_HeldDevice, true, false);
		} else {
			m_AngularVel = 0.0F;
		}

		bool heldDeviceIsAThrownDevice = m_HeldDevice && dynamic_cast<ThrownDevice *>(m_HeldDevice);

		// If there's no HeldDevice, or it's a ThrownDevice, the Arm should rotate to match the hand's current offset for visuals/aiming (instead of using the AHuman's aim angle).
		if (heldDeviceIsAThrownDevice || !m_HeldDevice) {
			m_Rotation = m_HandCurrentOffset.GetAbsRadAngle() + (m_HFlipped ? c_PI : 0);
		}

		UpdateHandCurrentOffset(armHasParent, heldDeviceIsAThrownDevice);
		if (m_HeldDevice) {
			// In order to keep the HeldDevice in the right place, we need to convert its offset (the hand's current offset) to work as the ParentOffset for the HeldDevice.
			// The HeldDevice will then use this to set its JointPos when it's updated. Unfortunately UnRotateOffset doesn't work for this, since it's Vector/Matrix division, which isn't commutative.
			Vector handOffsetAsParentOffset = RotateOffset(m_JointOffset) + m_HandCurrentOffset;
			handOffsetAsParentOffset.RadRotate(-m_Rotation.GetRadAngle()).FlipX(m_HFlipped);
			m_HeldDevice->SetParentOffset(handOffsetAsParentOffset);
		}

		Attachable::Update();

		m_Recoiled = m_HeldDevice && m_HeldDevice->IsRecoiled();

		//TODO why is this like this with the stuff above?!?!? I guess it's relevant that rotation is updated before/after updating current hand offset?
		if (m_HeldDevice) {
			m_Rotation = m_HandCurrentOffset.GetAbsRadAngle() + (m_HFlipped ? c_PI : 0);
			m_Pos = m_JointPos - RotateOffset(m_JointOffset);
		}

		if (armHasParent) {
			UpdateArmFrame();
		}

		m_HandIdleRotation = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::UpdateHandCurrentOffset(bool armHasParent, bool heldDeviceIsAThrownDevice) {
		if (armHasParent) {
			Vector targetOffset;
			if (m_HandTargets.empty()) {
				if (bool parentIsStable = dynamic_cast<Actor *>(m_Parent)->IsStatus(Actor::Status::STABLE); parentIsStable && m_HeldDevice) {
					targetOffset = m_HeldDevice->GetStanceOffset();
				} else if (parentIsStable && m_HeldDeviceThisArmIsTryingToSupport) {
					targetOffset = g_SceneMan.ShortestDistance(m_JointPos, m_HeldDeviceThisArmIsTryingToSupport->GetSupportPos(), g_SceneMan.SceneWrapsX() || g_SceneMan.SceneWrapsY());
				} else {
					targetOffset = m_HandDefaultIdleOffset.GetXFlipped(m_Parent->IsHFlipped()).GetRadRotatedCopy(m_Parent->GetRotAngle());
				}
				if (m_HandIdleRotation != 0) {
					targetOffset.RadRotate(m_HandIdleRotation);
				}
			} else {
				targetOffset = m_HandTargets.front().TargetOffset;
			}

			if (m_HeldDevice && !heldDeviceIsAThrownDevice) {
				if (!m_HeldDevice->GetRecoilForce().IsZero()) {
					AccountForHeldDeviceRecoil(m_HeldDevice, targetOffset);
				}
				AccountForHeldDeviceTerrainClipping(m_HeldDevice, targetOffset);
				targetOffset *= m_Rotation;
			}

			targetOffset.ClampMagnitude(m_MaxLength / 2.0F, m_MaxLength);
			m_HandCurrentOffset += (targetOffset - m_HandCurrentOffset) * m_MoveSpeed;
			m_HandCurrentOffset.ClampMagnitude(m_MaxLength / 2.0F, m_MaxLength);

			bool handIsCloseToTargetOffset = HandIsCloseToTargetOffset(targetOffset);
			if (handIsCloseToTargetOffset && !m_HandHasReachedCurrentTarget && !m_HandTargets.empty()) {
				m_HandMovementDelayTimer.SetSimTimeLimitMS(m_HandTargets.front().DelayAtTarget);
				m_HandMovementDelayTimer.Reset();
			}
			m_HandHasReachedCurrentTarget = handIsCloseToTargetOffset;

			if (m_HandHasReachedCurrentTarget && m_HandMovementDelayTimer.IsPastSimTimeLimit()) {
				if (!m_HandTargets.empty()) {
					m_HandTargets.pop();
				}
				m_HandHasReachedCurrentTarget = false;
			}
		} else {
			m_HandCurrentOffset.SetXY(m_MaxLength * 0.65F, 0);
			m_HandCurrentOffset.RadRotate((m_HFlipped ? c_PI : 0) + m_Rotation.GetRadAngle());
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::AccountForHeldDeviceRecoil(const HeldDevice *heldDevice, Vector &targetOffset) {
		// TODO: Fine-tune this if needed.
		if (!heldDevice->GetRecoilForce().IsZero()) {
			float totalGripStrength = m_GripStrength * heldDevice->GetGripStrengthMultiplier();
			if (totalGripStrength == 0) { totalGripStrength = heldDevice->GetJointStrength(); }
			if (heldDevice->GetSupported()) {
				const AHuman *rootParentAsAHuman = dynamic_cast<const AHuman *>(GetRootParent());
				const Arm *rootParentAsHumanBGArm = rootParentAsAHuman ? rootParentAsAHuman->GetBGArm() : nullptr;
				if (rootParentAsHumanBGArm) {
					if (rootParentAsHumanBGArm->GetGripStrength() < 0) {
						totalGripStrength = -1.0F;
					} else if (rootParentAsHumanBGArm->GetGripStrength() > 0) {
						totalGripStrength += (rootParentAsHumanBGArm->GetGripStrength() * heldDevice->GetGripStrengthMultiplier());
					} else {
						totalGripStrength *= 1.5F;
					}
				}
			}
			if (totalGripStrength > 0) {
				// Diminish recoil effect when body is horizontal so that the device doesn't get pushed into terrain when prone.
				float rotAngleScalar = std::abs(std::cos(m_Parent->GetRotAngle()));
				float recoilScalar = std::min((heldDevice->GetRecoilForce() / totalGripStrength).GetMagnitude() * 0.4F, 0.8F) * rotAngleScalar;
				targetOffset.SetX(targetOffset.GetX() * (1.0F - recoilScalar));

				// Shift Y offset slightly so the device is more likely to go under the shoulder rather than over it, otherwise it looks goofy.
				if (targetOffset.GetY() <= 0) {
					targetOffset.SetY(targetOffset.GetY() * (1.0F - recoilScalar) + recoilScalar);
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::AccountForHeldDeviceTerrainClipping(const HeldDevice *heldDevice, Vector &targetOffset) const {
		Vector newMuzzlePos = (m_JointPos + targetOffset) - RotateOffset(heldDevice->GetJointOffset()) + RotateOffset(heldDevice->GetMuzzleOffset());
		Vector midToMuzzle = RotateOffset({ heldDevice->GetIndividualRadius(), 0 });
		Vector midOfDevice = newMuzzlePos - midToMuzzle;

		Vector terrainOrMuzzlePosition;
		if (g_SceneMan.CastStrengthRay(midOfDevice, midToMuzzle, 5, terrainOrMuzzlePosition, 0, false)) {
			Vector muzzleAdjustment = g_SceneMan.ShortestDistance(newMuzzlePos, terrainOrMuzzlePosition, g_SceneMan.SceneWrapsX());
			if (muzzleAdjustment.GetMagnitude() > 2.0F) {
				targetOffset += muzzleAdjustment;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::UpdateArmFrame() {
		float halfMaxLength = m_MaxLength / 2.0F;
		float newFrame = std::floor(((m_HandCurrentOffset.GetMagnitude() - halfMaxLength) / halfMaxLength) * static_cast<float>(m_FrameCount));
		m_Frame = static_cast<unsigned int>(std::clamp(newFrame, 0.0F, static_cast<float>(m_FrameCount - 1)));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::Draw(BITMAP *targetBitmap, const Vector &targetPos, DrawMode mode, bool onlyPhysical) const {
		Attachable::Draw(targetBitmap, targetPos, mode, onlyPhysical);

		if (!onlyPhysical && (mode == g_DrawColor || mode == g_DrawWhite || mode == g_DrawTrans)) {
			DrawHand(targetBitmap, targetPos, mode);
			if (m_HeldDevice && m_HeldDevice->IsDrawnAfterParent()) { m_HeldDevice->Draw(targetBitmap, targetPos, mode, onlyPhysical); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Arm::DrawHand(BITMAP *targetBitmap, const Vector &targetPos, DrawMode mode) const {
		Vector handPos(m_JointPos + m_HandCurrentOffset + (m_Recoiled ? m_RecoilOffset : Vector()) - targetPos);
		handPos.m_X -= static_cast<float>(m_HandSpriteBitmap->w / 2);
		handPos.m_Y -= static_cast<float>(m_HandSpriteBitmap->h / 2);

		if (!m_HFlipped) {
			if (mode == g_DrawWhite) {
				draw_character_ex(targetBitmap, m_HandSpriteBitmap, handPos.GetFloorIntX(), handPos.GetFloorIntY(), g_WhiteColor, -1);
			} else {
				draw_sprite(targetBitmap, m_HandSpriteBitmap, handPos.GetFloorIntX(), handPos.GetFloorIntY());
			}
		} else {
			//TODO this draw_character_ex won't draw flipped. It should draw onto a temp bitmap and then draw that flipped. Maybe it can reuse a temp bitmap from MOSR, maybe not?
			if (mode == g_DrawWhite) {
				draw_character_ex(targetBitmap, m_HandSpriteBitmap, handPos.GetFloorIntX(), handPos.GetFloorIntY(), g_WhiteColor, -1);
			} else {
				draw_sprite_h_flip(targetBitmap, m_HandSpriteBitmap, handPos.GetFloorIntX(), handPos.GetFloorIntY());
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool Arm::HandIsCloseToTargetOffset(const Vector &targetOffset) const {
		return (m_HandCurrentOffset - targetOffset).MagnitudeIsLessThan(m_MaxLength / 10.0F);
	}
}