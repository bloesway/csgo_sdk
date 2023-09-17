#pragma once

#include "detail/detail.hpp"

namespace sdk {
    template < typename _value_t >
    struct cfg_var_t final : public detail::base_cfg_var_t {
    private:
        _value_t m_value{};
    public:
        ALWAYS_INLINE constexpr cfg_var_t( ) = default;

        ALWAYS_INLINE cfg_var_t( const hash_t hash, const _value_t value );

        ALWAYS_INLINE _value_t* operator &( );

        ALWAYS_INLINE operator _value_t( ) const;

        ALWAYS_INLINE _value_t* operator ->( );

        ALWAYS_INLINE const _value_t* operator ->( ) const;

        ALWAYS_INLINE _value_t& get( );

        ALWAYS_INLINE const _value_t& get( ) const;

        void save( nlohmann::json& object ) const override;

        void load( const nlohmann::json& object ) override;
    };

    class c_cfg {
    private:
        static constexpr std::uint8_t k_byte_xor = 0xffu;

        std::vector< detail::base_cfg_var_t* > m_vars{};
    public:
        virtual ~c_cfg( ) = default;

        void save( const std::string_view name ) const;

        void load( const std::string_view name );

        ALWAYS_INLINE void add_var( detail::base_cfg_var_t* const var );

        ALWAYS_INLINE detail::base_cfg_var_t* find_var( const hash_t hash ) const;
    };

    inline const auto g_cfg = std::make_unique< c_cfg >( );
}

#include "impl/cfg.inl"