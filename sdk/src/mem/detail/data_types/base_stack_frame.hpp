#pragma once

namespace sdk::detail {
    template < typename _addr_t >
        requires std::is_unsigned_v< _addr_t >
    struct base_stack_frame_t final : public base_address_t< _addr_t > {
    private:
        using base_t = base_address_t< _addr_t >;
    public:
        ALWAYS_INLINE constexpr base_stack_frame_t(
            const base_t addr = reinterpret_cast< _addr_t >( _AddressOfReturnAddress( ) ) - sizeof( _addr_t )
        ) : base_t{ addr } {}

        ALWAYS_INLINE base_t ret_addr( ) const { return addr_of_ret( ).self_deref( 1u ); }

        ALWAYS_INLINE constexpr base_t addr_of_ret( ) const { return base_t::offset( sizeof( _addr_t ) ); }

        ALWAYS_INLINE base_stack_frame_t< _addr_t >& self_next( ) {
            return static_cast< base_stack_frame_t< _addr_t >& >( base_t::self_deref( 1u ) );
        }

        ALWAYS_INLINE base_stack_frame_t< _addr_t > next( ) const { return base_t::deref( 1u ); }
    };
}