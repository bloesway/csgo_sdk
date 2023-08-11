#pragma once

namespace sdk::detail {
#pragma pack( push, 1 )
    union version_t {
        std::uint16_t   m_id{};

        struct {
            std::uint8_t m_major, m_minor;
        };
    };

    union ex_version_t {
        std::uint32_t m_id{};

        struct {
            std::uint16_t m_major, m_minor;
        };
    };
#pragma pack( pop )

#pragma pack( push, 4 )
    struct export_dir_t {
        std::uint32_t   m_characteristics{},
                        m_timedata_stamp{};
        ex_version_t    m_ver{};
        std::uint32_t   m_name{},
                        m_base{},
                        m_fns_count{},
                        m_names_count{},
                        m_fns_rva{},
                        m_names_rva{},
                        m_name_ords_rva{};
    };

    struct data_dir_t {
        std::uint32_t m_rva{}, m_size{};
    };

    struct raw_data_dir_t {
        std::uint32_t m_raw_data_ptr{}, m_size{};
    };

    union x86_data_dirs_t {
        struct {
            data_dir_t      m_export,
                            m_import,
                            m_resource,
                            m_exception;

            raw_data_dir_t  m_security;

            data_dir_t      m_base_reloc,
                            m_debug,
                            m_copyright,
                            m_global_ptr,
                            m_tls,
                            m_load_cfg,
                            m_bound_import,
                            m_iat,
                            m_delay_import,
                            m_com_desc,
                            m_reserved0;
        };

        data_dir_t m_entries[ -e_data_dir::max ]{};
    };

    union x64_data_dirs_t {
        struct {
            data_dir_t      m_export,
                            m_import,
                            m_resource,
                            m_exception;

            raw_data_dir_t  m_security;

            data_dir_t      m_base_reloc,
                            m_debug,
                            m_arch,
                            m_global_ptr,
                            m_tls,
                            m_load_cfg,
                            m_bound_import,
                            m_iat,
                            m_delay_import,
                            m_com_desc,
                            m_resverved0;
        };

        data_dir_t m_entries[ -e_data_dir::max ]{};
    };

    union file_characteristics_t {
        std::uint16_t m_flags{};

        struct {
            std::uint16_t   m_relocs_stripped : 1,
                            m_executable : 1,
                            m_lines_stripped : 1,
                            m_local_symbols_stripped : 1,
                            m_aggressive_ws_trim : 1,
                            m_large_address_aware : 1,
                            pad0 : 1,
                            m_bytes_reversed_lo : 1,
                            m_machine_32 : 1,
                            m_debug_stripped : 1,
                            m_runnable_from_swap : 1,
                            m_net_run_from_swap : 1,
                            m_system_file : 1,
                            m_dll_file : 1,
                            m_up_system_only : 1,
                            m_bytes_reversed_hi : 1;
        };
    };

    struct file_hdr_t {
        e_machine               m_machine{};
        std::uint16_t           m_sections_count{};
        std::uint32_t           m_timedate_stamp{},
                                m_symbols_ptr{}, m_symbols_count{};
        std::uint16_t           m_opt_hdr_size{};

        file_characteristics_t  m_characteristics{};
    };

    union dll_characteristics_t {
        std::uint16_t m_flags{};

        struct {
            std::uint16_t   pad0 : 5,
                            m_high_entropy_va : 1,
                            m_dynamic_base : 1,
                            m_force_integrity : 1,
                            m_nx_compat : 1,
                            m_no_isolation : 1,
                            m_no_seh : 1,
                            m_no_bind : 1,
                            m_app_container : 1,
                            m_wdm_driver : 1,
                            m_guard_cf : 1,
                            m_terminal_server_aware : 1;
        };
    };

    struct x64_opt_hdr_t {
        std::uint16_t           m_magic{};
        version_t               m_linker_ver{};

        std::uint32_t           m_code_size{},
                                m_init_data_size{},
                                m_uninit_data_size{},
                                m_ep{},
                                m_code_base{};

        std::uint64_t           m_image_base{};
        std::uint32_t           m_section_align{},
                                m_file_align{};

        ex_version_t            m_os_ver{},
                                m_img_ver{},
                                m_subsystem_ver{};
        std::uint32_t           m_win32_ver_value{},
                                m_image_size{},
                                m_hdrs_size{},
                                m_checksum{};

