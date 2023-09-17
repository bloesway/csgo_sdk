#include "../../csgo.hpp"

void c_players::on_entity_add( valve::base_entity_t* entity ) {
    const auto index = entity->networkable( )->index( );
    if ( index < 1 || index > valve::g_global_vars->m_max_clients
        || index == valve::g_engine->local_index( ) )
        return;

    for ( auto it = m_hash_map.begin( ); it != m_hash_map.end( ); it = std::next( it ) ) {
        if ( it->second.m_player == entity )
            return;
    }

    m_hash_map.emplace( index, entry_t{ entity, index } );
}

void c_players::on_entity_remove( valve::base_entity_t* entity ) {
    for ( auto it = m_hash_map.begin( ); it != m_hash_map.end( ); it = std::next( it ) ) {
        if ( it->second.m_player != entity )
            continue;

        m_hash_map.erase( it );

        return;
    }
}

std::vector< c_players::target_t > c_players::find_targets( ) {
    auto ret = std::vector< c_players::target_t >{};

    for ( const auto& it : m_hash_map ) {
        const auto entry = it.second;

        const auto player = entry.m_player;
        if ( !player || !player->alive( )
            || player->friendly( g_local_player->self( ) ) )
            continue;

        const auto records = entry.m_records;
        if ( records.empty( ) )
            continue;

        /* your logic */

        ret.emplace_back( target_t{ player /* your args( as example record or smth ) */ } );
    }

    return ret;
}