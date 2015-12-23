/*
 * Copyright (c) 2013-2015, Roland Bock
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

#ifndef SQLPP_MULTI_COLUMN_H
#define SQLPP_MULTI_COLUMN_H

#include <sqlpp11/logic.h>
#include <sqlpp11/detail/type_set.h>

#include <sqlpp11/detail/copy_tuple_args.h>

namespace sqlpp
{
  struct no_value_t;

  template <typename AliasProvider, typename... Columns>
  struct multi_column_alias_t;

  template <typename Unused, typename... Columns>
  struct multi_column_t
  {
    using _traits = make_traits<no_value_t>;
    using _nodes = detail::type_vector<Columns...>;

    static_assert(logic::all_t<is_selectable_t<Columns>::value...>::value,
                  "multi_column parameters need to be named expressions");

    multi_column_t(std::tuple<Columns...> columns) : _columns(columns)
    {
    }

    multi_column_t(Columns... columns) : _columns(columns...)
    {
    }

    multi_column_t(const multi_column_t&) = default;
    multi_column_t(multi_column_t&&) = default;
    multi_column_t& operator=(const multi_column_t&) = default;
    multi_column_t& operator=(multi_column_t&&) = default;
    ~multi_column_t() = default;

    template <typename AliasProvider>
    multi_column_alias_t<AliasProvider, Columns...> as(const AliasProvider&)
    {
      return {*this};
    }

    std::tuple<Columns...> _columns;
  };

  template <typename AliasProvider, typename... Columns>
  struct multi_column_alias_t
  {
    using _traits = make_traits<no_value_t, tag::is_alias, tag::is_multi_column, tag::is_selectable>;
    using _nodes = detail::type_vector<Columns...>;

    static_assert(logic::all_t<is_selectable_t<Columns>::value...>::value,
                  "multi_column parameters need to be named expressions");

    using _alias_t = typename AliasProvider::_alias_t;

    multi_column_alias_t(multi_column_t<void, Columns...> multi_column) : _columns(multi_column._columns)
    {
    }

    multi_column_alias_t(std::tuple<Columns...> columns) : _columns(columns)
    {
    }

    multi_column_alias_t(Columns... columns) : _columns(columns...)
    {
    }

    multi_column_alias_t(const multi_column_alias_t&) = default;
    multi_column_alias_t(multi_column_alias_t&&) = default;
    multi_column_alias_t& operator=(const multi_column_alias_t&) = default;
    multi_column_alias_t& operator=(multi_column_alias_t&&) = default;
    ~multi_column_alias_t() = default;

    std::tuple<Columns...> _columns;
  };

  template <typename Context, typename... Columns>
  struct serializer_t<Context, multi_column_t<void, Columns...>>
  {
    using _serialize_check = serialize_check_of<Context, Columns...>;
    using T = multi_column_t<void, Columns...>;

    static void _(const T&, Context&)
    {
      static_assert(wrong_t<serializer_t>::value, "multi_column must be used with an alias");
    }
  };

  template <typename Context, typename AliasProvider, typename... Columns>
  struct serializer_t<Context, multi_column_alias_t<AliasProvider, Columns...>>
  {
    using _serialize_check = serialize_check_of<Context, Columns...>;
    using T = multi_column_alias_t<AliasProvider, Columns...>;

    static Context& _(const T& t, Context& context)
    {
      interpret_tuple(t._columns, ',', context);
      return context;
    }
  };

  namespace detail
  {
    template <typename... Columns>
    using make_multi_column_t =
        copy_tuple_args_t<multi_column_t, void, decltype(column_tuple_merge(std::declval<Columns>()...))>;
  }

  template <typename... Columns>
  auto multi_column(Columns... columns) -> detail::make_multi_column_t<Columns...>
  {
    return detail::make_multi_column_t<Columns...>(std::tuple_cat(detail::as_column_tuple<Columns>::_(columns)...));
  }
}

#endif
