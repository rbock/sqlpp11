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

#include <sqlpp23/core/detail/type_set.h>
#include <sqlpp23/core/detail/type_vector.h>
#include <sqlpp23/core/query/dynamic_fwd.h>
#include <sqlpp23/core/type_traits/nodes_of.h>

namespace sqlpp {
// `required_ctes_of` recursively determines the type_set of ctes referenced
// within `T`. `cte_ref_t` and other structs that might reference a cte shall
// specialize this template to indicate their cte requirement.
template <typename T> struct required_ctes_of {
  using type = typename required_ctes_of<nodes_of_t<T>>::type;
};

template <typename... T> struct required_ctes_of<detail::type_vector<T...>> {
  using type = detail::make_joined_set_t<typename required_ctes_of<T>::type...>;
};

template <typename T>
using required_ctes_of_t = typename required_ctes_of<T>::type;

// `required_static_ctes_of` recursively determines the type_set of ctes
// statically referenced within `T`. `cte_ref_t` and other structs that might
// reference a cte shall specialize this template to indicate their cte
// requirement.
//
// Dynamic query parts are ignored.
template <typename T> struct required_static_ctes_of {
  using type = typename required_static_ctes_of<nodes_of_t<T>>::type;
};

template <typename T> struct required_static_ctes_of<dynamic_t<T>> {
  using type = detail::type_set<>;
};

template <typename... T>
struct required_static_ctes_of<detail::type_vector<T...>> {
  using type =
      detail::make_joined_set_t<typename required_static_ctes_of<T>::type...>;
};

template <typename T>
using required_static_ctes_of_t = typename required_static_ctes_of<T>::type;

// `provided_ctes_of` determines the type_set of ctes provided by a clause, e.g.
// by WITH. `cte_t` or other structs that might provide a cte in a query need to
// specialize this template.
//
// Note: In contrast to `required_ctes_of` above, `provided_ctes_of` is
// non-recursive.
template <typename T> struct provided_ctes_of {
  // This needs to the specialized by `cte_ref_t`.
  using type = detail::type_set<>;
};

template <typename T>
struct provided_ctes_of<dynamic_t<T>> : public provided_ctes_of<T> {};

template <typename T>
using provided_ctes_of_t = typename provided_ctes_of<T>::type;

// `provided_static_ctes_of` determines the type_vector of non-dynamic ctes
// provided by a clause, e.g. by WITH.
template <typename T>
struct provided_static_ctes_of : public provided_ctes_of<T> {};

template <typename T> struct provided_static_ctes_of<dynamic_t<T>> {
  using type = detail::type_set<>;
};

template <typename T>
using provided_static_ctes_of_t = typename provided_static_ctes_of<T>::type;

} // namespace sqlpp
