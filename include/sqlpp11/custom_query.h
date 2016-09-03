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

#ifndef SQLPP_CUSTOM_QUERY_H
#define SQLPP_CUSTOM_QUERY_H

#include <sqlpp11/connection.h>
#include <sqlpp11/detail/get_first.h>
#include <sqlpp11/hidden.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/statement.h>

namespace sqlpp
{
  template <typename Database, typename... Parts>
  struct custom_query_t;

  namespace detail
  {
    template <typename T>
    struct unhide
    {
      using type = T;
    };
    template <typename Clause>
    struct unhide<hidden_t<Clause>>
    {
      using type = Clause;
    };

    template <typename Db, typename... Parts>
    struct custom_parts_t
    {
      using _custom_query_t = custom_query_t<Db, Parts...>;
      using _maybe_hidden_result_type_provider = detail::get_first_if<is_return_value_t, noop, Parts...>;
      using _result_type_provider = typename unhide<_maybe_hidden_result_type_provider>::type;
      using _result_methods_t = typename _result_type_provider::template _result_methods_t<_result_type_provider>;
    };
  }

  template <typename Database, typename... Parts>
  struct custom_query_t : private detail::custom_parts_t<Database, Parts...>::_result_methods_t
  {
    using _methods_t = typename detail::custom_parts_t<Database, Parts...>::_result_methods_t;
    using _traits = make_traits<no_value_t, tag::is_statement>;
    using _nodes = detail::type_vector<Parts...>;

    using _parameter_check =
        typename std::conditional<detail::type_vector_size<parameters_of<custom_query_t>>::value == 0,
                                  consistent_t,
                                  assert_no_parameters_t>::type;
    using _run_check = detail::get_first_if<is_inconsistent_t, consistent_t, _parameter_check>;
    using _prepare_check = consistent_t;

    custom_query_t(Parts... parts) : _parts(parts...)
    {
    }

    custom_query_t(std::tuple<Parts...> parts) : _parts(parts)
    {
    }

    custom_query_t(const custom_query_t&) = default;
    custom_query_t(custom_query_t&&) = default;
    custom_query_t& operator=(const custom_query_t&) = default;
    custom_query_t& operator=(custom_query_t&&) = default;
    ~custom_query_t() = default;

    template <typename Db>
    auto _run(Db& db) const -> decltype(std::declval<_methods_t>()._run(db, *this))
    {
      _run_check{};  // FIXME: dispatch here?
      return _methods_t::_run(db, *this);
    }

    template <typename Db>
    auto _prepare(Db& db) const -> decltype(std::declval<_methods_t>()._prepare(db, *this))
    {
      _prepare_check{};  // FIXME: dispatch here?
      return _methods_t::_prepare(db, *this);
    }

    static constexpr size_t _get_static_no_of_parameters()
    {
      return std::tuple_size<parameters_of<custom_query_t>>::value;
    }

    size_t _get_no_of_parameters() const
    {
      return _get_static_no_of_parameters();
    }

    template <typename Part>
    auto with_result_type_of(Part part) -> custom_query_t<Database, hidden_t<Part>, Parts...>
    {
      return {tuple_cat(std::make_tuple(hidden(part)), _parts)};
    }

    std::tuple<Parts...> _parts;
  };

  template <typename Context, typename Database, typename... Parts>
  struct serializer_t<Context, custom_query_t<Database, Parts...>>
  {
    using _serialize_check = serialize_check_of<Context, Parts...>;
    using T = custom_query_t<Database, Parts...>;

    static Context& _(const T& t, Context& context)
    {
      interpret_tuple_without_braces(t._parts, " ", context);
      return context;
    }
  };

  template <typename... Parts>
  auto custom_query(Parts... parts) -> custom_query_t<void, wrap_operand_t<Parts>...>
  {
    static_assert(sizeof...(Parts) > 0, "custom query requires at least one argument");
    return custom_query_t<void, wrap_operand_t<Parts>...>(parts...);
  }

  template <typename Database, typename... Parts>
  auto dynamic_custom_query(const Database&, Parts... parts) -> custom_query_t<Database, wrap_operand_t<Parts>...>
  {
    static_assert(sizeof...(Parts) > 0, "custom query requires at least one query argument");
    static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");

    return custom_query_t<Database, wrap_operand_t<Parts>...>(parts...);
  }
}
#endif
