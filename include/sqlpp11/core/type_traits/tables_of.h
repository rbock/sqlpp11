#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/detail/type_vector.h>
#include <sqlpp11/core/query/dynamic_fwd.h>
#include <sqlpp11/core/type_traits/nodes_of.h>

namespace sqlpp {
// `required_tables_of` recursively determines the type_set of tables referenced
// by columns within `T`. `column_t` or other structs that might reference a
// table shall specialize this template to indicate their table requirement.
template <typename T> struct required_tables_of {
  using type = typename required_tables_of<nodes_of_t<T>>::type;
};

template <typename... T> struct required_tables_of<detail::type_vector<T...>> {
  using type =
      detail::make_joined_set_t<typename required_tables_of<T>::type...>;
};

template <typename T>
using required_tables_of_t = typename required_tables_of<T>::type;

// `required_static_tables_of` recursively determines the type_set of tables
// statically referenced by columns within `T`. `column_t` or other structs that
// might reference a table shall specialize this template to indicate their
// table requirement.
//
// Dynamic query parts are ignored.
template <typename T> struct required_static_tables_of {
  using type = typename required_static_tables_of<nodes_of_t<T>>::type;
};

template <typename T> struct required_static_tables_of<dynamic_t<T>> {
  using type = detail::type_set<>;
};

template <typename... T>
struct required_static_tables_of<detail::type_vector<T...>> {
  using type =
      detail::make_joined_set_t<typename required_static_tables_of<T>::type...>;
};

template <typename T>
using required_static_tables_of_t = typename required_static_tables_of<T>::type;

// `provided_tables_of` determines the type_set of tables provided by a clause,
// e.g. by FROM. `table_t`, `cte_ref_t`, or other structs that might provide a
// table in a query need to specialize this template.
//
// Note: In contrast to `required_tables_of` above, `provided_tables_of` is
// non-recursive. This is important for instance to prevent `SELECT...AS` to
// leak from `select_column_list`.
template <typename T> struct provided_tables_of {
  using type = detail::type_set<>;
};

template <typename T>
struct provided_tables_of<dynamic_t<T>> : public provided_tables_of<T> {};

template <typename T>
using provided_tables_of_t = typename provided_tables_of<T>::type;

// `provided_static_tables_of` determines the type_set of non-dynamic tables
// provided by a clause, e.g. by FROM.
template <typename T>
struct provided_static_tables_of : public provided_tables_of<T> {};

template <typename T> struct provided_static_tables_of<dynamic_t<T>> {
  using type = detail::type_set<>;
};

template <typename T>
using provided_static_tables_of_t = typename provided_static_tables_of<T>::type;

// `provided_optional_tables_of` determines the type_set of outer join tables
// provided by a clause, e.g. the right hand side table in a `left_outer_join`.
template <typename T> struct provided_optional_tables_of {
  using type = detail::type_set<>;
};

template <typename T>
using provided_optional_tables_of_t =
    typename provided_optional_tables_of<T>::type;

} // namespace sqlpp
