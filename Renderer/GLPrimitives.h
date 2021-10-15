#ifndef _RTEGLPRIMITIVES_
#define _RTEGLPRIMITIVES_
namespace RTE {
	enum class PrimitiveType : unsigned int {
		Point = 0x0000,
		Line = 0x0001,
		LineLoop = 0x0002,
		LineStrip = 0x0003,
		Triangle = 0x0004,
		TriangleStrip = 0x0005,
		TriangleFan = 0x0006,
	};
}
#endif
