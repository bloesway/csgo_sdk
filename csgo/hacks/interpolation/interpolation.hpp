#pragma once

namespace hacks {
	class c_interpolation {
	public:
		void handle( bool post );

		void skip( );
		void process( );

		bool	m_skip = false;
		float	m_amount = 0.f;
	};

	inline const auto g_interpolation = std::make_unique< c_interpolation >( );
}