#pragma once

namespace sdk::detail {
    /* the basic funcs that each vector should have */
    template < typename _value_t, std::size_t _size, typename _derived_t >
        requires ( _size >= 2u )
    struct vec_helper_t : public array_wrapper_t< _value_t, _size, _derived_t > {
    private:
        using base_t = array_wrapper_t< _value_t, _size, _derived_t >;
    public:
        ALWAYS_INLINE constexpr vec_helper_t( ) = default;

        template < typename... _args_t >
            requires ( sizeof...( _args_t ) <= _size )
        ALWAYS_INLINE constexpr vec_helper_t( const _args_t&... args ) : base_t{ args... } {}

        template < std::size_t _other_size, typename _other_derived_t >
        ALWAYS_INLINE constexpr _value_t dot(
            const array_wrapper_t< _value_t, _other_size, _other_derived_t >& other
        ) const {
            _value_t ret{};

            constexpr auto k_min_size = std::min( _size, _other_size );
            for ( std::size_t i{}; i < k_min_size; ++i )
                ret += base_t::at( i ) * other.at( i );

            return ret;
        }

        ALWAYS_INLINE constexpr _value_t length_sqr( const std::size_t size = _size ) const {
            _value_t ret{};

            const auto min_size = std::clamp( size, 2u, _size );
            for ( std::size_t i{}; i < min_size; ++i )
                ret += base_t::at( i ) * base_t::at( i );

            return ret;
        }

        ALWAYS_INLINE _value_t length( const std::size_t size = _size ) const {
            return std::sqrt( length_sqr( size ) );
        }

        ALWAYS_INLINE _value_t normalize( ) {
            const auto len = length( );
            if ( len )
                *this /= len;

            return len;
        }

        ALWAYS_INLINE _derived_t normalized( ) const {
            auto ret = *static_cast< const _derived_t* >( this );

            ret.normalize( );

            return ret;
        }
    };

    template < typename _value_t, std::size_t _size >
        requires ( _size >= 2u )
    struct base_vec_t final
        : public vec_helper_t< _value_t, _size, base_vec_t< _value_t, _size > > {
    private:
        using base_t = vec_helper_t< _value_t, _size, base_vec_t< _value_t, _size > >;
    public:
        ALWAYS_INLINE constexpr base_vec_t( ) = default;

        template < typename... _args_t >
            requires ( sizeof...( _args_t ) <= _size )
        ALWAYS_INLINE constexpr base_vec_t( const _args_t&... args ) : base_t{ args... } {}
    };

    template < typename _value_t >
    struct base_vec_t< _value_t, 2u > final
        : public vec_helper_t< _value_t, 2u, base_vec_t< _value_t, 2u > > {
    private:
        using base_t = vec_helper_t< _value_t, 2u, base_vec_t< _value_t, 2u > >;
    public:
        ALWAYS_INLINE constexpr base_vec_t( ) = default;

        ALWAYS_INLINE constexpr base_vec_t( const _value_t value )
            : base_t{ value, value } {}

        ALWAYS_INLINE constexpr base_vec_t(
            const _value_t x, const _value_t y
        ) : base_t{ x, y } {}

        ALWAYS_INLINE constexpr _value_t& x( ) { return base_t::at( 0u ); }

        ALWAYS_INLINE constexpr _value_t x( ) const { return base_t::at( 0u ); }

        ALWAYS_INLINE constexpr _value_t& y( ) { return base_t::at( 1u ); }

        ALWAYS_INLINE constexpr _value_t y( ) const { return base_t::at( 1u ); }
    };

