#include "Box.h"

namespace RTE {

	const std::string Box::c_ClassName = "Box";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Box::Create(const Vector &corner1, const Vector &corner2) {
		m_Corner = corner1;
		m_Width = corner2.m_X - corner1.m_X;
		m_Height = corner2.m_Y - corner1.m_Y;
		Unflip();
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Box::Create(float x1, float y1, float x2, float y2) {
		m_Corner.SetXY(x1, y1);
		m_Width = x2 - x1;
		m_Height = y2 - y1;
		Unflip();
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Box::Create(const Vector &corner, float width, float height) {
		m_Corner = corner;
		m_Width = width;
		m_Height = height;
		Unflip();
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Box::Create(const Box &reference) {
		m_Corner = reference.m_Corner;
		m_Width = reference.m_Width;
		m_Height = reference.m_Height;
		Unflip();
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Box::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "Corner") {
			reader >> m_Corner;
		} else if (propName == "Width") {
			reader >> m_Width;
		} else if (propName == "Height") {
			reader >> m_Height;
		} else {
			return Serializable::ReadProperty(propName, reader);
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int Box::Save(Writer &writer) const {
		Serializable::Save(writer);

		writer.NewPropertyWithValue("Corner", m_Corner);
		writer.NewPropertyWithValue("Width", m_Width);
		writer.NewPropertyWithValue("Height", m_Height);

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Box::Unflip() {
		if (m_Width < 0) {
			m_Width = -m_Width;
			m_Corner.m_X -= m_Width;
		}
		if (m_Height < 0) {
			m_Height = -m_Height;
			m_Corner.m_Y -= m_Height;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool Box::IsWithinBox(const Vector &point) const {
		return !IsEmpty() && (((m_Width > 0 && point.m_X >= m_Corner.m_X && point.m_X < (m_Corner.m_X + m_Width)) ||
			(m_Width < 0 && point.m_X < m_Corner.m_X && point.m_X >= (m_Corner.m_X + m_Width))) &&
			(m_Height > 0 && point.m_Y >= m_Corner.m_Y && point.m_Y < (m_Corner.m_Y + m_Height)) ||
			(m_Height < 0 && point.m_Y < m_Corner.m_Y && point.m_Y <= (m_Corner.m_Y + m_Height)));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool Box::IsWithinBoxX(float pointX) const {
		return !IsEmpty() && ((m_Width > 0 && pointX >= m_Corner.m_X && pointX < (m_Corner.m_X + m_Width)) ||
			(m_Width < 0 && pointX < m_Corner.m_X && pointX >= (m_Corner.m_X + m_Width)));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool Box::IsWithinBoxY(float pointY) const {
		return !IsEmpty() && ((m_Height > 0 && pointY >= m_Corner.m_Y && pointY < (m_Corner.m_Y + m_Height)) ||
			(m_Height < 0 && pointY < m_Corner.m_Y && pointY <= (m_Corner.m_Y + m_Height)));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float Box::GetWithinBoxX(float pointX) const {
		if (m_Width > 0) {
			return Limit(pointX, m_Corner.m_X + m_Width - 1, m_Corner.m_X);
		} else if (m_Width < 0) {
			return Limit(pointX, m_Corner.m_X - 1, m_Corner.m_X + m_Width);
		}
		return m_Corner.m_X;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float Box::GetWithinBoxY(float pointY) const {
		if (m_Height > 0) {
			return Limit(pointY, m_Corner.m_Y + m_Height - 1, m_Corner.m_Y);
		} else if (m_Height < 0) {
			return Limit(pointY, m_Corner.m_Y - 1, m_Corner.m_Y + m_Height);
		}
		return m_Corner.m_Y;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool Box::IntersectsBox(const Box &rhs) {
		if (IsEmpty() || rhs.IsEmpty()) {
			return false;
		}
		Box box1 = *this;
		Box box2 = rhs;
		box1.Unflip();
		box2.Unflip();
		return (box1.m_Corner.m_X < box2.m_Corner.m_X + box2.m_Width) && (box1.m_Corner.m_X + box1.m_Width > box2.m_Corner.m_X) &&
			(box1.m_Corner.m_Y < box2.m_Corner.m_Y + box2.m_Height) && (box1.m_Corner.m_Y + box1.m_Height > box2.m_Corner.m_Y);
	}

	Box Box::GetIntersection(const Box &rhs) {
		Box intersection = *this;
		intersection.Unflip();
		Box tester = rhs;
		tester.Unflip();

		int aMinX = intersection.m_Corner.m_X;
		int aMaxX = intersection.m_Corner.m_X + intersection.m_Width;
		int bMinX = tester.m_Corner.m_X;
		int bMaxX = tester.m_Corner.m_X + tester.m_Width;

		if (bMinX > aMinX) {
			aMinX = bMinX;
		}
		intersection.m_Corner.m_X = aMinX;

		if (bMaxX < aMaxX) {
			aMaxX = bMaxX;
		}
		intersection.m_Width = aMaxX - aMinX;

		int aMinY = intersection.m_Corner.m_Y;
		int aMaxY = intersection.m_Corner.m_Y + intersection.m_Height;
		int bMinY = tester.m_Corner.m_Y;
		int bMaxY = tester.m_Corner.m_Y + tester.m_Height;

		if (bMinY > aMinY) {
			aMinY = bMinY;
		}
		intersection.m_Corner.m_Y = aMinY;

		if (bMaxY < aMaxY){
			aMaxY = bMaxY;
		}
		intersection.m_Height = aMaxY - aMinY;

		return intersection;
	}
}
