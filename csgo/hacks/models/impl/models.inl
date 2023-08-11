#pragma once

#include "../models.hpp"

namespace hacks {
    ALWAYS_INLINE valve::material_t* c_models::create_material( const char* name, const char* shader, const char* settings ) const {
        const auto key_values = reinterpret_cast< valve::key_values_t* >(
            valve::g_mem_alloc->alloc( sizeof( valve::key_values_t ) )
        );

        key_values->init( shader );
        key_values->load_from_buffer( name, settings );

        return valve::g_material_system->create_material( name, key_values );
    }

    ALWAYS_INLINE void c_models::create_materials( ) {
        m_flat = create_material(
            "wok_flat.vmt", "UnlitGeneric",
            R"#("UnlitGeneric" {
                    "$basetexture"      "vgui/white_additive"
	                "$ignorez"          "0"
	                "$model"		    "1"
	                "$flat"			    "1"
	                "$nocull"		    "1"
	                "$selfillum"	    "1"
	                "$halflambert"      "1"
	                "$nofog"		    "1"
	                "$wireframe"	    "0"
                    "$znearer"          "0"
            })#"
        );

        m_flat_ignorez = create_material(
             "wok_flat_ignorez.vmt", "UnlitGeneric",
             R"#("UnlitGeneric" {
                    "$basetexture"      "vgui/white_additive"
	                "$ignorez"          "1"
	                "$model"		    "1"
	                "$flat"			    "1"
	                "$nocull"		    "1"
	                "$selfillum"	    "1"
	                "$halflambert"      "1"
	                "$nofog"		    "1"
	                "$wireframe"	    "0"
                    "$znearer"          "0"
            })#"
        );
    }

    ALWAYS_INLINE void c_models::set_clr( const ui_clr_t& clr ) const {
        valve::g_studio_render->set_alpha_mod( clr.Value.w );
        valve::g_studio_render->set_clr_mod( const_cast< float* >( &clr.Value.x ) );
    }

    ALWAYS_INLINE c_models::e_model_type c_models::get_model_type( const valve::draw_model_info_t& info ) const {
        if ( !info.m_studio
            || !info.m_renderable )
            return e_model_type::invalid;

        const auto name = info.m_studio->m_name;
        if ( !name )
            return e_model_type::invalid;

        if ( *reinterpret_cast< std::uint32_t* >( name + 14 ) == 'yalp' )
            return e_model_type::player;

        return e_model_type::invalid;
    }
}