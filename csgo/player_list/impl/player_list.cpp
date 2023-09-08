#include "../../csgo.hpp"

void c_players::on_entity_add( valve::base_entity_t* entity ) {
    if ( !entity 
        || entity == g_local_player->self( ) )
        return;

    const auto index = entity->networkable( )->index( );
    if ( index < 1
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

void c_players::update_in_game( ) {
    const auto filled = m_entries.size( ) > 0u;
    if ( m_updated || filled )
        return;

    if ( !g_local_player->self( ) ||
        !g_local_player->self( )->alive( ) )
        return;

    for ( auto i = 0; i < valve::g_global_vars->m_max_clients; i++ ) {
        const auto player = reinterpret_cast< valve::cs_player_t* >( valve::g_entity_list->get_entity( i ) );
        if ( !player || !player->alive( )
            || player->friendly( g_local_player->self( ) ) )
            continue;

        if ( m_entries.empty( ) ) {
            m_entries.emplace_back( entry_t{ player, i } );

            continue;
        }

        for ( auto it = m_entries.begin( ); it != m_entries.end( ); it = std::next( it ) ) {
            if ( it->m_player == player )
                continue;
        }

        m_entries.emplace_back( entry_t{ player, i } );
    }

    if ( filled )
        m_updated = true;
}