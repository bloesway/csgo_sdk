#include "../../csgo.hpp"

void c_players::on_entity_add( valve::base_entity_t* entity ) {
    if ( !entity || entity == g_local_player->self( ) )
        return;

    for ( auto it = m_entries.begin( ); it != m_entries.end( ); it = std::next( it ) ) {
        if ( it->m_player == entity )
            return;
    }

    const auto index = entity->networkable( )->index( );
    if ( index < 1u || index > 64u )
        return;

    m_entries.push_back( entity );
}

void c_players::on_entity_remove( valve::base_entity_t* entity ) {
    if ( !entity || entity == g_local_player->self( ) )
        return;

    for ( auto it = m_entries.begin( ); it != m_entries.end( ); it = std::next( it ) ) {
        if ( it->m_player != entity )
            continue;

        m_entries.erase( it );

        return;
    }
}