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

#ifndef SQLPP_OFFSET_H
#define SQLPP_OFFSET_H

#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/policy_update.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  // OFFSET DATA
  template <typename Offset>
  struct offset_data_t
  {
    offset_data_t(Offset value) : _value(value)
    {
    }

    offset_data_t(const offset_data_t&) = default;
    offset_data_t(offset_data_t&&) = default;
    offset_data_t& operator=(const offset_data_t&) = default;
    offset_data_t& operator=(offset_data_t&&) = default;
    ~offset_data_t() = default;

    Offset _value;
  };

  // OFFSET
  template <typename Offset>
  struct offset_t
  {
    using _traits = make_traits<no_value_t, tag::is_offset>;
    using _nodes = detail::type_vector<Offset>;

    // Data
    using _data_t = offset_data_t<Offset>;

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
      using _data_t = offset_data_t<Offset>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : offset{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> offset;
      _impl_t<Policies>& operator()()
      {
        return offset;
      }
      const _impl_t<Policies>& operator()() const
      {
        return offset;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.offset)
      {
        return t.offset;
      }

      using _consistency_check = consistent_t;
    };
  };

  // DYNAMIC OFFSET DATA
  template <typename Database>
  struct dynamic_offset_data_t
  {
    dynamic_offset_data_t() : _value(noop())
    {
    }

    template <typename Offset>
    dynamic_offset_data_t(Offset value) : _initialized(true), _value(wrap_operand_t<Offset>(value))
    {
    }

    dynamic_offset_data_t(const dynamic_offset_data_t&) = default;
    dynamic_offset_data_t(dynamic_offset_data_t&&) = default;
    dynamic_offset_data_t& operator=(const dynamic_offset_data_t&) = default;
    dynamic_offset_data_t& operator=(dynamic_offset_data_t&&) = default;
    ~dynamic_offset_data_t() = default;

    bool _initialized = false;
    interpretable_t<Database> _value;
  };

  // DYNAMIC OFFSET
  template <typename Database>
  struct dynamic_offset_t
  {
    using _traits = make_traits<no_value_t, tag::is_offset>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = dynamic_offset_data_t<Database>;

    // Member implementation with data and methods
    template <typename Policies>
    struct _impl_t
    {
      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      _impl_t() = default;
      _impl_t(const _data_t& data) : _data(data)
      {
      }

      template <typename Offset>
      void set(Offset value)
      {
        // FIXME: Make sure that Offset does not require external tables? Need to read up on SQL
        using arg_t = wrap_operand_t<Offset>;
        static_assert(is_unsigned_integral_t<arg_t>::value,
                      "offset requires an unsigned integral value or unsigned integral parameter");
        _data._value = arg_t{value};
        _data._initialized = true;
      }

    public:
      _data_t _data;
    };

    // Base template to be inherited by the statement
    template <typename Policies>
    struct _base_t
    {
      using _data_t = dynamic_offset_data_t<Database>;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : offset{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> offset;
      _impl_t<Policies>& operator()()
      {
        return offset;
      }
      const _impl_t<Policies>& operator()() const
      {
        return offset;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.offset)
      {
        return t.offset;
      }

      using _consistency_check = consistent_t;

      template <typename Offset>
      void set_offset(Offset value)
      {
        // FIXME: Make sure that Offset does not require external tables? Need to read up on SQL
        using arg_t = wrap_operand_t<Offset>;
        static_cast<derived_statement_t<Policies>*>(this)->_offset()._value = arg_t{value};
        static_cast<derived_statement_t<Policies>*>(this)->_offset()._initialized = true;
      }
    };

    bool _initialized = false;
    interpretable_t<Database> _value;
  };

  SQLPP_PORTABLE_STATIC_ASSERT(assert_offset_is_unsigned_integral,
                               "argument for offset() must be an integral expressions");
  template <typename T>
  struct check_offset
  {
    using type =
        static_combined_check_t<static_check_t<is_unsigned_integral_t<T>::value, assert_offset_is_unsigned_integral>>;
  };
  template <typename T>
  using check_offset_t = typename check_offset<wrap_operand_t<T>>::type;

  struct no_offset_t
  {
    using _traits = make_traits<no_value_t, tag::is_noop>;
    using _nodes = detail::type_vector<>;

    // Data
    using _data_t = no_data_t;

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
      using _data_t = no_data_t;

      // workaround for msvc bug https://connect.microsoft.com/VisualStudio/Feedback/Details/2091069
      template <typename... Args>
      _base_t(Args&&... args) : no_offset{std::forward<Args>(args)...}
      {
      }

      _impl_t<Policies> no_offset;
      _impl_t<Policies>& operator()()
      {
        return no_offset;
      }
      const _impl_t<Policies>& operator()() const
      {
        return no_offset;
      }

      template <typename T>
      static auto _get_member(T t) -> decltype(t.no_offset)
      {
        return t.no_offset;
      }

      using _database_t = typename Policies::_database_t;

      template <typename Check, typename T>
      using _new_statement_t = new_statement_t<Check, Policies, no_offset_t, T>;

      using _consistency_check = consistent_t;

      template <typename Arg>
      auto offset(Arg arg) const -> _new_statement_t<check_offset_t<Arg>, offset_t<wrap_operand_t<Arg>>>
      {
        return _offset_impl(check_offset_t<Arg>{}, wrap_operand_t<Arg>{arg});
      }

      auto dynamic_offset() const -> _new_statement_t<consistent_t, dynamic_offset_t<_database_t>>
      {
        static_assert(not std::is_same<_database_t, void>::value,
                      "dynamic_offset must not be called in a static statement");
        return {static_cast<const derived_statement_t<Policies>&>(*this), dynamic_offset_data_t<_database_t>{}};
      }

    private:
      template <typename Check, typename Arg>
      auto _offset_impl(Check, Arg arg) const -> inconsistent<Check>;

      template <typename Arg>
      auto _offset_impl(consistent_t, Arg arg) const -> _new_statement_t<consistent_t, offset_t<Arg>>
      {
        return {static_cast<const derived_statement_t<Policies>&>(*this), offset_data_t<Arg>{arg}};
      }
    };
  };

  // Interpreters
  template <typename Context, typename Offset>
  struct serializer_t<Context, offset_data_t<Offset>>
  {
    using _serialize_check = serialize_check_of<Context, Offset>;
    using T = offset_data_t<Offset>;

    static Context& _(const T& t, Context& context)
    {
      context << " OFFSET ";
      serialize_operand(t._value, context);
      return context;
    }
  };

  template <typename Context, typename Database>
  struct serializer_t<Context, dynamic_offset_data_t<Database>>
  {
    using _serialize_check = consistent_t;
    using T = dynamic_offset_data_t<Database>;

    static Context& _(const T& t, Context& context)
    {
      if (t._initialized)
      {
        context << " OFFSET ";
        serialize(t._value, context);
      }
      return context;
    }
  };

  template <typename T>
  auto offset(T&& t) -> decltype(statement_t<void, no_offset_t>().offset(std::forward<T>(t)))
  {
    return statement_t<void, no_offset_t>().offset(std::forward<T>(t));
  }

  template <typename Database>
  auto dynamic_offset(const Database&) -> decltype(statement_t<Database, no_offset_t>().dynamic_offset())
  {
    return statement_t<Database, no_offset_t>().dynamic_offset();
  }
}

#endif