    template < typename _value_t >
    struct base_vec_t< _value_t, 3u > final
        : public vec_helper_t< _value_t, 3u, base_vec_t< _value_t, 3u > > {
    private:
        using base_t = vec_helper_t< _value_t, 3u, base_vec_t< _value_t, 3u > >;
    public:
        ALWAYS_INLINE constexpr base_vec_t( ) = default;

        ALWAYS_INLINE constexpr base_vec_t( const _value_t value )
            : base_t{ value, value, value } {}

        ALWAYS_INLINE constexpr base_vec_t(
            const _value_t x, const _value_t y, const _value_t z
        ) : base_t{ x, y, z } {}

        ALWAYS_INLINE constexpr _value_t& x( ) { return base_t::at( 0u ); }

        ALWAYS_INLINE constexpr _value_t x( ) const { return base_t::at( 0u ); }

        ALWAYS_INLINE constexpr _value_t& y( ) { return base_t::at( 1u ); }

        ALWAYS_INLINE constexpr _value_t y( ) const { return base_t::at( 1u ); }

        ALWAYS_INLINE constexpr _value_t& z( ) { return base_t::at( 2u ); }

        ALWAYS_INLINE constexpr _value_t z( ) const { return base_t::at( 2u ); }

        ALWAYS_INLINE constexpr base_vec_t< _value_t, 3u > cross(
            const base_vec_t< _value_t, 3u >& other
        ) const {
            return {
                y( ) * other.z( ) - z( ) * other.y( ),
                z( ) * other.x( ) - x( ) * other.z( ),
                x( ) * other.y( ) - y( ) * other.x( )
            };
        }

        ALWAYS_INLINE base_qang_t< _value_t > angles(
            base_vec_t< _value_t, 3u >* const up = nullptr
        ) const {
            base_qang_t< _value_t > ret{};

            const auto len_2d = base_t::length( 2u );
            if ( up && len_2d > 0.001f ) {
                ret.x( ) = to_deg( std::atan2( -z( ), len_2d ) );
                ret.y( ) = to_deg( std::atan2( y( ), x( ) ) );

                auto left = ( *up ).cross( *this );

                left.normalize( );

                const auto up_z = ( left.y( ) * x( ) ) - ( left.x( ) * y( ) );

                ret.z( ) = to_deg( std::atan2( left.z( ), up_z ) );
            }
            else {
                if ( len_2d > 0.f ) {
                    ret.x( ) = to_deg( std::atan2( -z( ), len_2d ) );
                    ret.y( ) = to_deg( std::atan2( y( ), x( ) ) );
                }
                else
                    ret.x( ) = z( ) > 0.f ? -90.f : 90.f;
            }

            return ret;
        }

        ALWAYS_INLINE constexpr base_vec_t< _value_t, 3u > transform(
            const base_mat_t< _value_t, 3u, 4u >& matrix
        ) const {
            return {
                base_t::dot( matrix.row( 0u ) ) + matrix.row( 0u ).at( 3u ),
                base_t::dot( matrix.row( 1u ) ) + matrix.row( 1u ).at( 3u ),
                base_t::dot( matrix.row( 2u ) ) + matrix.row( 2u ).at( 3u )
            };
        }

        ALWAYS_INLINE constexpr base_vec_t< _value_t, 3u > i_transform(
            const base_mat_t< _value_t, 3u, 4u >& matrix
        ) const {
            const auto& row0 = matrix.row( 0u );
            const auto& row1 = matrix.row( 1u );
            const auto& row2 = matrix.row( 2u );

            return {
                ( x( ) - row0.at( 3u ) ) * row0.at( 0u ) + ( y( ) - row1.at( 3u ) ) * row1.at( 0u ) + ( z( ) - row2.at( 3u ) ) * row2.at( 0u ),
                ( x( ) - row0.at( 3u ) ) * row0.at( 1u ) + ( y( ) - row1.at( 3u ) ) * row1.at( 1u ) + ( z( ) - row2.at( 3u ) ) * row2.at( 1u ),
                ( x( ) - row0.at( 3u ) ) * row0.at( 2u ) + ( y( ) - row1.at( 3u ) ) * row1.at( 2u ) + ( z( ) - row2.at( 3u ) ) * row2.at( 2u )
            };
        }

        ALWAYS_INLINE constexpr base_vec_t< _value_t, 3u > rotate(
            const base_mat_t< _value_t, 3u, 4u >& matrix
        ) const {
            return { base_t::dot( matrix.row( 0u ) ), base_t::dot( matrix.row( 1u ) ), base_t::dot( matrix.row( 2u ) ) };
        }

        ALWAYS_INLINE constexpr base_vec_t< _value_t, 3u > i_rotate(
            const base_mat_t< _value_t, 3u, 4u >& matrix
        ) const {
            const auto& row0 = matrix.row( 0u );
            const auto& row1 = matrix.row( 1u );
            const auto& row2 = matrix.row( 2u );

            return {
                x( ) * row0.at( 0u ) + y( ) * row1.at( 0u ) + z( ) * row2.at( 0u ),
                x( ) * row0.at( 1u ) + y( ) * row1.at( 1u ) + z( ) * row2.at( 1u ),
                x( ) * row0.at( 2u ) + y( ) * row1.at( 2u ) + z( ) * row2.at( 2u )
            };
        }
    };
}