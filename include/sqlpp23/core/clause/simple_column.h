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

#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename Column>
struct simple_column_t {
  simple_column_t(const Column& column) : _column{column} {}
  simple_column_t(const simple_column_t&) = default;
  simple_column_t(simple_column_t&&) = default;
  simple_column_t& operator=(const simple_column_t&) = default;
  simple_column_t& operator=(simple_column_t&&) = default;
  ~simple_column_t() = default;

 private:
  Column _column;
};

template <typename Context, typename Column>
auto to_sql_string(Context& context, const simple_column_t<Column>&)
    -> std::string {
  return name_to_sql_string(context, name_tag_of_t<Column>{});
}

template <typename Column>
struct make_simple_column {
  using type = simple_column_t<Column>;
};

template <typename Column>
struct make_simple_column<dynamic_t<Column>> {
  using type = dynamic_t<simple_column_t<Column>>;
};

template <typename Column>
using make_simple_column_t = typename make_simple_column<Column>::type;

template <typename Column>
auto simple_column(Column c) -> simple_column_t<Column> {
  return {std::move(c)};
}

template <typename Column>
auto simple_column(dynamic_t<Column> c) -> dynamic_t<simple_column_t<Column>> {
  return {c.has_value(), std::move(c._value())};
}
}  // namespace sqlpp
