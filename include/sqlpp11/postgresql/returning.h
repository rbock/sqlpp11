#pragma once

/**
 * Copyright © 2014-2018, Matthijs Möhlmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
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

#include <sqlpp11/postgresql/returning_column_list.h>
#include <sqlpp11/select_column_list.h>
#include <sqlpp11/statement.h>

namespace sqlpp
{
  SQLPP_VALUE_TRAIT_GENERATOR(is_returning)

  namespace postgresql
  {
    struct return_name_t
    {
    };

    struct returning_t : public statement_name_t<return_name_t, tag::is_returning>
    {
    };

    template <typename Database>
    using blank_returning_t = statement_t<Database, returning_t, no_returning_column_list_t>;

    inline blank_returning_t<void> returning()
    {
      return {};
    }

    struct no_returning_t
    {
      using _traits = make_traits<no_value_t, tag::is_returning>;
      using _nodes = ::sqlpp::detail::type_vector<>;

      using _data_t = no_data_t;

      template <typename Policies>
      struct _impl_t
      {
        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
        _impl_t() = default;
        _impl_t(const _data_t& data) : _data(data)
        {
        }

        _data_t _data;
      };

      template <typename Policies>
      struct _base_t
      {
        using _data_t = no_data_t;

        // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
        template <typename... Args>
        _base_t(Args&&... args) : no_returning{std::forward<Args>(args)...}
        {
        }

        _impl_t<Policies> no_returning;
        _impl_t<Policies>& operator()()
        {
          return no_returning;
        }
        const _impl_t<Policies>& operator()() const
        {
          return no_returning;
        }

        template <typename T>
        static auto _get_member(T t) -> decltype(t.no_returning)
        {
          return t.no_returning;
        }

        using _database_t = typename Policies::_database_t;

        template <typename... T>
        static constexpr auto _check_tuple(std::tuple<T...>) -> check_selected_columns_t<T...>
        {
          return {};
        }

        template <typename... T>
        static constexpr auto _check_args(T... args)
            -> decltype(_check_tuple(sqlpp::detail::column_tuple_merge(args...)))
        {
          return {};
        }

        template <typename Check, typename T>
        using _new_statement_t = new_statement_t<Check, Policies, no_returning_t, T>;

        using _consistency_check = consistent_t;

        template <typename... Columns>
        auto returning(Columns... columns) const
            -> _new_statement_t<decltype(_check_args(columns...)), returning_column_list_t<_database_t, Columns...>>
        {
          static_assert(sizeof...(Columns),
                        "at least one returnable expression (e.g. a column) is required in returning");
          static_assert(decltype(_check_args(columns...))::value,
                        "at least one argument is not a returnable expression in returning()");
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  typename returning_column_list_t<_database_t, Columns...>::_data_t{columns...}};
        }

        template <typename... Columns>
        auto returning(std::tuple<Columns...> columns) const
            -> _new_statement_t<decltype(_check_args(columns)), returning_column_list_t<_database_t, Columns...>>
        {
          static_assert(sizeof...(Columns),
                        "at least one returnable expression (e.g. a column) is required in returning");
          static_assert(decltype(_check_args(columns))::value,
                        "at least one argument is not a returnable expression in returning()");
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  typename returning_column_list_t<_database_t, Columns...>::_data_t{columns}};
        }

        template <typename... Columns>
        auto dynamic_returning(Columns... columns) const
            -> _new_statement_t<void, returning_column_list_t<_database_t, Columns...>>
        {
          static_assert(sizeof...(Columns),
                        "at least one returnable expression (e.g. a column) is required in returning");
          static_assert(decltype(_check_args(columns...))::value,
                        "at least one argument is not a returnable expression in returning()");
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  typename returning_column_list_t<_database_t, Columns...>::_data_t{columns...}};
        }

        template <typename Database, typename... Columns>
        auto dynamic_returning(const Database&, Columns... columns)
            -> decltype(blank_returning_t<Database>().columns(columns...))
        {
          static_assert(sizeof...(Columns),
                        "at least one returnable expression (e.g. a column) is required in returning");
          static_assert(decltype(_check_args(columns...))::value,
                        "at least one argument is not a returnable expression in returning()");
          static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
          return {static_cast<const derived_statement_t<Policies>&>(*this),
                  typename dynamic_returning_column_list<_database_t>::_data_t{columns...}};
        }
      };
    };
  }  // namespace postgresql
}  // namespace sqlpp
