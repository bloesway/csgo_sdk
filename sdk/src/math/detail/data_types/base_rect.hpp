#pragma once

namespace sdk::detail {
    template < typename _value_t >
        requires std::is_arithmetic_v< _value_t >
    struct base_rect_t {
    private:
        using corner_t = base_vec_t< _value_t, 2u >;

        /* m_mins - upper left | m_maxs - lower right */
        corner_t m_mins{}, m_maxs{};
    public:
        ALWAYS_INLINE constexpr base_rect_t( ) = default;

        ALWAYS_INLINE constexpr base_rect_t(
            const corner_t& min, const corner_t& max
        ) : m_mins{ min }, m_maxs{ max } {}

        ALWAYS_INLINE constexpr base_rect_t(
            const _value_t min_x, const _value_t min_y,
            const _value_t max_x, const _value_t max_y
        ) : m_mins{ min_x, min_y }, m_maxs{ max_x, max_y } {}

        ALWAYS_INLINE constexpr corner_t& mins( ) { return m_mins; }

        ALWAYS_INLINE constexpr const corner_t& mins( ) const { return m_mins; }

        ALWAYS_INLINE constexpr corner_t opp_mins( ) const { return { m_maxs.x( ), m_mins.y( ) }; }

        ALWAYS_INLINE constexpr corner_t& maxs( ) { return m_maxs; }

        ALWAYS_INLINE constexpr const corner_t& maxs( ) const { return m_maxs; }

        ALWAYS_INLINE constexpr corner_t opp_maxs( ) const { return { m_mins.x( ), m_maxs.y( ) }; }
    };
}