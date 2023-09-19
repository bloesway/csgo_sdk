#pragma once

class c_local_player {
private:
    void update_prediction( ) const;

    void simulate_prediction( valve::user_cmd_t& cmd );

    void finish_prediction( );

    valve::weapon_cs_base_gun_t*    m_weapon{};
    valve::weapon_info_t*           m_weapon_info{};

    struct prediction_t {
        ALWAYS_INLINE void store( );

        ALWAYS_INLINE void restore( );

        int                  m_tick_count{};

        bool                 m_in_prediction{}, m_first_time_predicted{};

        float                m_cur_time{}, m_frame_time{};

        int*                 m_random_seed{};
        valve::cs_player_t*  m_pred_player{};

        valve::move_data_t*  m_move_data{};
    } m_prediction{};

    bool        m_packet{};
    sdk::qang_t m_sent_angles{};
public:
    void frame_stage( valve::e_frame_stage stage, bool post );

    void create_move( bool& send_packet,
        valve::user_cmd_t& cmd, valve::vfyd_user_cmd_t& vfyd_cmd
    );

    ALWAYS_INLINE valve::cs_player_t* self( ) const;

    ALWAYS_INLINE valve::weapon_cs_base_gun_t* weapon( ) const;

    ALWAYS_INLINE valve::weapon_info_t* weapon_info( ) const;

    ALWAYS_INLINE auto& packet( );

    ALWAYS_INLINE auto& sent_angles( );
};

inline const auto g_local_player = std::make_unique< c_local_player >( );

#include "impl/local_player.inl"