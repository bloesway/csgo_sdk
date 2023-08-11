#pragma once

#include "detail/detail.hpp"

#define BYTESEQ( value ) \
    [ & ] ( ) { \
        static_assert( std::is_same_v< const char*, std::decay_t< decltype( value ) > > || std::is_pointer_v< decltype( value ) > ); \
    \
        constexpr auto k_size = sdk::detail::byte_seq_t< sizeof( value ) >::measure( value ); \
    \
        return sdk::detail::byte_seq_t< k_size >{ [ & ] ( ) { return value; }, std::make_index_sequence< k_size >{} }; \
    }( ) \

#define HASH( str ) [ ] ( ) { constexpr auto k_hash = sdk::hash( str ); return k_hash; }( )

namespace sdk {
    using hash_t = std::size_t;

    using ulong_t = unsigned long;

    template < typename _type >
    inline constexpr auto is_char_v = std::is_same_v< _type, char > || std::is_same_v< _type, char16_t >
        || std::is_same_v< _type, char32_t > || std::is_same_v< _type, wchar_t >;

#if defined( _WIN32 ) || defined( _WIN64 )
    ALWAYS_INLINE std::string to_multi_byte( const std::wstring_view str );

    ALWAYS_INLINE std::wstring to_wide_char( const std::string_view str );
#endif

    template < typename _char_t >
        requires is_char_v< _char_t >
    ALWAYS_INLINE constexpr hash_t hash( const _char_t* const str, const std::size_t len );

    template < typename _char_t >
        requires is_char_v< _char_t >
    ALWAYS_INLINE constexpr hash_t hash( const _char_t* const str );
}

#include "impl/util.inl"