#pragma once

namespace hacks {
    class c_move {
    private:

    public:
        void handle( valve::user_cmd_t& cmd ) const;

        void rotate( valve::user_cmd_t& cmd, const sdk::qang_t& wish_angles ) const;
    };

    inline const auto g_move = std::make_unique< c_move >( );
}