#pragma once

namespace valve {
    /* should be in interfaces.hpp tbh */
    class c_mem_alloc {
    public:
        VFUNC( void*( __thiscall* )( decltype( this ), std::size_t ), alloc( std::size_t size ), 1u, size );

        VFUNC( void*( __thiscall* )( decltype( this ), void*, std::size_t ),
           realloc( sdk::address_t addr, std::size_t size ), 3u, addr.as< void* >( ), size
        );

        VFUNC( void( __thiscall* )( decltype( this ), void* ), free( sdk::address_t addr ), 5u, addr.as< void* >( ) );
    } inline* g_mem_alloc{};

    template < typename _value_t, typename _index_t >
    struct utl_mem_t {
    private:
        _value_t*   m_ptr{};
        int         m_alloc_count{};
        int         m_grow_size{};
    public:
        ALWAYS_INLINE _value_t& at( const _index_t i );

        ALWAYS_INLINE const _value_t& at( const _index_t i ) const;

        ALWAYS_INLINE void clear( );

        ALWAYS_INLINE void grow( const int count );

        ALWAYS_INLINE int alloc_count( ) const;
    };

    template < typename _value_t >
    struct utl_vec_t {
    private:    
        utl_mem_t< _value_t, int >  m_mem{};
        int                         m_size{};
        _value_t*                   m_elements{};
    public:
        ALWAYS_INLINE constexpr utl_vec_t( ) = default;

        ALWAYS_INLINE _value_t& at( const int i );

        ALWAYS_INLINE const _value_t& at( const int i ) const;

        ALWAYS_INLINE void clear( );

        ALWAYS_INLINE void reserve( const int size );

        ALWAYS_INLINE int size( ) const;

        ALWAYS_INLINE utl_vec_t< _value_t >& operator =( const utl_vec_t< _value_t >& other );
    };

    struct key_values_t {
        using get_symbol_proc_t = bool( __cdecl* )( const char* );

#ifdef CSGO2018
        OFFSET_VFUNC( void( __thiscall* )( decltype( this ), const char* ),
            init( const char* key ), g_ctx->offsets( ).m_key_values.m_init, key
        );

        OFFSET_VFUNC( bool( __thiscall* )( decltype( this ), const char*, const char*, void*, const char*, get_symbol_proc_t ),
            load_from_buffer(
                const char* name, const char* buffer, void* file_system = nullptr,
                const char* path_id = nullptr, get_symbol_proc_t symbol_proc = nullptr
            ),
            g_ctx->offsets( ).m_key_values.m_load_from_buffer, name, buffer, file_system, path_id, symbol_proc
        );
#else
        OFFSET_VFUNC( void( __thiscall* )( decltype( this ), const char*, int, int ),
            init( const char* key ), g_ctx->offsets( ).m_key_values.m_init, key, 0, 0
        );

        OFFSET_VFUNC( bool( __thiscall* )( decltype( this ), const char*, const char*, void*, const char*, get_symbol_proc_t, int ),
            load_from_buffer(
                const char* name, const char* buffer, void* file_system = nullptr,
                const char* path_id = nullptr, get_symbol_proc_t symbol_proc = nullptr
            ),
            g_ctx->offsets( ).m_key_values.m_load_from_buffer, name, buffer, file_system, path_id, symbol_proc, 0
        );
#endif

        std::uint32_t       m_key_name : 24u,
                            m_key_name_case_sensitive1 : 8u;

        char*               m_str{};
        wchar_t*            m_wstr{};

        union {
            int             m_int;
            float           m_float;
            sdk::address_t  m_ptr;
            sdk::argb_t     m_clr{};
        };

        char                m_data_type{},
                            m_has_esc_sequences{};
        std::uint16_t       m_key_name_case_sensitive2{};
#ifndef CSGO2018
        std::uint8_t        pad0[ 8u ]{};
#endif
        key_values_t*       m_peer{}, *m_sub{}, *m_chain{};

        get_symbol_proc_t   m_get_symbol_proc_fn{};
    };
}

#include "impl/util.inl"