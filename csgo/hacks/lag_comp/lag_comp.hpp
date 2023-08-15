#pragma once

namespace hacks {
	class c_lag_comp {
	public:
		void handle( );
	};

	inline const auto g_lag_comp = std::make_unique< c_lag_comp >( );
}