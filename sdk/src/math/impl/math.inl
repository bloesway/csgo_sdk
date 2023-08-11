#pragma once

#include "../math.hpp"

namespace sdk {
    template < typename _value_t >
        requires std::is_arithmetic_v< _value_t >
    ALWAYS_INLINE constexpr auto to_deg( const _value_t rad ) {
        using ret_t = detail::enough_float_t< _value_t >;

        return static_cast< ret_t >( rad * k_rad_pi< ret_t > );
    }

    template < typename _value_t >
        requires std::is_arithmetic_v< _value_t >
    ALWAYS_INLINE constexpr auto to_rad( const _value_t deg ) {
        using ret_t = detail::enough_float_t< _value_t >;

        return static_cast< ret_t >( deg * k_deg_pi< ret_t > );
    }

    template < typename _value_t >
        requires is_addable< _value_t, _value_t > && is_multipliable< _value_t, float >
    ALWAYS_INLINE constexpr _value_t lerp( const _value_t& from, const _value_t& to, const float amt ) {
        return from * ( 1.f - amt ) + to * amt;
    }

    template < typename _value_t >
        requires std::is_floating_point_v< _value_t >
    ALWAYS_INLINE constexpr _value_t normalize_angle( const _value_t angle ) {
        return std::remainder( angle, static_cast< _value_t >( 360.0 ) );
    }
}