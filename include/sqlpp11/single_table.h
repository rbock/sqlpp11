/*
 * Copyright (c) 2013-2016, Roland Bock
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

#ifndef SQLPP_SINGLE_TABLE_H
#define SQLPP_SINGLE_TABLE_H

#include <sqlpp11/data_types/no_value.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // A SINGLE TABLE DATA
  template <typename Database, typename Table>
  struct single_table_data_t
  {
    single_table_data_t(Table table) : _table(table)
    {
    }

    single_table_data_t(const single_table_data_t&) = default;
    single_table_data_t(single_table_data_t&&) = default;
    single_table_data_t& operator=(const single_table_data_t&) = default;
    single_table_data_t& operator=(single_table_data_t&&) = default;
    ~single_table_data_t() = default;

    Table _table;
  };

  // A SINGLE TABLE
  template <typename Database, typename Table>
  struct single_table_t
  {
    using _traits = make_traits<no_value_t, tag::is_single_table>;
    using _nodes = detail::type_vector<Table>;

    static_assert(is_table_t<Table>::value, "argument has to be a table");
    static_assert(required_tables_of<Table>::size::value == 0, "table depends on another table");

    using _data_t = single_table_data_t<Database, Table>;

    struct _alias_t
    {
    };

    // Member implementation with data and methods
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

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = single_table_data_t<Database, Table>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : from{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> from;
      _impl_t<Policies>& operator()()
      {
        return from;
      }
      const _impl_t<Policies>& operator()() const
      {
        return from;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.from)
      {
        return t.from;
      }

      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_update_table_arg_is_table_t, "argument for update() must be a table");
  template <typename Table>
  struct check_update_table
  {
    using type = static_combined_check_t<static_check_t<is_table_t<Table>::value, assert_update_table_arg_is_table_t>>;
  };
  template <typename Table>
  using check_update_table_t = typename check_update_table<Table>::type;

  // NO TABLE YET
  struct no_single_table_t
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

      _impl_t<Policies> no_from;
      _impl_t<Policies>& operator()()
      {
        return no_from;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_from;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_from)
      {
        return t.no_from;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_single_table_t, T>;

      using _consistency_check = consistent_t;

      template <typename Table>
      auto single_table(Table table) const -> _new_statement_t<check_update_table_t<Table>, single_table_t<void, Table>>
      {
        return _single_table_impl<void>(check_update_table_t<Table>{}, table);
      }

    private:
      template <typename Database, typename Check, typename Table>
      auto _single_table_impl(Check, Table table) const -> inconsistent<Check>;

      template <typename Database, typename Table>
      auto _single_table_impl(consistent_t, Table table) const
          -> _new_statement_t<consistent_t, single_table_t<Database, Table>>
      {
        static_assert(required_tables_of<single_table_t<Database, Table>>::size::value == 0,
                      "argument depends on another table in single_table()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), single_table_data_t<Database, Table>{table}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename Table>
  struct serializer_t<Context, single_table_data_t<Database, Table>>
  {
    using _serialize_check = serialize_check_of<Context, Table>;
    using T = single_table_data_t<Database, Table>;

    static Context& _(const T& t, Context& context)
    {
      serialize(t._table, context);
      return context;
    }
  };
}

#endif
