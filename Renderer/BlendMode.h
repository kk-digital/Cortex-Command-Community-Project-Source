#ifndef _RTEBLENDMODE_
#define _RTEBLENDMODE_
namespace RTE {
	enum class BlendFunc {
		Add,
		Subtract,
		ReverseSubtract
	};
	enum class BlendFactor : unsigned int {
		Zero = 0,
		One = 1,
		SrcColor = 0x0300,
		OneMinusSrcColor = 0x0301,
		SrcAlpha = 0x0302,
		OneMinusSrcAlpha = 0x0303,
		DestAlpha = 0x0304,
		OneMinusDestAlpha = 0x0305,
		DestColor = 0x0306,
		OneMinusDestColor = 0x0307
	};
	class BlendMode {
	public:
		constexpr BlendMode() = default;
		constexpr BlendMode(BlendFunc srcBlendFunc, BlendFactor srcColorFactor, BlendFactor srcAlphaFactor, BlendFunc dstBlendFunc, BlendFactor dstColorFactor, BlendFactor dstAlphaFactor) :
		    m_SrcBlendFunc{srcBlendFunc}, m_DestBlendFunc{dstBlendFunc}, m_SrcColor{srcColorFactor}, m_DestColor{dstColorFactor}, m_SrcAlpha{srcAlphaFactor}, m_DestAlpha{dstAlphaFactor} {}

		constexpr BlendMode(const BlendMode &blend) = default;
		constexpr BlendMode(BlendMode &&blend) = default;
		~BlendMode() = default;

		constexpr BlendMode &operator=(const BlendMode &) = default;
		constexpr BlendMode &operator=(BlendMode &&) = default;

		void Enable() const;

	private:
		BlendFunc m_SrcBlendFunc = BlendFunc::Add;
		BlendFunc m_DestBlendFunc = BlendFunc::Add;
		BlendFactor m_SrcColor = BlendFactor::One;
		BlendFactor m_DestColor = BlendFactor::Zero;
		BlendFactor m_SrcAlpha = BlendFactor::One;
		BlendFactor m_DestAlpha = BlendFactor::Zero;
	};
	namespace BlendModes {
		static constexpr BlendMode None{};
		static constexpr BlendMode Blend{BlendFunc::Add, BlendFactor::SrcAlpha, BlendFactor::One, BlendFunc::Add, BlendFactor::OneMinusSrcAlpha, BlendFactor::OneMinusSrcAlpha};
		static constexpr BlendMode Screen{BlendFunc::Add, BlendFactor::One, BlendFactor::One, BlendFunc::Add, BlendFactor::OneMinusSrcColor, BlendFactor::OneMinusSrcAlpha};
	} // namespace BlendModes
} // namespace RTE
#endif
