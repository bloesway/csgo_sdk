#include "../../../sdk.hpp"

namespace sdk {
    void c_cfg::save( const std::string_view name ) const {
        const auto path = std::filesystem::path{ SDK_CFG_ID_OBJECT } /= name;

        std::filesystem::create_directory( SDK_CFG_ID_OBJECT );

        nlohmann::json json{};

        auto& object = json[ SDK_CFG_ID_OBJECT ];

        for ( const auto var : m_vars )
            var->save( object );

        auto str = json.dump( );

        for ( auto& chr : str )
            chr ^= k_byte_xor;

        if ( std::ofstream file{ path, std::ios::out | std::ios::trunc } )
            file << str;
    }

    void c_cfg::load( const std::string_view name ) {
        const auto path = std::filesystem::path{ SDK_CFG_ID_OBJECT } /= name;

        std::filesystem::create_directory( SDK_CFG_ID_OBJECT );

        std::string str{};
        if ( std::ifstream file{ path, std::ios::in } )
            file >> str;
        else
            return;

        if ( str.empty( ) )
            return;

        for ( auto& chr : str )
            chr ^= k_byte_xor;

        const auto json = nlohmann::json::parse( str );
        if ( !json.is_object( ) )
            return;

        /* verify the cfg file via trying to find our identity object */
        const auto object = json.find( SDK_CFG_ID_OBJECT );
        if ( object == json.end( ) )
            return;

        const auto& value = object.value( );

        for ( const auto var : m_vars )
            var->load( value );
    }
}