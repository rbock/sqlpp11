/*
 * Copyright (c) 2015-2015, Roland Bock
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

#ifndef SQLPP_DATE_H
#define SQLPP_DATE_H

#include <sqlpp11/date_fwd.h>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/value_type.h>
#include <sqlpp11/assignment.h>
#include <sqlpp11/result_field.h>

namespace sqlpp
{
  // date value type
  struct date
  {
    using _traits = make_traits<date, tag::is_value_type>;
    using _tag = tag::is_date;
    using _cpp_value_type = day_point;

    template <typename T>
    using _is_valid_operand = is_date_t<T>;
  };

  // date parameter value
  template <>
  struct parameter_value_t<date>
  {
    using _value_type = date;
    using _cpp_value_type = typename _value_type::_cpp_value_type;

    parameter_value_t() : _value{}, _is_null(true)
    {
    }

    explicit parameter_value_t(const _cpp_value_type& val) : _value(val), _is_null(false)
    {
    }

    parameter_value_t& operator=(const _cpp_value_type& val)
    {
      _value = val;
      _is_null = false;
      return *this;
    }

    parameter_value_t& operator=(const tvin_t<wrap_operand_t<_cpp_value_type>>& t)
    {
      if (t._is_trivial())
      {
        _value = day_point{};
        _is_null = true;
      }
      else
      {
        _value = t._value._t;
        _is_null = false;
      }
      return *this;
    }

    void set_null()
    {
      _value = day_point{};
      _is_null = true;
    }

    bool is_null() const
    {
      return _is_null;
    }

    const _cpp_value_type& value() const
    {
      return _value;
    }

    operator _cpp_value_type() const
    {
      return _value;
    }

    template <typename Target>
    void _bind(Target& target, size_t index) const
    {
      target._bind_date_parameter(index, &_value, _is_null);
    }

  private:
    _cpp_value_type _value;
    bool _is_null;
  };

  // date expression operators
  template <typename Base>
  struct expression_operators<Base, date> : public basic_expression_operators<Base, date>
  {
    template <typename T>
    using _is_valid_operand = is_valid_operand<date, T>;
  };

  // date column operators
  template <typename Base>
  struct column_operators<Base, date>
  {
    template <typename T>
    using _is_valid_operand = is_valid_operand<date, T>;

    template <typename T>
    auto operator+=(T t) const -> assignment_t<Base, plus_t<Base, value_type_t<T>, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Base*>(this), {{*static_cast<const Base*>(this), rhs{t}}}};
    }

    template <typename T>
    auto operator-=(T t) const -> assignment_t<Base, minus_t<Base, value_type_t<T>, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Base*>(this), {{*static_cast<const Base*>(this), rhs{t}}}};
    }

    template <typename T>
    auto operator/=(T t) const -> assignment_t<Base, divides_t<Base, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Base*>(this), {{*static_cast<const Base*>(this), rhs{t}}}};
    }

    template <typename T>
    auto operator*=(T t) const -> assignment_t<Base, multiplies_t<Base, value_type_t<T>, wrap_operand_t<T>>>
    {
      using rhs = wrap_operand_t<T>;
      static_assert(_is_valid_operand<rhs>::value, "invalid rhs assignment operand");

      return {*static_cast<const Base*>(this), {{*static_cast<const Base*>(this), rhs{t}}}};
    }
  };

  // date result field
  template <typename Db, typename FieldSpec>
  struct result_field_t<date, Db, FieldSpec> : public result_field_methods_t<result_field_t<date, Db, FieldSpec>>
  {
    static_assert(std::is_same<value_type_of<FieldSpec>, date>::value, "field type mismatch");
    using _cpp_value_type = typename date::_cpp_value_type;

    result_field_t() : _is_valid(false), _is_null(true), _value{}
    {
    }

    void _invalidate()
    {
      _is_valid = false;
      _is_null = true;
      _value = day_point{};
    }

    void _validate()
    {
      _is_valid = true;
    }

    bool is_null() const
    {
      if (not _is_valid)
        throw exception("accessing is_null in non-existing row");
      return _is_null;
    }

    bool _is_trivial() const
    {
      if (not _is_valid)
        throw exception("accessing is_null in non-existing row");

      return value() == day_point{};
    }

    _cpp_value_type value() const
    {
      if (not _is_valid)
        throw exception("accessing value in non-existing row");

      if (_is_null)
      {
        if (enforce_null_result_treatment_t<Db>::value and not null_is_trivial_value_t<FieldSpec>::value)
        {
          throw exception("accessing value of NULL field");
        }
        else
        {
          return day_point{};
        }
      }
      return _value;
    }

    template <typename Target>
    void _bind(Target& target, size_t i)
    {
      target._bind_date_result(i, &_value, &_is_null);
    }

  private:
    bool _is_valid;
    bool _is_null;
    _cpp_value_type _value;
  };

  // ostream operator for date result field
  template <typename Db, typename FieldSpec>
  inline std::ostream& operator<<(std::ostream& os, const result_field_t<date, Db, FieldSpec>& e)
  {
    return serialize(e, os);
  }
}
#endif
