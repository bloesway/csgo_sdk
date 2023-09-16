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

    /* hell no.... */
    template < typename _value_t >
        requires std::is_arithmetic_v< _value_t >
    ALWAYS_INLINE constexpr auto normalize_yaw( const _value_t value ) {
        auto ret = value;

        if ( std::isnan( ret ) || std::isinf( ret ) )
            ret = 0.f;

        while ( ret < -180.f )
            ret += 360.f;

        while ( ret > 180.f )
            ret -= 360.f;

        return ret;
    }

    template < typename _value_t >
        requires is_addable< _value_t, _value_t >&& is_multipliable< _value_t, float >
    ALWAYS_INLINE constexpr _value_t lerp( const _value_t& from, const _value_t& to, const float amt ) {
        return from + ( to - from ) * amt;
    }

    template < typename _value_t >
        requires is_addable< _value_t, _value_t >&& is_multipliable< _value_t, float >
    ALWAYS_INLINE constexpr _value_t lerp( const _value_t& from, const _value_t& to,
        const int step, const int max
    ) {
        return from + ( ( ( to - from ) / max ) * step );
    }

    template < typename _value_t >
        requires std::is_floating_point_v< _value_t >
    ALWAYS_INLINE constexpr _value_t normalize_angle( const _value_t angle ) {
        return std::remainder( angle, static_cast< _value_t >( 360.0 ) );
    }
}