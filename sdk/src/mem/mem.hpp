#pragma once

namespace sdk {
    inline constexpr auto k_dos_hdr_magic = 0x5a4du;
    inline constexpr auto k_nt_hdrs_magic = 0x4550u;

    enum struct e_machine : std::uint16_t {
        unknown,
        target_host = 0x1u,
        i386 = 0x14cu,
        r3000 = 0x162u,
        r4000 = 0x166u,
        r10000 = 0x168u,
        wcemipsv2 = 0x169u,
        alpha = 0x184u,
        sh3 = 0x1a2u,
        sh3dsp = 0x1a3u,
        sh3e = 0x1a4u,
        sh4 = 0x1a6u,
        sh5 = 0x1a8u,
        arm = 0x1c0u,
        thumb = 0x1c2u,
        armnt = 0x1c4u,
        am33 = 0x1d3u,
        powerpc = 0x1f0u,
        powerpcfp = 0x1f1u,
        ia64 = 0x200u,
        mips16 = 0x266u,
        alpha64 = 0x284u,
        mipsfpu = 0x366u,
        mipsfpu16 = 0x466u,
        axp64 = 0x284u,
        tricore = 0x520u,
        cef = 0xcefu,
        ebc = 0xebcu,
        amd64 = 0x8664u,
        m32r = 0x9041u,
        arm64 = 0xaa64u,
        cee = 0xc0eeu
    };

    enum struct e_subsystem : std::uint16_t {
        unknown,
        native,
        win_gui,
        win_cui,
        os2_cui = 5u,
        posix_cui = 7u,
        native_win,
        win_ce_gui,
        efi_app,
        efi_boot_service_driver,
        efi_runtime_driver,
        efi_rom,
        xbox,
        win_boot_application = 16u,
        xbox_code_catalog
    };

    enum struct e_data_dir : std::uint8_t {
        entry_export,
        entry_import,
        entry_resource,
        entry_exception,
        entry_security,
        entry_base_reloc,
        entry_debug,
        entry_copyright,
        entry_arch = 7u,
        entry_global_ptr,
        entry_tls,
        entry_load_cfg,
        entry_bound_import,
        entry_iat,
        entry_delay_import,
        entry_com_descriptor,
        res0,
        max
    };
    ENUM_UNDERLYING_OPERATOR( e_data_dir );
}

#include "detail/detail.hpp"

namespace sdk {
    using address_t = detail::base_address_t< std::uintptr_t >;

    using stack_frame_t = detail::base_stack_frame_t< std::uintptr_t >;

    using x64_pe_image_t = detail::pe_image_t< true >;

    using x86_pe_image_t = detail::pe_image_t< false >;

#if defined( _WIN32 ) || defined( _WIN64 )
    enum struct e_ldr_data_table : std::uint8_t {
        in_load_order,
        in_mem_order,
        in_init_order,
        max
    };
    ENUM_UNDERLYING_OPERATOR( e_ldr_data_table );

    struct rtl_balanced_node_t {
        rtl_balanced_node_t*    m_child[ 2u ]{},
                                *m_left{}, *m_right{};

        address_t               m_parent_value{};
    };

    template < bool _wide >
    struct rtl_str_t {
        using char_t = std::conditional_t< _wide, wchar_t, char >;

        std::uint16_t    m_len{},
                        m_max_len{};

        char_t*         m_buffer{};
    };

    struct rtl_user_process_params_t {
        std::uint32_t   m_max_len{},
                        m_len{},
                        m_flags{},
                        m_debug_flags{};

        address_t       m_con_handle{};
        std::uint32_t   m_con_flags{};

        address_t       m_standard_input{},
                        m_standard_output{},
                        m_standard_error{};

        struct {
            rtl_str_t< false >  m_dos_path{};
            address_t           m_handle{};
        }                   m_cur_dir{};

        rtl_str_t< true >   m_dll_path{},
                            m_image_path_name{},
                            m_cmd_line{};

        address_t           m_env{};
        std::uint32_t       m_starting_x{},
                            m_starting_y{},
                            m_count_x{},
                            m_count_y{},
                            m_count_chars_x{},
                            m_count_chars_y{},
                            m_file_attribute{},
                            m_wnd_flags{},
                            m_show_wnd_flags{};

        rtl_str_t< true >   m_wnd_title{},
                            m_desktop_info{},
                            m_shell_info{},
                            m_runtime_data{};

        struct {
            std::uint16_t       m_flags{}, m_len{};
            std::uint32_t       m_timestamp{};

            rtl_str_t< false >  m_dos_path{};
        }                   m_cur_dirs[ 32u ]{};

        std::uintptr_t      m_env_size{},
                            m_env_ver{};
        address_t           m_pckg_dependency_data{};
        std::uint32_t       m_proc_group_id{},
                            m_ldr_threads{};
    };

    struct ldr_data_t {
        std::uint32_t   m_len{};
        std::uint8_t    m_initialized{};
        address_t       m_ss_handle{};

        union {
            struct {
                LIST_ENTRY  m_in_load_order,
                            m_in_mem_order,
                            m_in_init_order;
            };

            LIST_ENTRY m_entries[ -e_ldr_data_table::max ]{};
        }               m_module_lists{};

        std::uintptr_t  m_entry_in_progress{};
        std::uint8_t    m_shutdown_in_progress{};
        std::uintptr_t  m_shutdown_thread_id{};
    };

    struct ldr_data_table_entry_t {
        union {
            struct {
                LIST_ENTRY  m_in_load_order,
                            m_in_mem_order,
                            m_in_init_order;
            };

