#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/core/basic/table_ref.h>
#include <sqlpp23/core/concepts.h>
#include <sqlpp23/core/detail/type_set.h>
#include <sqlpp23/core/query/dynamic.h>
#include <sqlpp23/core/query/statement.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
// USING is used in DELETE statements in
// * PostgreSQL
// * MySQL

// USING
template <typename _Table> struct using_t {
  using_t(_Table table) : _table(table) {}

  using_t(const using_t &) = default;
  using_t(using_t &&) = default;
  using_t &operator=(const using_t &) = default;
  using_t &operator=(using_t &&) = default;
  ~using_t() = default;

  template <typename Context>
  friend auto to_sql_string(Context& context, const using_t& t) -> std::string {
    if constexpr (is_dynamic<_Table>::value) {
      if (t._table.has_value()) {
        return " USING " + to_sql_string(context, t._table.value());
      }
      return {};
    } else {
      return " USING " + to_sql_string(context, t._table);
    }
  }

private:
  _Table _table;
};

template <typename _Table>
struct is_clause<using_t<_Table>> : public std::true_type {};

template <typename _Table> struct nodes_of<using_t<_Table>> {
  using type = detail::type_vector<_Table>;
};

template <typename Statement, typename _Table>
struct consistency_check<Statement, using_t<_Table>> {
  using type = consistent_t;
};

template <typename _Table>
struct provided_tables_of<using_t<_Table>> : public provided_tables_of<_Table> {
};

template <typename _Table>
struct provided_optional_tables_of<using_t<_Table>>
    : public provided_optional_tables_of<_Table> {};

// NO USING YET
struct no_using_t {
  template <typename Statement, DynamicTable _Table>
  auto using_(this Statement &&statement, _Table table) {
    return new_statement<no_using_t>(
        std::forward<Statement>(statement),
        using_t<table_ref_t<_Table>>{make_table_ref(table)});
  }

  template <typename Context>
  friend auto to_sql_string(Context&, const no_using_t&) -> std::string {
    return {};
  }
};

template <typename Statement> struct consistency_check<Statement, no_using_t> {
  using type = consistent_t;
};

template <DynamicTable T> auto using_(T t) {
  return statement_t<no_using_t>{}.using_(std::move(t));
}

} // namespace sqlpp
