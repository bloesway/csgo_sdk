#pragma once

namespace sdk::detail {
    template < typename _value_t >
    ALWAYS_INLINE _value_t load_from_reg( _value_t value ) {
#if defined( __clang__ ) || defined( __GNUC__ )
        asm(
            ""
            : "=r"( value )
            : "0"( value )
            :
        );

        return value;
#else
        //volatile _value_t reg = value;

        //return reg;
        return value;
#endif
    }

    template < std::size_t _size >
        requires ( _size >= 1u )
    struct byte_seq_t {
    private:
        static constexpr auto k_wildcard = '?';
        static constexpr auto k_delimiter = ' ';
        static constexpr auto k_not = '!';
    public:
        struct byte_t {
            enum struct e_type : std::uint8_t {
                invalid,
                equal,
                not_equal,
                wildcard
            };

            ALWAYS_INLINE bool valid( const std::uint8_t byte ) const {
                if ( m_type == e_type::invalid )
                    return false;

                return m_type == e_type::wildcard
                    || ( m_value == byte ) ^ ( m_type == e_type::not_equal );
            }

            e_type          m_type{};
            std::uint8_t    m_value{};
        };

        using bytes_t = std::array< byte_t, _size >;

        template < typename _value_t >
            requires ( !std::_Is_any_of_v< std::decay_t< _value_t >, std::string_view, const char* > )
        static consteval std::size_t measure( const _value_t ) {
            return _size;
        }

        static consteval std::size_t measure( const std::string_view str ) {
            std::size_t size{};

            for ( std::size_t i{}; i < str.size( ); i += 2u ) {
                const auto& chr = str.at( i );
                if ( chr == k_wildcard )
                    ++size;
                else if ( chr == k_delimiter )
                    --i;
                else {
                    if ( chr == k_not )
                        ++i;

                    ++size;
                }
            }

            return size;
        }

        static constexpr bytes_t parse( const std::string_view str ) {
            constexpr auto hex2int = [ ] ( const std::size_t chr ) {
                if ( chr >= '0'
                    && chr <= '9' )
                    return chr - '0';

                if ( chr >= 'A'
                    && chr <= 'F' )
                    return chr - 'A' + 10u;

                return chr - 'a' + 10u;
            };

            bytes_t bytes{};

            for ( std::size_t i{}, j{}; i < str.size( ); i += 2u ) {
                const auto& chr = str.at( i );
                if ( chr == k_wildcard )
                    bytes.at( j++ ).m_type = byte_t::e_type::wildcard;
                else if ( chr == k_delimiter )
                    --i;
                else {
                    auto& byte = bytes.at( j++ );

                    if ( chr != k_not )
                        byte.m_type = byte_t::e_type::equal;
                    else {
                        byte.m_type = byte_t::e_type::not_equal;

                        ++i;
                    }

                    byte.m_value = static_cast< std::uint8_t >(
                        hex2int( str.at( i ) ) * 0x10u + hex2int( str.at( i + 1u ) )
                    );
                }
            }

            return bytes;
        }

        ALWAYS_INLINE constexpr byte_seq_t( ) = default;

        ALWAYS_INLINE consteval byte_seq_t( const bytes_t& bytes ) : m_bytes{ bytes } {}

        template < typename _lambda_t, std::size_t... _indices >
            requires std::is_invocable_v< _lambda_t >
        ALWAYS_INLINE byte_seq_t( const _lambda_t lambda, std::index_sequence< _indices... > ) {
            if constexpr ( std::is_same_v< const char*, std::decay_t< std::invoke_result_t< _lambda_t > > > ) {
                constexpr auto seq = parse( lambda( ) );

                m_bytes = { /*load_from_reg*/( seq[ _indices ] )... };

                return;
            }

            m_bytes = {
                byte_t{
                    byte_t::e_type::equal,
                    reinterpret_cast< const std::uint8_t* >( lambda( ) )[ _indices ]
                }...
            };
        }

        ALWAYS_INLINE address_t search( const address_t start, const address_t end ) const {
            const auto seq_end = m_bytes.end( );

            for ( auto i = start.as< std::uint8_t* >( ); ; ++i ) {
                auto j = i;
                for ( auto k = m_bytes.begin( ); ; ++j, k = std::next( k ) ) {
                    if ( k == seq_end )
                        return i;

                    if ( j == end.as< std::uint8_t* >( ) )
                        return end;

                    if ( !k->valid( *j ) )
                        break;
                }
            }

            return end;
        }

        bytes_t m_bytes{};
    };
}