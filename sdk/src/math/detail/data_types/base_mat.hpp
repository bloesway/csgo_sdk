#pragma once

namespace sdk::detail {
    /* the basic funcs that each matrix should have */
    template < typename _value_t, std::size_t _rows_count, std::size_t _columns_count, typename _derived_t >
    struct mat_helper_t {
    protected:
        using row_t = base_vec_t< _value_t, _columns_count >;

        using column_t = base_vec_t< _value_t, _rows_count >;

        _value_t m_elements[ _rows_count * _columns_count ]{};
    public:
        ALWAYS_INLINE constexpr mat_helper_t( ) = default;

        template < typename... _args_t >
            requires ( sizeof...( _args_t ) <= ( _rows_count * _columns_count ) )
        ALWAYS_INLINE constexpr mat_helper_t( const _args_t&... args ) : m_elements{ args... } {}

        ALWAYS_INLINE constexpr _value_t& at( const std::size_t row, const std::size_t column ) {
            return m_elements[ row * _columns_count + column ];
        }

        ALWAYS_INLINE constexpr _value_t at( const std::size_t row, const std::size_t column ) const {
            return m_elements[ row * _columns_count + column ];
        }

        ALWAYS_INLINE row_t& row( const std::size_t i ) {
            return *reinterpret_cast< row_t* >( &m_elements[ i * _columns_count ] );
        }

        ALWAYS_INLINE const row_t& row( const std::size_t i ) const {
            return *reinterpret_cast< const row_t* >( &m_elements[ i * _columns_count ] );
        }

        template < typename _rhs_t >
            requires std::is_arithmetic_v< _rhs_t >
        ALWAYS_INLINE _derived_t& operator -=( const _rhs_t rhs ) {
            for ( auto& element : m_elements )
                element -= rhs;

            return static_cast< _derived_t& >( *this );
        }

        template < typename _rhs_t >
            requires std::is_arithmetic_v< _rhs_t >
        ALWAYS_INLINE _derived_t& operator +=( const _rhs_t rhs ) {
            for ( auto& element : m_elements )
                element += rhs;

            return static_cast< _derived_t& >( *this );
        }

        template < typename _rhs_t >
            requires std::is_arithmetic_v< _rhs_t >
        ALWAYS_INLINE _derived_t& operator *=( const _rhs_t rhs ) {
            for ( auto& element : m_elements )
                element *= rhs;

            return static_cast< _derived_t& >( *this );
        }

        ALWAYS_INLINE _derived_t& operator -=( const _derived_t& rhs ) const {
            for ( std::size_t i{}; i < _rows_count; ++i )
                for ( std::size_t j{}; j < _columns_count; ++j )
                    at( i, j ) -= rhs.at( i, j );

            return static_cast< _derived_t& >( *this );
        }

        ALWAYS_INLINE _derived_t& operator +=( const _derived_t& rhs ) const {
            for ( std::size_t i{}; i < _rows_count; ++i )
                for ( std::size_t j{}; j < _columns_count; ++j )
                    at( i, j ) += rhs.at( i, j );

            return static_cast< _derived_t& >( *this );
        }

        ALWAYS_INLINE _derived_t& operator *=(
            const base_mat_t< _value_t, _columns_count, _columns_count >& rhs
        ) {
            auto lhs = *this;

            for ( auto& element : m_elements )
                element = static_cast< _value_t >( 0 );

            constexpr auto k_min_size = std::min( _rows_count, _columns_count );

            for ( std::size_t i{}; i < _rows_count; ++i )
                for ( std::size_t j{}; j < _columns_count; ++j )
                    for ( std::size_t k{}; k < k_min_size; ++k )
                        at( i, j ) += lhs.at( i, k ) * rhs.at( k, j );

            return static_cast< _derived_t& >( *this );
        }

        template < typename _rhs_t >
            requires std::is_arithmetic_v< _rhs_t >
        ALWAYS_INLINE _derived_t operator -( const _rhs_t rhs ) const {
            auto ret = *this;

            return ret -= rhs;
        }

