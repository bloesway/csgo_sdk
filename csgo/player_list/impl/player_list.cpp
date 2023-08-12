#include "../../csgo.hpp"

void c_players::on_entity_add( valve::base_entity_t* entity ) {
    if ( !entity 
        || entity == g_local_player->self( ) )
        return;

    const auto index = entity->networkable( )->index( );
    if ( 1 < index 
        || index > valve::g_global_vars->m_max_clients )
        return;

    for ( auto it = m_entries.begin( ); it != m_entries.end( ); it = std::next( it ) ) {
        if ( it->m_player == entity )
            return;
    }

    m_entries.push_back( entry_t{ entity, index } );
}

void c_players::on_entity_remove( valve::base_entity_t* entity ) {
    if ( !entity
        || entity == g_local_player->self( ) )
        return;

    for ( auto it = m_entries.begin( ); it != m_entries.end( ); it = std::next( it ) ) {
        if ( it->m_player != entity )
            continue;

        m_entries.erase( it );

        return;
    }
}