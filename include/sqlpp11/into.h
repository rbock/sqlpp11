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

#ifndef SQLPP_INTO_H
#define SQLPP_INTO_H

#include <sqlpp11/statement_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/detail/type_set.h>

namespace sqlpp
{
  // A SINGLE TABLE DATA
  template <typename Database, typename Table>
  struct into_data_t
  {
    into_data_t(Table table) : _table(table)
    {
    }

    into_data_t(const into_data_t&) = default;
    into_data_t(into_data_t&&) = default;
    into_data_t& operator=(const into_data_t&) = default;
    into_data_t& operator=(into_data_t&&) = default;
    ~into_data_t() = default;

    Table _table;
  };

  // A SINGLE TABLE
  template <typename Database, typename Table>
  struct into_t
  {
    using _traits = make_traits<no_value_t, tag::is_into>;
    using _nodes = detail::type_vector<Table>;

    using _data_t = into_data_t<Database, Table>;

    struct _alias_t
    {
    };

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = into_data_t<Database, Table>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args)
          : into{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> into;
      _impl_t<Policies>& operator()()
      {
        return into;
      }
      const _impl_t<Policies>& operator()() const
      {
        return into;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.into)
      {
        return t.into;
      }

      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_into_t, "into() required");

  // NO INTO YET
  struct no_into_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = no_data_t;

      _impl_t<Policies> no_into;
      _impl_t<Policies>& operator()()
      {
        return no_into;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_into;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_into)
      {
        return t.no_into;
      }

      using _database_t = typename Policies::_database_t;

      template <typename T>
      using _check = logic::all_t<is_raw_table_t<T>::value>;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check::value, Policies, no_into_t, T>;

      using _consistency_check = assert_into_t;

      template <typename Table>
      auto into(Table table) const -> _new_statement_t<_check<Table>, into_t<void, Table>>
      {
        static_assert(_check<Table>::value, "argument is not a raw table in into()");
        return _into_impl<void>(_check<Table>{}, table);
      }

    private:
      template <typename Database, typename Table>
      auto _into_impl(const std::false_type&, Table table) const -> bad_statement;

      template <typename Database, typename Table>
      auto _into_impl(const std::true_type&, Table table) const
          -> _new_statement_t<std::true_type, into_t<Database, Table>>
      {
        static_assert(required_tables_of<into_t<Database, Table>>::size::value == 0,
                      "argument depends on another table in into()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), into_data_t<Database, Table>{table}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename Table>
  struct serializer_t<Context, into_data_t<Database, Table>>
  {
    using _serialize_check = serialize_check_of<Context, Table>;
    using T = into_data_t<Database, Table>;

    static Context& _(const T& t, Context& context)
    {
      context << " INTO ";
      serialize(t._table, context);
      return context;
    }
  };

  template <typename T>
  auto into(T&& t) -> decltype(statement_t<void, no_into_t>().into(std::forward<T>(t)))
  {
    return statement_t<void, no_into_t>().into(std::forward<T>(t));
  }
}

#endif
