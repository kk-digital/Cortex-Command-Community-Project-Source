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
		OneMinusSrcColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DestAlpha,
		OneMinusDestAlpha,
		DestColor,
		OneMinusDestColor
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
		BlendFactor m_SrcColor = BlendFactor::Zero;
		BlendFactor m_DestColor = BlendFactor::One;
		BlendFactor m_SrcAlpha = BlendFactor::Zero;
		BlendFactor m_DestAlpha = BlendFactor::One;
	};
	namespace BlendModes {
		static constexpr BlendMode None{};
		static constexpr BlendMode Blend{BlendFunc::Add, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendFunc::Add, BlendFactor::One, BlendFactor::OneMinusSrcAlpha};
		static constexpr BlendMode Screen{BlendFunc::Add, BlendFactor::One, BlendFactor::OneMinusSrcColor, BlendFunc::Add, BlendFactor::One, BlendFactor::OneMinusSrcAlpha};
	} // namespace BlendModes
} // namespace RTE
#endif
