#pragma once

#include "../util.hpp"

namespace valve {
    template < typename _value_t, typename _index_t >
    ALWAYS_INLINE _value_t& utl_mem_t< _value_t, _index_t >::at( const _index_t i ) { return m_ptr[ i ]; }

    template < typename _value_t, typename _index_t >
    ALWAYS_INLINE const _value_t& utl_mem_t< _value_t, _index_t >::at( const _index_t i ) const { return m_ptr[ i ]; }

    template < typename _value_t, typename _index_t >
    ALWAYS_INLINE void utl_mem_t< _value_t, _index_t >::grow( const int count ) {
        if ( m_grow_size < 0 )
            return;

        const auto requested = m_alloc_count + count;

        const auto calc_new_alloc_count = [ ] ( int count, const int grow_size, const int requested, const int bytes ) {
            if ( grow_size )
               return ( 1 + ( requested - 1 ) / grow_size ) * grow_size;

            if ( !count )
                count = ( 31 + bytes ) / bytes;

            while ( count < requested )
                count *= 2;

            return count;
        };

        auto new_alloc_count = calc_new_alloc_count( m_alloc_count, m_grow_size, requested, sizeof( _value_t ) );

        if ( static_cast< int >( static_cast< _index_t >( new_alloc_count ) ) < requested ) {
            if ( static_cast< int >( static_cast< _index_t >( new_alloc_count ) ) == 0
                && static_cast< int >( static_cast< _index_t >( new_alloc_count - 1 ) ) >= requested )
                --new_alloc_count;
            else {
                if ( static_cast< int >( static_cast< _index_t >( requested ) ) != requested )
                    return;

                while ( static_cast< int >( static_cast< _index_t >( new_alloc_count ) ) < requested )
                    new_alloc_count = ( new_alloc_count + requested ) / 2;
            }
        }

        m_alloc_count = new_alloc_count;

        m_ptr = reinterpret_cast< _value_t* >( m_ptr
            ? g_mem_alloc->realloc( m_ptr, m_alloc_count * sizeof( _value_t ) )
            : g_mem_alloc->alloc( m_alloc_count * sizeof( _value_t ) )
        );
    }

    template < typename _value_t, typename _index_t >
    ALWAYS_INLINE void utl_mem_t< _value_t, _index_t >::clear( ) {
        if ( m_grow_size < 0 )
            return;

        if ( m_ptr ) {
            g_mem_alloc->free( m_ptr );

            m_ptr = nullptr;
        }

        m_alloc_count = 0;
    }

    template < typename _value_t, typename _index_t >
    ALWAYS_INLINE int utl_mem_t< _value_t, _index_t >::alloc_count( ) const { return m_alloc_count; }

    template < typename _value_t >
    ALWAYS_INLINE _value_t& utl_vec_t< _value_t >::at( const int i ) { return m_mem.at( i ); }

    template < typename _value_t >
    ALWAYS_INLINE const _value_t& utl_vec_t< _value_t >::at( const int i ) const { return m_mem.at( i ); }

    template < typename _value_t >
    ALWAYS_INLINE void utl_vec_t< _value_t >::clear( ) {
        for ( int i{}; i < m_size; ++i )
            ( &at( i ) )->~_value_t( );

        m_size = 0;

        m_mem.clear( );
    }

    template < typename _value_t >
    ALWAYS_INLINE void utl_vec_t< _value_t >::reserve( const int size ) {
        if ( size <= m_mem.alloc_count( ) )
            return;

        m_mem.grow( size - m_mem.alloc_count( ) );
    }

    template < typename _value_t >
    ALWAYS_INLINE int utl_vec_t< _value_t >::size( ) const { return m_size; }

    template < typename _value_t >
    ALWAYS_INLINE utl_vec_t< _value_t >& utl_vec_t< _value_t >::operator =( const utl_vec_t< _value_t >& other ) {
        reserve( m_size = other.m_size );
    
        for ( int i{}; i < m_size; ++i )
            at( i ) = other.at( i );

        return *this;
    }
}