#include "PieSlice.h"

#include "Constants.h"

#include "PresetMan.h"

namespace RTE {

	const string PieSlice::c_ClassName = "Slice";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PieSlice::Clear() {
		m_SliceType = PieSliceIndex::PSI_NONE;
		m_Description.clear();
		m_Direction = SliceDirection::UP;
		m_Enabled = true;
		m_Icon.Reset();

		m_ScriptPath.clear();
		m_FunctionName.clear();

		m_AreaStart = 0;
		m_AreaArc = c_QuarterPI;
		m_MidAngle = m_AreaStart + (m_AreaArc / 2);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int PieSlice::Create() { return 0; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int PieSlice::Create(const PieSlice &reference) {
		m_SliceType = reference.m_SliceType;
		m_Description = reference.m_Description;
		m_Direction = reference.m_Direction;
		m_Enabled = reference.m_Enabled;
		m_Icon = reference.m_Icon;

		m_ScriptPath = reference.m_ScriptPath;
		m_FunctionName = reference.m_FunctionName;

		m_AreaStart = reference.m_AreaStart;
		m_AreaArc = reference.m_AreaArc;
		m_MidAngle = reference.m_MidAngle;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP *RTE::PieSlice::GetAppropriateIcon(bool sliceIsSelected) const {
		int iconFrameCount = m_Icon.GetFrameCount();
		if (iconFrameCount > 0) {
			if (!IsEnabled() && iconFrameCount > 2) {
				return m_Icon.GetBitmaps8()[2];
			} else if (sliceIsSelected && iconFrameCount > 1) {
				return m_Icon.GetBitmaps8()[1];
			} else {
				return m_Icon.GetBitmaps8()[0];
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int PieSlice::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "Description") {
			reader >> m_Description;
		} else if (propName == "Icon") {
			reader >> m_Icon;
		} else if (propName == "Direction") {
			m_Direction = static_cast<SliceDirection>(std::stoi(reader.ReadPropValue()));
		} else if (propName == "ScriptPath") {
			reader >> m_ScriptPath;
		} else if (propName == "FunctionName") {
			reader >> m_FunctionName;
		} else {
			return Serializable::ReadProperty(propName, reader);
		}

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int PieSlice::Save(Writer &writer) const {
		Serializable::Save(writer);

		writer.NewPropertyWithValue("Description", m_Description);
		writer.NewPropertyWithValue("Icon", m_Icon);
		writer.NewPropertyWithValue("Direction", static_cast<int>(m_Direction));
		if (!m_ScriptPath.empty() && !m_FunctionName.empty()) {
			writer.NewPropertyWithValue("ScriptPath", m_ScriptPath);
			writer.NewPropertyWithValue("FunctionName", m_FunctionName);
		}

		return 0;
	}
}