#pragma once

#include "sdk.hpp"

#include <d3d9.h>

#include <minhook.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#define OFFSET( type, definition, offset ) \
    ALWAYS_INLINE auto& definition { \
        return *reinterpret_cast< std::add_pointer_t< type > >( \
            reinterpret_cast< std::uintptr_t >( this ) + offset \
        ); \
    } \

#define POFFSET( type, definition, offset ) \
    ALWAYS_INLINE auto definition { \
        return reinterpret_cast< std::add_pointer_t< type > >( \
            reinterpret_cast< std::uintptr_t >( this ) + offset \
        ); \
    } \

#define PPOFFSET( type, definition, offset ) \
    ALWAYS_INLINE auto& definition { \
        return **reinterpret_cast< std::add_pointer_t< std::add_pointer_t< type > > >( \
            reinterpret_cast< std::uintptr_t >( this ) + offset \
        ); \
    } \

#define VFUNC( type, definition, index, ... ) \
    ALWAYS_INLINE auto definition { \
        return reinterpret_cast< type >( \
            ( *reinterpret_cast< std::uintptr_t** >( this ) )[ index ] \
        )( this, __VA_ARGS__ ); \
    } \

#define OFFSET_VFUNC( type, definition, offset, ... ) \
    ALWAYS_INLINE auto definition { \
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