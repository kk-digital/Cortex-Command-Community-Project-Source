#ifndef _RTESHAPE_
#define _RTESHAPE_
#include "VertexArray.h"
namespace RTE {
	class Shape {
	public:
		Shape();
		virtual ~Shape();
	private:
		VertexArray m_Vertices;
		int m_OutlineThickness;
	};
}
#endif
