#include "Icon.h"

namespace RTE {

	ConcreteClassInfo(Icon, Entity, 0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Icon::Clear() {
		m_BitmapFile.Reset();
		m_FrameCount = 0;
		m_TexturesIndexed = 0;
		m_TexturesTrueColor = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Icon::Create() {
		if (!m_TexturesIndexed || !m_TexturesTrueColor) {
			if (m_BitmapFile.GetDataPath().empty()) { m_BitmapFile.SetDataPath("Base.rte/GUIs/DefaultIcon.png"); }

			m_TexturesIndexed = m_BitmapFile.GetAsAnimation(m_FrameCount);
			m_TexturesTrueColor = m_BitmapFile.GetAsAnimation(m_FrameCount);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Icon::Create(const Icon &reference) {
		Entity::Create(reference);

		m_BitmapFile = reference.m_BitmapFile;
		m_FrameCount = reference.m_FrameCount;

		if (reference.m_TexturesIndexed && reference.m_TexturesTrueColor) {
			m_TexturesIndexed = new SDL_Texture *[m_FrameCount];
			m_TexturesTrueColor = new SDL_Texture *[m_FrameCount];

			for (unsigned short frame = 0; frame < m_FrameCount; ++frame) {
				m_TexturesIndexed[frame] = reference.m_TexturesIndexed[frame];
				m_TexturesTrueColor[frame] = reference.m_TexturesTrueColor[frame];
			}
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Icon::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "BitmapFile") {
			reader >> m_BitmapFile;
		} else if (propName == "FrameCount") {
			reader >> m_FrameCount;
		} else {
			return Entity::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Icon::Save(Writer &writer) const {
		Entity::Save(writer);
		writer.NewProperty("BitmapFile");
		writer << m_BitmapFile;
		writer.NewProperty("FrameCount");
		writer << m_FrameCount;

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Icon::Destroy(bool notInherited) {
		delete[] m_TexturesIndexed;
		delete[] m_TexturesTrueColor;

		if (!notInherited) { Entity::Destroy(); }
		Clear();
	}
}
