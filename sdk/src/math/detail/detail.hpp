#pragma once

namespace sdk::detail {
    template < typename _value_t >
        requires std::is_arithmetic_v< _value_t >
    using enough_float_t = std::conditional_t< sizeof( _value_t ) <= sizeof( float ), float, double >;
}

#include "data_types/array_wrapper.hpp"
#include "data_types/base_qang.hpp"
#include "data_types/base_vec.hpp"
#include "data_types/base_mat.hpp"
#include "data_types/base_rect.hpp"
#include "data_types/base_clr.hpp"