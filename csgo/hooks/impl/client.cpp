#include "../../csgo.hpp"

namespace hooks {
    NAKED void __stdcall create_move_proxy( int seq_number, float input_sample_frame_time, bool active ) {
        __asm {
            push ebp
            mov ebp, esp

            push ebx
            push esp
            push dword ptr[ ebp + 16 ]
            push dword ptr[ ebp + 12 ]
            push dword ptr[ ebp + 8 ]

            call create_move

            pop ebx

            pop ebp
            retn 12
        }
    }

    void __stdcall create_move( int seq_number, float input_sample_frame_time, bool active, bool& send_packet ) {
        o_create_move( valve::g_client, seq_number, input_sample_frame_time, active );

        if ( !valve::g_engine->in_game( )
            || !g_local_player->self( )
            || !g_local_player->self( )->alive( ) )
            return;

        const auto slot = seq_number % valve::k_mp_backup;

        g_local_player->create_move( send_packet,
            valve::g_input->m_cmds[ slot ], valve::g_input->m_vfyd_cmds[ slot ]
        );
    }
}