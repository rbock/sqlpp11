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

#ifndef SQLPP_USING_H
#define SQLPP_USING_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/interpretable_list.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/policy_update.h>

namespace sqlpp
{
  // USING DATA
  template <typename Database, typename... Tables>
  struct using_data_t
  {
    using_data_t(Tables... tables) : _tables(tables...)
    {
    }

    using_data_t(const using_data_t&) = default;
    using_data_t(using_data_t&&) = default;
    using_data_t& operator=(const using_data_t&) = default;
    using_data_t& operator=(using_data_t&&) = default;
    ~using_data_t() = default;

    std::tuple<Tables...> _tables;
    interpretable_list_t<Database> _dynamic_tables;
  };

  // USING
  template <typename Database, typename... Tables>
  struct using_t
  {
    using _traits = make_traits<no_value_t, tag::is_using_>;
    using _nodes = detail::type_vector<Tables...>;

    using _is_dynamic = is_database<Database>;

    // Data
    using _data_t = using_data_t<Database, Tables...>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
// workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
	  _impl_t() = default;
	  _impl_t(const _data_t &data) : _data(data){}

	  template <typename Table>
      void add(Table table)
      {
        static_assert(_is_dynamic::value, "add must not be called for static using()");
        static_assert(is_table_t<Table>::value, "invalid table argument in add()");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Table>;
        _serialize_check::_();

        using ok = logic::all_t<_is_dynamic::value, is_table_t<Table>::value, _serialize_check::type::value>;

        _add_impl(table, ok());  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename Table>
      void _add_impl(Table table, const std::true_type&)
      {
        return _data._dynamic_tables.emplace_back(table);
      }

      template <typename Table>
      void _add_impl(Table table, const std::false_type&);

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = using_data_t<Database, Tables...>;

// workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
	  template<typename ...Args>
	  _base_t(Args&& ...args) : using_{std::forward<Args>(args)...} {}

      _impl_t<Policies> using_;
      _impl_t<Policies>& operator()()
      {
        return using_;
      }
      const _impl_t<Policies>& operator()() const
      {
        return using_;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.using_)
      {
        return t.using_;
      }

      // FIXME: Maybe check for unused tables, similar to from
      using _consistency_check = consistent_t;
    };
  };

  // NO USING YET
  struct no_using_t
  {
    using _traits = make_traits<no_value_t, tag::is_where>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
// workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
	  _impl_t() = default;
	  _impl_t(const _data_t &data) : _data(data){}

	  _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = no_data_t;

// workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
	  template<typename ...Args>
	  _base_t(Args&& ...args) : no_using{std::forward<Args>(args)...} {}

      _impl_t<Policies> no_using;
      _impl_t<Policies>& operator()()
      {
        return no_using;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_using;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_using)
      {
        return t.no_using;
      }

      using _database_t = typename Policies::_database_t;

// workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
//	  template <typename... T>
//	  using _check = logic::all_t<is_table_t<T>::value...>;
      template <typename... T>
	  struct _check : logic::all_t<is_table_t<T>::value...> {};

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check::value, Policies, no_using_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Args>
      auto using_(Args... args) const -> _new_statement_t<_check<Args...>, using_t<void, Args...>>
      {
        static_assert(not detail::has_duplicates<Args...>::value,
                      "at least one duplicate argument detected in using()");
        static_assert(sizeof...(Args), "at least one table required in using()");
        static_assert(_check<Args...>::value, "at least one argument is not an table in using()");

        return {_using_impl<void>(_check<Args...>{}, args...)};
      }

      template <typename... Args>
      auto dynamic_using(Args... args) const -> _new_statement_t<_check<Args...>, using_t<_database_t, Args...>>
      {
        static_assert(not std::is_same<_database_t, void>::value,
                      "dynamic_using must not be called in a static statement");
        static_assert(_check<Args...>::value, "at least one argument is not an table in using()");

        return {_using_impl<_database_t>(_check<Args...>{}, args...)};
      }

    private:
      template <typename Database, typename... Args>
      auto _using_impl(const std::false_type&, Args... args) const -> bad_statement;

      template <typename Database, typename... Args>
      auto _using_impl(const std::true_type&, Args... args) const
          -> _new_statement_t<std::true_type, using_t<_database_t, Args...>>
      {
        static_assert(not detail::has_duplicates<Args...>::value,
                      "at least one duplicate argument detected in using()");

        return {static_cast<const derived_statement_t<Policies>&>(*this), using_data_t<Database, Args...>{args...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename... Tables>
  struct serializer_t<Context, using_data_t<Database, Tables...>>
  {
    using _serialize_check = serialize_check_of<Context, Tables...>;
    using T = using_data_t<Database, Tables...>;

    static Context& _(const T& t, Context& context)
    {
      if (sizeof...(Tables) == 0 and t._dynamic_tables.empty())
        return context;
      context << " USING ";
      interpret_tuple(t._tables, ',', context);
      if (sizeof...(Tables) and not t._dynamic_tables.empty())
        context << ',';
      interpret_list(t._dynamic_tables, ',', context);
      return context;
    }
  };
}

#endif
