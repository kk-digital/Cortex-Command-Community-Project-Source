#include "Color.h"
#include "FrameMan.h"

namespace RTE {

	const std::string Color::c_ClassName = "Color";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Color::Create() {
		if (Serializable::Create()) {
			return -1;
		}
		RecalculateIndex();
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Color::Create(int inputR, int inputG, int inputB) {
		SetR(inputR);
		SetG(inputG);
		SetB(inputB);
		RecalculateIndex();
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Color::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "Index") {
			SetRGBWithIndex(std::stoi(reader.ReadPropValue()));
		} else if (propName == "R") {
			SetR(std::stoi(reader.ReadPropValue()));
		} else if (propName == "G") {
			SetG(std::stoi(reader.ReadPropValue()));
		} else if (propName == "B") {
			SetB(std::stoi(reader.ReadPropValue()));
		} else {
			return Serializable::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Color::Save(Writer &writer) const {
		Serializable::Save(writer);

		writer.NewPropertyWithValue("R", m_R);
		writer.NewPropertyWithValue("G", m_G);
		writer.NewPropertyWithValue("B", m_B);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Color::SetRGBWithIndex(int index) {
		m_Index = std::clamp(index, 0, 255);

		glm::vec4 rgbColor = g_FrameMan.GetDefaultPalette()->at(m_Index);

		// Multiply by 4 because the Allegro RGB struct elements are in range 0-63, and proper RGB needs 0-255.
		m_R = rgbColor.r;
		m_G = rgbColor.g;
		m_B = rgbColor.b;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Color::RecalculateIndex() {
		return m_Index = g_FrameMan.GetDefaultPalette()->GetIndexFromColor(*this);
	}
}
