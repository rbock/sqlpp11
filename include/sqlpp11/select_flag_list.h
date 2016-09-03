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

#ifndef SQLPP_SELECT_FLAG_LIST_H
#define SQLPP_SELECT_FLAG_LIST_H

#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/no_data.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/type_traits.h>
#include <tuple>

namespace sqlpp
{
  // SELECTED FLAGS DATA
  template <typename Database, typename... Flags>
  struct select_flag_list_data_t
  {
    select_flag_list_data_t(Flags... flgs) : _flags(flgs...)
    {
    }

    select_flag_list_data_t(const select_flag_list_data_t&) = default;
    select_flag_list_data_t(select_flag_list_data_t&&) = default;
    select_flag_list_data_t& operator=(const select_flag_list_data_t&) = default;
    select_flag_list_data_t& operator=(select_flag_list_data_t&&) = default;
    ~select_flag_list_data_t() = default;

    std::tuple<Flags...> _flags;
    interpretable_list_t<Database> _dynamic_flags;
  };

  // SELECT FLAGS
  template <typename Database, typename... Flags>
  struct select_flag_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_select_flag_list>;
    using _nodes = detail::type_vector<Flags...>;

    using _is_dynamic = is_database<Database>;

    // Data
    using _data_t = select_flag_list_data_t<Database, Flags...>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      template <typename Flag>
      void add(Flag flag)
      {
        static_assert(_is_dynamic::value, "select_flags::add() must not be called for static select flags");
        static_assert(is_select_flag_t<Flag>::value, "invalid select flag argument in select_flags::add()");
        static_assert(Policies::template _no_unknown_tables<Flag>::value,
                      "flag uses tables unknown to this statement in select_flags::add()");
        using _serialize_check = sqlpp::serialize_check_t<typename Database::_serializer_context_t, Flag>;
        _serialize_check{};

        using ok = logic::all_t<_is_dynamic::value, is_select_flag_t<Flag>::value, _serialize_check::type::value>;

        _add_impl(flag, ok());  // dispatch to prevent compile messages after the static_assert
      }

    private:
      template <typename Flag>
      void _add_impl(Flag flag, const std::true_type&)
      {
        return _data._dynamic_flags.emplace_back(flag);
      }

      template <typename Flag>
      void _add_impl(Flag flag, const std::false_type&);

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = select_flag_list_data_t<Database, Flags...>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : select_flags{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> select_flags;
      _impl_t<Policies>& operator()()
      {
        return select_flags;
      }
      const _impl_t<Policies>& operator()() const
      {
        return select_flags;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.select_flags)
      {
        return t.select_flags;
      }

      using _consistency_check = consistent_t;
    };
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_select_flags_are_flags_t, "arguments for flags() must be known select flags");
  template <typename... Flags>
  struct check_select_flags
  {
    using type = static_combined_check_t<
        static_check_t<logic::all_t<is_select_flag_t<Flags>::value...>::value, assert_select_flags_are_flags_t>>;
  };
  template <typename... Flags>
  using check_select_flags_t = typename check_select_flags<Flags...>::type;

  struct no_select_flag_list_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

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
      using _data_t = no_data_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2173269
      template <typename... Args>
      _base_t(Args&&... args) : no_select_flags{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_select_flags;
      _impl_t<Policies>& operator()()
      {
        return no_select_flags;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_select_flags;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_select_flags)
      {
        return t.no_select_flags;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_select_flag_list_t, T>;

      using _consistency_check = consistent_t;

      template <typename... Flags>
      auto flags(Flags... flgs) const
          -> _new_statement_t<check_select_flags_t<Flags...>, select_flag_list_t<void, Flags...>>
      {
        return _flags_impl<void>(check_select_flags_t<Flags...>{}, flgs...);
      }

      template <typename... Flags>
      auto dynamic_flags(Flags... flgs) const
          -> _new_statement_t<check_select_flags_t<Flags...>, select_flag_list_t<_database_t, Flags...>>
      {
        static_assert(not std::is_same<_database_t, void>::value,
                      "dynamic_flags must not be called in a static statement");

        return _flags_impl<_database_t>(check_select_flags_t<Flags...>{}, flgs...);
      }

    private:
      template <typename Database, typename Check, typename... Flags>
      auto _flags_impl(Check, Flags... flgs) const -> inconsistent<Check>;

      template <typename Database, typename... Flags>
      auto _flags_impl(consistent_t, Flags... flgs) const
          -> _new_statement_t<consistent_t, select_flag_list_t<Database, Flags...>>
      {
        static_assert(not detail::has_duplicates<Flags...>::value,
                      "at least one duplicate argument detected in select flag list");

        return {static_cast<const derived_statement_t<Policies>&>(*this),
                select_flag_list_data_t<Database, Flags...>{flgs...}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Database, typename... Flags>
  struct serializer_t<Context, select_flag_list_data_t<Database, Flags...>>
  {
    using _serialize_check = serialize_check_of<Context, Flags...>;
    using T = select_flag_list_data_t<Database, Flags...>;

    static Context& _(const T& t, Context& context)
    {
      interpret_tuple(t._flags, ' ', context);
      if (sizeof...(Flags))
        context << ' ';
      interpret_list(t._dynamic_flags, ',', context);
      if (not t._dynamic_flags.empty())
        context << ' ';
      return context;
    }
  };

  template <typename T>
  auto select_flags(T&& t) -> decltype(statement_t<void, no_select_flag_list_t>().flags(std::forward<T>(t)))
  {
    return statement_t<void, no_select_flag_list_t>().flags(std::forward<T>(t));
  }

  template <typename Database, typename T>
  auto dynamic_select_flags(const Database&, T&& t)
      -> decltype(statement_t<Database, no_select_flag_list_t>().dynamic_flags(std::forward<T>(t)))
  {
    return statement_t<Database, no_select_flag_list_t>().dynamic_flags(std::forward<T>(t));
  }
}

#endif