        e_subsystem             m_subsystem{};

        dll_characteristics_t   m_characteristics{};


        std::uint64_t           m_stack_reserve_size{},
                                m_stack_commit_size{},
                                m_heap_reserve_size{},
                                m_heap_commit_size{};

        std::uint32_t           m_ldr_flags{},
                                m_data_dirs_count{};

        x64_data_dirs_t         m_data_dirs{};
    };

    struct x86_opt_hdr_t {
        std::uint16_t           m_magic{};
        version_t               m_linker_ver{};

        std::uint32_t           m_code_size{},
                                m_init_data_size{},
                                m_uninit_data_size{},
                                m_ep{},
                                m_code_base{},
                                m_data_base{},
                                m_image_base{},
                                m_section_align{},
                                m_file_align{};

        ex_version_t            m_os_ver{},
                                m_img_ver{},
                                m_subsystem_ver{};
        std::uint32_t           m_win32_ver_value{},
                                m_image_size{},
                                m_hdrs_size{},
                                m_checksum{};

        e_subsystem             m_subsystem{};

        dll_characteristics_t   m_characteristics{};

        std::uint32_t           m_stack_reserve_size{},
                                m_stack_commit_size{},
                                m_heap_reserve_size{},
                                m_heap_commit_size{},
                                m_ldr_flags{},
                                m_data_dirs_count{};
        
        x86_data_dirs_t         m_data_dirs{};
    };

    template < bool _x64 >
    using opt_hdr_t = std::conditional_t< _x64, x64_opt_hdr_t, x86_opt_hdr_t >;

    template < bool _x64 >
    struct nt_hdrs_t {
        std::uint32_t       m_sig{};

        file_hdr_t          m_file_hdr{};
        opt_hdr_t< _x64 >   m_opt_hdr{};
    };

    struct dos_hdr_t {
        std::uint16_t   e_magic{}, e_cblp{},
                        e_cp{}, e_crlc{}, e_cparhdr{},
                        e_minalloc{}, e_maxalloc{},
                        e_ss{}, e_sp{}, e_csum{},
                        e_ip{}, e_cs{}, e_lfarlc{},
                        e_ovno{}, e_res[ 4u ]{}, e_oemid{},
                        e_oeminfo{}, e_res2[ 10u ]{};

        std::uint32_t   e_lfanew{};
    };

    template < bool _x64 >
    struct pe_image_t {
        using addr_t = base_address_t< std::conditional_t< _x64, std::uint64_t, std::uint32_t > >;

        dos_hdr_t m_dos_hdr{};

        ALWAYS_INLINE nt_hdrs_t< _x64 >* nt_hdrs( ) {
            return reinterpret_cast< nt_hdrs_t< _x64 >* >(
                reinterpret_cast< std::uint8_t* >( this ) + m_dos_hdr.e_lfanew
            );
        }

        ALWAYS_INLINE const nt_hdrs_t< _x64 >* nt_hdrs( ) const {
            return reinterpret_cast< const nt_hdrs_t< _x64 >* >(
                reinterpret_cast< const std::uint8_t* >( this ) + m_dos_hdr.e_lfanew
            );
        }

        ALWAYS_INLINE bool for_each_export( const addr_t base_addr,
            const std::function< bool( const char*, const addr_t ) >& fn
        ) {
            const auto& opt_hdr = nt_hdrs( )->m_opt_hdr;
            if ( opt_hdr.m_data_dirs_count <= -e_data_dir::entry_export )
                return false;

            const auto& data_dir = opt_hdr.m_data_dirs.m_export;
            if ( !data_dir.m_size )
                return false;

            const auto export_dir = base_addr.offset( data_dir.m_rva ).template as< export_dir_t* >( );

            const auto names = base_addr.offset( export_dir->m_names_rva ).template as< std::uint32_t* >( );
            const auto fns = base_addr.offset( export_dir->m_fns_rva ).template as< std::uint32_t* >( );
            const auto name_ords = base_addr.offset( export_dir->m_name_ords_rva ).template as< std::uint16_t* >( );

            for ( std::uint32_t i{}; i < export_dir->m_names_count; ++i )
                if ( fn( base_addr.offset( names[ i ] ).template as< const char* >( ), base_addr.offset( fns[ name_ords[ i ] ] ) ) )
                    break;

            return true;
        }
    };

#pragma pack( pop )
}