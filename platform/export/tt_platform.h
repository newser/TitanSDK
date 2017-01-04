/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file tt_platform.h
@brief include all ts platform header files

developer only need include this file to use ts platform APIs
*/

#ifndef __TT_PLATFORM__
#define __TT_PLATFORM__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <algorithm/ptr/tt_ptr_heap.h>
#include <algorithm/ptr/tt_ptr_vector.h>
#include <algorithm/tt_algorithm_def.h>
#include <algorithm/tt_binary_search.h>
#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_common.h>
#include <algorithm/tt_buffer_format.h>
#include <algorithm/tt_compare.h>
#include <algorithm/tt_double_linked_list.h>
#include <algorithm/tt_hash.h>
#include <algorithm/tt_hashmap.h>
#include <algorithm/tt_list.h>
#include <algorithm/tt_pointer_list.h>
#include <algorithm/tt_red_black_tree.h>
#include <algorithm/tt_rng.h>
#include <algorithm/tt_rng_xorshift.h>
#include <algorithm/tt_single_linked_list.h>
#include <algorithm/tt_sort.h>
#include <algorithm/tt_stack.h>
#include <algorithm/tt_string.h>
#include <algorithm/tt_string_common.h>
#include <algorithm/tt_vector.h>
#include <cli/tt_cli.h>
#include <cli/tt_cli_def.h>
#include <cli/tt_cli_line.h>
#include <config/tt_algorithm_config.h>
#include <config/tt_customization_config.h>
#include <config/tt_environment_config.h>
#include <config/tt_file_system_config.h>
#include <config/tt_log_config.h>
#include <config/tt_memory_config.h>
#include <config/tt_mpn_config.h>
#include <config/tt_os_config.h>
#include <config/tt_platform_config.h>
#include <config/tt_platform_optimized_config.h>
#include <crypto/tt_aes.h>
#include <crypto/tt_aes_def.h>
#include <crypto/tt_crypto.h>
#include <crypto/tt_crypto_def.h>
#include <crypto/tt_dh.h>
#include <crypto/tt_dh_def.h>
#include <crypto/tt_dh_mpn.h>
#include <crypto/tt_hmac.h>
#include <crypto/tt_hmac_def.h>
#include <crypto/tt_md5.h>
#include <crypto/tt_pkcs5.h>
#include <crypto/tt_pkcs8.h>
#include <crypto/tt_rsa.h>
#include <crypto/tt_rsa_def.h>
#include <crypto/tt_sha.h>
#include <crypto/tt_sha_def.h>
#include <event/tt_adns_event.h>
#include <event/tt_event_base.h>
#include <event/tt_event_center.h>
#include <event/tt_event_poller.h>
#include <event/tt_evp_event.h>
#include <event/tt_process_event.h>
#include <event/tt_site_event.h>
#include <event/tt_thread_event.h>
#include <event/tt_timer_event.h>
#include <init/config_shell/tt_config_command.h>
#include <init/config_shell/tt_config_shell.h>
#include <init/config_shell/tt_console_config_shell.h>
#include <init/tt_component.h>
#include <init/tt_config_bool.h>
#include <init/tt_config_group.h>
#include <init/tt_config_node.h>
#include <init/tt_config_path.h>
#include <init/tt_config_s32.h>
#include <init/tt_config_string.h>
#include <init/tt_config_u32.h>
#include <init/tt_init_config.h>
#include <init/tt_platform_info.h>
#include <init/tt_platform_init.h>
#include <init/tt_profile.h>
#include <init/tt_version_info.h>
#include <io/tt_console.h>
#include <io/tt_console_event.h>
#include <io/tt_file_system.h>
#include <io/tt_file_system_aio.h>
#include <io/tt_file_system_aio_cb.h>
#include <io/tt_ipc_aio.h>
#include <io/tt_ipc_aio_cb.h>
#include <io/tt_network_io_def.h>
#include <io/tt_network_io_init.h>
#include <io/tt_socket.h>
#include <io/tt_socket_addr.h>
#include <io/tt_socket_aio.h>
#include <io/tt_socket_aio_cb.h>
#include <io/tt_socket_option.h>
#include <log/io/tt_log_io.h>
#include <log/io/tt_log_io_standard.h>
#include <log/layout/tt_log_field.h>
#include <log/layout/tt_log_layout.h>
#include <log/layout/tt_log_layout_pattern.h>
#include <log/tt_log.h>
#include <log/tt_log_context.h>
#include <log/tt_log_init.h>
#include <log/tt_log_manager.h>
#include <math/mp/tt_mpn.h>
#include <math/mp/tt_mpn_add.h>
#include <math/mp/tt_mpn_cache.h>
#include <math/mp/tt_mpn_common.h>
#include <math/mp/tt_mpn_div.h>
#include <math/mp/tt_mpn_exp.h>
#include <math/mp/tt_mpn_gcd.h>
#include <math/mp/tt_mpn_init.h>
#include <math/mp/tt_mpn_mod.h>
#include <math/mp/tt_mpn_mul.h>
#include <math/mp/tt_mpn_sub.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_memory_alloc.h>
#include <memory/tt_memory_pool.h>
#include <memory/tt_memory_spring.h>
#include <memory/tt_page.h>
#include <memory/tt_slab.h>
#include <misc/tt_asn1_def.h>
#include <misc/tt_assert.h>
#include <misc/tt_base64.h>
#include <misc/tt_charset_convert.h>
#include <misc/tt_charset_def.h>
#include <misc/tt_der_decode.h>
#include <misc/tt_der_encode.h>
#include <misc/tt_distinguished_name.h>
#include <misc/tt_error.h>
#include <misc/tt_error_def.h>
#include <misc/tt_reference_counter.h>
#include <misc/tt_util.h>
#include <network/adns/tt_adns_domain.h>
#include <network/adns/tt_adns_domain_manager.h>
#include <network/adns/tt_adns_domain_name.h>
#include <network/adns/tt_adns_init.h>
#include <network/adns/tt_adns_packet.h>
#include <network/adns/tt_adns_query.h>
#include <network/adns/tt_adns_question.h>
#include <network/adns/tt_adns_resolver.h>
#include <network/adns/tt_adns_rr.h>
#include <network/adns/tt_adns_rr_a_in.h>
#include <network/adns/tt_adns_rr_aaaa_in.h>
#include <network/adns/tt_adns_rr_cname_in.h>
#include <network/ssh/context/tt_ssh_context.h>
#include <network/ssh/context/tt_ssh_encrypt.h>
#include <network/ssh/context/tt_ssh_encrypt_aes.h>
#include <network/ssh/context/tt_ssh_kdf.h>
#include <network/ssh/context/tt_ssh_kex.h>
#include <network/ssh/context/tt_ssh_kex_dh.h>
#include <network/ssh/context/tt_ssh_mac.h>
#include <network/ssh/context/tt_ssh_mac_sha.h>
#include <network/ssh/context/tt_ssh_pubkey.h>
#include <network/ssh/message/tt_ssh_message.h>
#include <network/ssh/message/tt_ssh_msg_channel_close.h>
#include <network/ssh/message/tt_ssh_msg_channel_data.h>
#include <network/ssh/message/tt_ssh_msg_channel_failure.h>
#include <network/ssh/message/tt_ssh_msg_channel_open.h>
#include <network/ssh/message/tt_ssh_msg_channel_open_confirmation.h>
#include <network/ssh/message/tt_ssh_msg_channel_open_failure.h>
#include <network/ssh/message/tt_ssh_msg_channel_request.h>
#include <network/ssh/message/tt_ssh_msg_channel_success.h>
#include <network/ssh/message/tt_ssh_msg_channel_window_adjust.h>
#include <network/ssh/message/tt_ssh_msg_disconnect.h>
#include <network/ssh/message/tt_ssh_msg_global_request.h>
#include <network/ssh/message/tt_ssh_msg_ignore.h>
#include <network/ssh/message/tt_ssh_msg_kexdh_init.h>
#include <network/ssh/message/tt_ssh_msg_kexdh_reply.h>
#include <network/ssh/message/tt_ssh_msg_keyinit.h>
#include <network/ssh/message/tt_ssh_msg_newkeys.h>
#include <network/ssh/message/tt_ssh_msg_request_failure.h>
#include <network/ssh/message/tt_ssh_msg_request_success.h>
#include <network/ssh/message/tt_ssh_msg_service_accept.h>
#include <network/ssh/message/tt_ssh_msg_service_request.h>
#include <network/ssh/message/tt_ssh_msg_userauth_banner.h>
#include <network/ssh/message/tt_ssh_msg_userauth_failure.h>
#include <network/ssh/message/tt_ssh_msg_userauth_request.h>
#include <network/ssh/message/tt_ssh_msg_userauth_success.h>
#include <network/ssh/message/tt_ssh_msg_verxchg.h>
#include <network/ssh/message/tt_ssh_parse.h>
#include <network/ssh/message/tt_ssh_render.h>
#include <network/ssh/server/tt_ssh_server.h>
#include <network/ssh/server/tt_ssh_server_conn.h>
#include <network/ssh/server/tt_ssh_server_fsm.h>
#include <network/ssh/server/tt_ssh_server_state_auth.h>
#include <network/ssh/server/tt_ssh_server_state_authdone.h>
#include <network/ssh/server/tt_ssh_server_state_kexdh.h>
#include <network/ssh/server/tt_ssh_server_state_kexdone.h>
#include <network/ssh/server/tt_ssh_server_state_keyxchg.h>
#include <network/ssh/server/tt_ssh_server_state_verxchg.h>
#include <network/ssh/tt_ssh_channel.h>
#include <network/ssh/tt_ssh_channel_cb.h>
#include <network/ssh/tt_ssh_channel_manager.h>
#include <network/ssh/tt_ssh_def.h>
#include <network/ssl/tt_ssl.h>
#include <network/ssl/tt_ssl_aio.h>
#include <network/ssl/tt_ssl_aio_cb.h>
#include <network/ssl/tt_ssl_cache.h>
#include <network/ssl/tt_ssl_context.h>
#include <network/ssl/tt_ssl_def.h>
#include <network/tt_network_def.h>
#include <network/tt_network_interface.h>
#include <os/tt_atomic.h>
#include <os/tt_dll.h>
#include <os/tt_mutex.h>
#include <os/tt_process.h>
#include <os/tt_rwlock.h>
#include <os/tt_semaphore.h>
#include <os/tt_spinlock.h>
#include <timer/tt_time_reference.h>
#include <timer/tt_time_util.h>
#include <timer/tt_timer.h>
#include <timer/tt_timer_manager.h>
#include <tt_basic_type.h>
#include <unit_test/tt_test_framework.h>
#include <unit_test/tt_unit_test.h>
#include <xml/parse/tt_xml_parser.h>
#include <xml/parse/tt_xmlparse_state.h>
#include <xml/parse/tt_xmlparse_state_attr.h>
#include <xml/parse/tt_xmlparse_state_cdata.h>
#include <xml/parse/tt_xmlparse_state_comment.h>
#include <xml/parse/tt_xmlparse_state_etag.h>
#include <xml/parse/tt_xmlparse_state_exclamation.h>
#include <xml/parse/tt_xmlparse_state_pi.h>
#include <xml/parse/tt_xmlparse_state_stag.h>
#include <xml/parse/tt_xmlparse_state_stagend.h>
#include <xml/parse/tt_xmlparse_state_tag.h>
#include <xml/parse/tt_xmlparse_state_text.h>
#include <xml/parse/tt_xmlparse_state_waitattr.h>
#include <xml/render/tt_xml_render.h>
#include <xml/tt_xml_char_decode.h>
#include <xml/tt_xml_char_encode.h>
#include <xml/tt_xml_def.h>
#include <xml/tt_xml_document.h>
#include <xml/tt_xml_memory.h>
#include <xml/tt_xml_namespace.h>
#include <xml/tt_xml_node.h>
#include <xml/tt_xml_node_element.h>
#include <xml/tt_xml_node_parser.h>
#include <xml/tt_xml_node_render.h>

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_PLATFORM__ */
