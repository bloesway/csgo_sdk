#pragma once

#include "sdk.hpp"

#include <d3d9.h>

#include <minhook.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#define OFFSET( type, name, offset ) \
    ALWAYS_INLINE type& name { \
        return *sdk::address_t{ this }.self_offset( offset ).as< type* >( ); \
    } \

#define POFFSET( type, name, offset ) \
    ALWAYS_INLINE type* name { \
        return sdk::address_t{ this }.self_offset( offset ).as< type* >( ); \
    } \

#define VFUNC( type, name, index, ... ) \
    ALWAYS_INLINE auto name { \
        using fn_t = type; \
        \
        return ( *sdk::address_t{ this }.as< fn_t** >( ) )[ index ]( this, __VA_ARGS__ ); \
    } \

#define OFFSET_VFUNC( type, name, offset, ... ) \
    ALWAYS_INLINE auto name { \
        return offset.as< type >( )( this, __VA_ARGS__ ); \
    } \

#ifdef CSGO2018
#define VARVAL( old, latest ) old
#else
#define VARVAL( old, latest ) latest
#endif

#include "ctx/ctx.hpp"
#include "menu/menu.hpp"

#include "valve/valve.hpp"
#include "local_player/local_player.hpp"

#include "hacks/hacks.hpp"
#include "hooks/hooks.hpp"