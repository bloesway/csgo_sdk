#pragma once

namespace sdk::detail {
    struct base_cfg_var_t {
    protected:
        hash_t m_hash{};
    public:
        ALWAYS_INLINE constexpr base_cfg_var_t( ) = default;

        ALWAYS_INLINE constexpr base_cfg_var_t( const hash_t hash )
            : m_hash{ hash } {}

        virtual void save( nlohmann::json& object ) const = 0;

        virtual void load( const nlohmann::json& object ) = 0;

        ALWAYS_INLINE hash_t hash( ) const { return m_hash; }
    };
}