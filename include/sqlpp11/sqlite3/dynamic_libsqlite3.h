#pragma once

/*
 * Copyright (c) 2017, Volker Aßmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <stdexcept>

// namespace for internal sqlite function wrappers - when using this instead of sqlite3 direct linking
// do this:
//    using namespace sqlpp::sqlite3::dyn;
// to override the libsqlite3 functions with these function pointers

#ifdef SQLPP_DYNAMIC_LOADING

namespace sqlpp
{
  namespace sqlite3
  {
    namespace dynamic
    {
      /// load the SQLite libraries, optionally providing the filename (leave empty for default)
      void init_sqlite(std::string libname);

#define DYNDEFINE(NAME) extern decltype(::NAME)* NAME

      DYNDEFINE(sqlite3_open_v2);
      DYNDEFINE(sqlite3_open);
      DYNDEFINE(sqlite3_prepare_v2);
      DYNDEFINE(sqlite3_exec);
      DYNDEFINE(sqlite3_step);
      DYNDEFINE(sqlite3_bind_int);
      DYNDEFINE(sqlite3_close);
      DYNDEFINE(sqlite3_initialize);
      DYNDEFINE(sqlite3_os_init);
      DYNDEFINE(sqlite3_os_end);
      DYNDEFINE(sqlite3_bind_blob);
      DYNDEFINE(sqlite3_bind_double);
      DYNDEFINE(sqlite3_bind_int64);
      DYNDEFINE(sqlite3_bind_null);
      DYNDEFINE(sqlite3_bind_text);
      DYNDEFINE(sqlite3_bind_value);
      DYNDEFINE(sqlite3_libversion_number);
      //    DYNDEFINE(sqlite3_compileoption_used);
      DYNDEFINE(sqlite3_threadsafe);
      //    DYNDEFINE(sqlite3_close_v2);
      DYNDEFINE(sqlite3_shutdown);
      DYNDEFINE(sqlite3_config);
      DYNDEFINE(sqlite3_db_config);
      DYNDEFINE(sqlite3_extended_result_codes);
      DYNDEFINE(sqlite3_last_insert_rowid);
      DYNDEFINE(sqlite3_changes);
      DYNDEFINE(sqlite3_total_changes);
      DYNDEFINE(sqlite3_interrupt);
      DYNDEFINE(sqlite3_complete);
      DYNDEFINE(sqlite3_complete16);
      DYNDEFINE(sqlite3_busy_handler);
      DYNDEFINE(sqlite3_busy_timeout);
      DYNDEFINE(sqlite3_get_table);
      DYNDEFINE(sqlite3_free_table);
      //    DYNDEFINE(sqlite3_realloc64);
      DYNDEFINE(sqlite3_free);
      //    DYNDEFINE(sqlite3_msize);
      DYNDEFINE(sqlite3_memory_used);
      DYNDEFINE(sqlite3_memory_highwater);
      DYNDEFINE(sqlite3_randomness);
      DYNDEFINE(sqlite3_set_authorizer);
      DYNDEFINE(sqlite3_progress_handler);
      DYNDEFINE(sqlite3_open16);
      //    DYNDEFINE(sqlite3_uri_boolean);
      //    DYNDEFINE(sqlite3_uri_int64);
      DYNDEFINE(sqlite3_errcode);
      DYNDEFINE(sqlite3_errmsg);
      DYNDEFINE(sqlite3_extended_errcode);
      DYNDEFINE(sqlite3_limit);
      DYNDEFINE(sqlite3_prepare);
      DYNDEFINE(sqlite3_prepare16);
      DYNDEFINE(sqlite3_prepare16_v2);
      //    DYNDEFINE(sqlite3_stmt_readonly);
      //    DYNDEFINE(sqlite3_stmt_busy);
      //    DYNDEFINE(sqlite3_bind_blob64);
      DYNDEFINE(sqlite3_bind_text16);
      //    DYNDEFINE(sqlite3_bind_text64);
      DYNDEFINE(sqlite3_bind_zeroblob);
      DYNDEFINE(sqlite3_bind_parameter_count);
      DYNDEFINE(sqlite3_bind_parameter_index);
      DYNDEFINE(sqlite3_clear_bindings);
      DYNDEFINE(sqlite3_column_count);
      DYNDEFINE(sqlite3_data_count);
      DYNDEFINE(sqlite3_column_bytes);
      DYNDEFINE(sqlite3_column_bytes16);
      DYNDEFINE(sqlite3_column_double);
      DYNDEFINE(sqlite3_column_int);
      DYNDEFINE(sqlite3_column_int64);
      DYNDEFINE(sqlite3_column_text);
      DYNDEFINE(sqlite3_column_type);
      DYNDEFINE(sqlite3_column_value);
      DYNDEFINE(sqlite3_column_blob);
      DYNDEFINE(sqlite3_finalize);
      DYNDEFINE(sqlite3_reset);
      DYNDEFINE(sqlite3_create_function);
      DYNDEFINE(sqlite3_create_function16);
      //    DYNDEFINE(sqlite3_create_function_v2);
      DYNDEFINE(sqlite3_value_bytes);
      DYNDEFINE(sqlite3_value_bytes16);
      DYNDEFINE(sqlite3_value_double);
      DYNDEFINE(sqlite3_value_int);
      DYNDEFINE(sqlite3_value_int64);
      DYNDEFINE(sqlite3_value_type);
      DYNDEFINE(sqlite3_value_numeric_type);
      DYNDEFINE(sqlite3_set_auxdata);
      DYNDEFINE(sqlite3_result_blob);
      //    DYNDEFINE(sqlite3_result_blob64);
      DYNDEFINE(sqlite3_result_double);
      DYNDEFINE(sqlite3_result_error);
      DYNDEFINE(sqlite3_result_error16);
      DYNDEFINE(sqlite3_result_error_toobig);
      DYNDEFINE(sqlite3_result_error_nomem);
      DYNDEFINE(sqlite3_result_error_code);
      DYNDEFINE(sqlite3_result_int);
      DYNDEFINE(sqlite3_result_int64);
      DYNDEFINE(sqlite3_result_null);
      DYNDEFINE(sqlite3_result_text);
      //    DYNDEFINE(sqlite3_result_text64);
      DYNDEFINE(sqlite3_result_text16);
      DYNDEFINE(sqlite3_result_text16le);
      DYNDEFINE(sqlite3_result_text16be);
      DYNDEFINE(sqlite3_result_value);
      DYNDEFINE(sqlite3_result_zeroblob);
      DYNDEFINE(sqlite3_create_collation);
      DYNDEFINE(sqlite3_create_collation_v2);
      DYNDEFINE(sqlite3_create_collation16);
      DYNDEFINE(sqlite3_collation_needed);
      DYNDEFINE(sqlite3_collation_needed16);

#ifdef SQLITE_HAS_CODEC

      DYNDEFINE(sqlite3_key);
      DYNDEFINE(sqlite3_key_v2);
      DYNDEFINE(sqlite3_rekey);
      DYNDEFINE(sqlite3_rekey_v2);
      DYNDEFINE(sqlite3_activate_see);
#endif
#ifdef SQLITE_ENABLE_CEROD
      DYNDEFINE(sqlite3_activate_cerod);
#endif
      DYNDEFINE(sqlite3_sleep);
      DYNDEFINE(sqlite3_get_autocommit);
      //    DYNDEFINE(sqlite3_db_readonly);
      DYNDEFINE(sqlite3_next_stmt);
      DYNDEFINE(sqlite3_enable_shared_cache);
      DYNDEFINE(sqlite3_release_memory);
      //    DYNDEFINE(sqlite3_db_release_memory);
      //    DYNDEFINE(sqlite3_soft_heap_limit64);
      DYNDEFINE(sqlite3_table_column_metadata);
      DYNDEFINE(sqlite3_load_extension);
      DYNDEFINE(sqlite3_enable_load_extension);
      DYNDEFINE(sqlite3_auto_extension);
      //    DYNDEFINE(sqlite3_cancel_auto_extension);
      DYNDEFINE(sqlite3_reset_auto_extension);
      DYNDEFINE(sqlite3_create_module);
      DYNDEFINE(sqlite3_create_module_v2);
      DYNDEFINE(sqlite3_declare_vtab);
      DYNDEFINE(sqlite3_overload_function);
      DYNDEFINE(sqlite3_blob_open);
      DYNDEFINE(sqlite3_blob_close);
      DYNDEFINE(sqlite3_blob_bytes);
      DYNDEFINE(sqlite3_blob_read);
      DYNDEFINE(sqlite3_blob_write);
      DYNDEFINE(sqlite3_vfs_find);
      DYNDEFINE(sqlite3_vfs_register);
      DYNDEFINE(sqlite3_vfs_unregister);
      DYNDEFINE(sqlite3_mutex_alloc);
      DYNDEFINE(sqlite3_mutex_free);
      DYNDEFINE(sqlite3_mutex_enter);
      DYNDEFINE(sqlite3_mutex_try);
      DYNDEFINE(sqlite3_mutex_leave);
      //    DYNDEFINE(sqlite3_mutex_held);
      //    DYNDEFINE(sqlite3_mutex_notheld);
      DYNDEFINE(sqlite3_db_mutex);
      DYNDEFINE(sqlite3_file_control);
      DYNDEFINE(sqlite3_test_control);
      DYNDEFINE(sqlite3_status);
      DYNDEFINE(sqlite3_db_status);
      DYNDEFINE(sqlite3_stmt_status);
      DYNDEFINE(sqlite3_backup_init);
      DYNDEFINE(sqlite3_backup_step);
      DYNDEFINE(sqlite3_backup_finish);
      DYNDEFINE(sqlite3_backup_remaining);
      DYNDEFINE(sqlite3_backup_pagecount);
      DYNDEFINE(sqlite3_unlock_notify);
      //    DYNDEFINE(sqlite3_stricmp);
      //    DYNDEFINE(sqlite3_strnicmp);
      //    DYNDEFINE(sqlite3_strglob);
      //    DYNDEFINE(sqlite3_log);
      //    DYNDEFINE(sqlite3_wal_autocheckpoint);
      //    DYNDEFINE(sqlite3_wal_checkpoint);
      //    DYNDEFINE(sqlite3_wal_checkpoint_v2);
      //    DYNDEFINE(sqlite3_vtab_config);
      //    DYNDEFINE(sqlite3_vtab_on_conflict);
      //    DYNDEFINE(sqlite3_rtree_geometry_callback);
      //    DYNDEFINE(sqlite3_rtree_query_callback);
    }  // namespace dynamic
  }    // namespace sqlite3
}  // namespace sqlpp

#undef DYNDEFINE

#endif
