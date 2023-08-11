#include "../../csgo.hpp"

namespace hooks {
    void __fastcall draw_model( valve::studio_render_t* ecx, std::uintptr_t edx, std::uintptr_t results, const valve::draw_model_info_t& info,
        sdk::mat3x4_t* bones, float* flex_weights, float* flex_delayed_weights, const sdk::vec3_t& origin, int flags
    ) {
        if ( ecx->is_forced_material_override( ) )
            return o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );

        if ( hacks::g_models->on_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags ) )
            return;

        o_draw_model( ecx, edx, results, info, bones, flex_weights, flex_delayed_weights, origin, flags );
    }
}