            LIST_ENTRY m_entries[ -e_ldr_data_table::max ]{};
        }                   m_links{};

        address_t           m_dll_base{},
                            m_ep{};
        std::uint32_t       m_image_size{};
        rtl_str_t< true >   m_full_dll_name{},
                            m_base_dll_name{};
        std::uint8_t        m_flag_group[ 4u ]{};
        std::uint32_t       m_flags{};
        std::uint16_t       m_obsolete_load_count{},
                            m_tls_index{};
        LIST_ENTRY          m_hash_links{};
        std::uint32_t       m_timedata_stamp{};
        address_t           m_ep_activation_context{},
                            m_lock{},
                            m_ddag_node{};
        LIST_ENTRY          m_node_module_link{};
        address_t           m_load_context{},
                            m_parent_dll_base{},
                            m_switch_back_context{};
        rtl_balanced_node_t m_base_addr_index_node{},
                            m_mapping_info_index_node{};
        address_t           m_original_base{};
        LARGE_INTEGER       m_load_time{};
        std::uint32_t       m_base_name_hash_value{},
                            m_load_reason{},
                            m_implicit_path_options{},
                            m_ref_count{};
    };

    struct peb_t {
        ALWAYS_INLINE void for_each_ldr_data_table_entry(
            const std::function< bool( ldr_data_table_entry_t* const ) >& fn, const e_ldr_data_table type
        );

        std::uint8_t                m_inherited_addr_space{},
                                    m_read_image_file_exec_options{},
                                    m_being_debugged{},
                                    m_bit_field{};

        address_t                   m_mutant{}, m_image_base_addr{};
        ldr_data_t*                 m_ldr{};
        rtl_user_process_params_t*  m_process_params{};

        address_t                   m_subsystem_data{},
                                    m_process_heap{};

        RTL_CRITICAL_SECTION*       m_fast_peb_lock{};
        address_t                   m_atl_thunks_list{},
                                    m_ifeo_key{};

        std::uint32_t               m_cross_process_flags{};

        union {
            address_t   m_kernel_callback_table{},
                        m_user_shared_info;
        };

        std::uint32_t               m_reserved0{},
                                    m_atl_thunks_list32{};
        address_t                   m_api_set_map{};
        std::uint32_t               m_tls_expansion_cnt{};
        address_t                   m_tls_bitmap{};
        std::uint32_t               m_tls_bitmap_bits[ 2u ]{};

        address_t                   m_read_only_shared_mem_base{},
                                    m_hotpatch_info{},
                                    *m_read_only_static_srv_data{},
                                    m_ansi_code_page_data{},
                                    m_oem_code_page_data{},
                                    m_unicode_case_table_data{};

        std::uint32_t               m_processors_count{},
                                    m_nt_global_flag{};

        LARGE_INTEGER               m_critical_section_timeout{};
        address_t                   m_heap_segment_reserve{},
                                    m_heap_segment_commit{},
                                    m_heap_decommit_total_free_threshold{},
                                    m_heap_decommit_free_block_threshold{};

        std::uint32_t               m_heaps_count{},
                                    m_max_heaps_count{};

        address_t                   m_process_heaps{},
                                    m_gdi_shared_handle_table{},
                                    m_process_starter_helper{};

        std::uint32_t               m_gdi_attribute_list{};
        RTL_CRITICAL_SECTION*       m_ldr_lock{};

        std::uint32_t               m_os_major_ver{},
                                    m_os_minor_ver{};

        std::uint16_t               m_os_build_number{},
                                    m_oscsd_ver{};

        std::uint32_t               m_os_patform_id{},
                                    m_image_subsystem{},
                                    m_image_subsystem_major_ver{},
                                    m_image_subsystem_minor_ver{};

        std::uintptr_t              m_active_process_affinity_mask{};
#ifdef _WIN32
        std::uint32_t               m_gdi_handle_buffer[ 34u ]{};
#else
        std::uint32_t               m_gdi_handle_buffer[ 60u ]{};
#endif

        address_t                   m_post_process_init_routine{},
                                    m_tls_expansion_bitmap{};
        std::uint32_t               m_tls_expansion_bitmap_bits[ 32u ]{},
                                    m_session_id{};

        ULARGE_INTEGER              m_app_compat_flags{},
                                    m_app_compat_flags_user{};

        address_t                   m_shim_data{},
                                    m_app_compat_info{};

        rtl_str_t< true >           m_csd_ver{};
        address_t                   m_activation_context_data{},
                                    m_process_asm_storage_map{},
                                    m_sys_default_activation_context_data_map{},
                                    m_sys_asm_storage_map{},
                                    m_min_stack_commit{},
                                    m_fls_callback{};

        LIST_ENTRY                  m_fls_list_head{};
        address_t                   m_fls_bitmap{};
        std::uint32_t               m_fls_bitmap_bits[ 4u ]{},
                                    m_fls_high_index{};

        address_t                   m_wer_registration_data{},
                                    m_wer_ship_assert{},
                                    m_context_data{},
                                    m_image_hdr_hash{};

        std::uint32_t               m_tracing_flags{};
        std::uint64_t               m_csr_srv_read_only_shared_mem_base{};

        address_t                   m_tpp_worker_list_lock{};
        LIST_ENTRY                  m_tpp_worker_list{};

        address_t                   m_wait_on_addr_hash_table[ 128u ]{};
    };

    ALWAYS_INLINE peb_t* peb( );
#endif
}

#include "impl/mem.inl"