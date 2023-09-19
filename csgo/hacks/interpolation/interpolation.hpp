#pragma once

namespace hacks {
	class c_interpolation {
	public:
		void handle( bool post );

		void skip( );
		void process( );

		bool	m_skip{};
		float	m_amount{};
	};

	inline const auto g_interpolation = std::make_unique< c_interpolation >( );
}