#pragma once

#include "../mem.hpp"

namespace sdk {
#if defined( _WIN32 ) || defined( _WIN64 )
    ALWAYS_INLINE void peb_t::for_each_ldr_data_table_entry(
        const std::function< bool( ldr_data_table_entry_t* const ) >& fn, const e_ldr_data_table type
    ) {
        const auto list = &m_ldr->m_module_lists.m_entries[ -type ];

        for ( auto i = list->Flink; i != list; i = i->Flink ) {
            const auto entry = CONTAINING_RECORD( i, ldr_data_table_entry_t, m_links.m_entries[ -type ] );
            if ( !entry )
                continue;

            if ( fn( entry ) )
                break;
        }
    }

    ALWAYS_INLINE peb_t* peb( ) {
#if defined( _M_X64 ) || defined( __amd64__ )
        return reinterpret_cast< peb_t* >( __readgsqword( 0x60ul ) );
#elif defined( _M_IX86 ) || defined( __i386__ )
        return reinterpret_cast< peb_t* >( __readfsdword( 0x30ul ) );
#elif defined( _M_ARM ) || defined( __arm__ )
        return *reinterpret_cast< peb_t** >( _MoveFromCoprocessor( 15, 0, 13, 0, 2 ) + 0x30 );
#elif defined( _M_ARM64 ) || defined( __aarch64__ )
        return *reinterpret_cast< peb_t** >( __getReg( 18 ) + 0x60 );
#elif defined( _M_IA64 ) || defined( __ia64__ )
        return *reinterpret_cast< peb_t** >( static_cast< std::uint8_t* >( _rdteb( ) ) + 0x60u );
#elif defined( _M_ALPHA )
        return *reinterpret_cast< peb_t** >( static_cast< std::uint8_t* >( _rdteb( ) ) + 0x30u );
#elif defined( _M_MIPS )
        return *reinterpret_cast< peb_t** >( *reinterpret_cast< std::uintptr_t* >( 0x7ffff030u ) + 0x30u );
#elif defined( _M_PPC )
        return *reinterpret_cast< peb_t** >( __gregister_get( 13 ) + 0x30 );
#else
#error "unsupported architecture"
#endif
    }
#endif
}