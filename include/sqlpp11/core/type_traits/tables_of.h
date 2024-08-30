#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/core/type_traits/nodes_of.h>
#include <sqlpp11/core/detail/type_vector.h>

namespace sqlpp
{
  // required_tables_of determines the type_vector of tables referenced by columns within within T.
  // column_t or other structs that might reference a table shall specialize this template to indicate their table
  // requirement.
  // Dynamic parts of a query shall wrap their required tables in dynamic_t.
  template<typename T>
  struct required_tables_of
  {
    using type = typename required_tables_of<nodes_of_t<T>>::type;
  };

  template<typename... T>
  struct required_tables_of<detail::type_vector<T...>>
  {
    using type = detail::type_vector_cat_t<typename required_tables_of<T>::type...>;
  };

  template<typename T>
    using required_tables_of_t = typename required_tables_of<T>::type;

  template<typename T>
  struct required_static_tables_of
  {
    using type = typename required_static_tables_of<nodes_of_t<T>>::type;
  };

  template<typename... T>
  struct required_static_tables_of<detail::type_vector<T...>>
  {
    using type = detail::type_vector_cat_t<typename required_static_tables_of<T>::type...>;
  };

  template<typename T>
    using required_static_tables_of_t = typename required_static_tables_of<T>::type;

#warning: need type tests...
  //static_assert(required_tables_of_t<int>::size::value == 0, "");

  // provided_tables_of determines the type_vector of tables provided by the query clause, e.g. by FROM.
  // Provided tables can be wrapped in dynamic_t if they are provided through a dynamic join.
  // table_t, cte_ref_t, or other structs that might provide a table in a query need to specialize this template.
  template <typename T>
  struct provided_tables_of
  {
    using type = typename provided_tables_of<nodes_of_t<T>>::type;
  };

  template <typename... T>
  struct provided_tables_of<detail::type_vector<T...>>
  {
    using type = detail::type_vector_cat_t<typename provided_tables_of<T>::type...>;
  };

  template <typename T>
  using provided_tables_of_t = typename provided_tables_of<T>::type;

  template <typename T>
  struct provided_static_tables_of
  {
    using type = typename provided_static_tables_of<nodes_of_t<T>>::type;
  };

  template <typename... T>
  struct provided_static_tables_of<detail::type_vector<T...>>
  {
    using type = detail::type_vector_cat_t<typename provided_static_tables_of<T>::type...>;
  };

  template <typename T>
  using provided_static_tables_of_t = typename provided_static_tables_of<T>::type;

  template <typename T>
  struct provided_optional_tables_of
  {
    using type = typename provided_optional_tables_of<nodes_of_t<T>>::type;
  };

  template <typename... T>
  struct provided_optional_tables_of<detail::type_vector<T...>>
  {
    using type = detail::type_vector_cat_t<typename provided_optional_tables_of<T>::type...>;
  };

  template <typename T>
  using provided_optional_tables_of_t = typename provided_optional_tables_of<T>::type;

  static_assert(provided_tables_of_t<int>::empty(), "");

}  // namespace sqlpp