        template < typename _rhs_t >
            requires std::is_arithmetic_v< _rhs_t >
        ALWAYS_INLINE _derived_t operator +( const _rhs_t rhs ) const {
            auto ret = *this;

            return ret += rhs;
        }

        template < typename _rhs_t >
            requires std::is_arithmetic_v< _rhs_t >
        ALWAYS_INLINE _derived_t operator *( const _rhs_t rhs ) const {
            auto ret = *this;

            return ret *= rhs;
        }

        template < std::size_t _rhs_columns_count >
        ALWAYS_INLINE base_mat_t< _value_t, _rows_count, _rhs_columns_count > operator *(
            const base_mat_t< _value_t, _columns_count, _rhs_columns_count >& rhs
        ) const {
            if constexpr ( _columns_count == _rhs_columns_count ) {
                auto ret = *this;

                return ret *= rhs;
            }

            base_mat_t< _value_t, _rows_count, _rhs_columns_count > ret{};

            constexpr auto k_min_size = std::min( _rhs_columns_count, _columns_count );

            for ( std::size_t i{}; i < _rows_count; ++i )
                for ( std::size_t j{}; j < _rhs_columns_count; ++j )
                    for ( std::size_t k{}; k < k_min_size; ++k )
                        ret.at( i, j ) += at( i, k ) * rhs.at( k, j );

            return ret;
        }

        ALWAYS_INLINE static constexpr _derived_t identity( ) {
            _derived_t ret{};

            constexpr auto k_min_size = std::min( _rows_count, _columns_count );
            for ( std::size_t i{}; i < k_min_size; ++i )
                ret.at( i, i ) = static_cast< _value_t >( 1 );

            return ret;
        }
    };

    template < typename _value_t, std::size_t _rows_count, std::size_t _columns_count >
    struct base_mat_t final
        : public mat_helper_t< _value_t, _rows_count, _columns_count, base_mat_t< _value_t, _rows_count, _columns_count > > {
    private:
        using base_t = mat_helper_t< _value_t, _rows_count, _columns_count, base_mat_t< _value_t, _rows_count, _columns_count > >;
    public:
        ALWAYS_INLINE constexpr base_mat_t( ) = default;

        template < typename... _args_t >
            requires ( sizeof...( _args_t ) <= ( _rows_count * _columns_count ) )
        ALWAYS_INLINE constexpr base_mat_t( const _args_t&... args ) : base_t{ args... } {}
    };

    template < typename _value_t >
    struct base_mat_t< _value_t, 3u, 4u > final
        : public mat_helper_t< _value_t, 3u, 4u, base_mat_t< _value_t, 3u, 4u > > {
    private:
        using base_t = mat_helper_t< _value_t, 3u, 4u, base_mat_t< _value_t, 3u, 4u > >;
    public:
        ALWAYS_INLINE constexpr base_mat_t( ) = default;

        template < typename... _args_t >
            requires ( sizeof...( _args_t ) <= 12u )
        ALWAYS_INLINE constexpr base_mat_t( const _args_t&... args ) : base_t{ args... } {}

        ALWAYS_INLINE constexpr base_mat_t< _value_t, 3u, 4u >& operator *=(
            const base_mat_t< _value_t, 3u, 4u >& rhs
        ) {
            auto lhs = *this;

            for ( auto& element : base_t::m_elements )
                element = static_cast< _value_t >( 0 );

            for ( std::size_t i{}; i < 3u; ++i )
                for ( std::size_t j{}; j < 4u; ++j ) {
                    if ( j == 3u )
                        base_t::at( i, j ) += lhs.at( i, 3u );

                    for ( std::size_t k{}; k < 3u; ++k )
                        base_t::at( i, j ) += lhs.at( i, k ) * rhs.at( k, j );
                }

            return *this;
        }

        ALWAYS_INLINE constexpr base_mat_t< _value_t, 3u, 4u > operator *(
            const base_mat_t< _value_t, 3u, 4u >& rhs
        ) {
            auto ret = *this;

            return ret *= rhs;
        }
    };
}