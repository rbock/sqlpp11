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

#ifndef SQLPP_FLOATING_POINT_RESULT_FIELD_H
#define SQLPP_FLOATING_POINT_RESULT_FIELD_H

#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/result_field.h>
#include <sqlpp11/result_field_methods.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/data_types/floating_point/data_type.h>
#include <ostream>

namespace sqlpp
{
  template <typename Db, typename FieldSpec>
  struct result_field_t<floating_point, Db, FieldSpec>
      : public result_field_methods_t<result_field_t<floating_point, Db, FieldSpec>>
  {
    static_assert(std::is_same<value_type_of<FieldSpec>, floating_point>::value, "field type mismatch");
    using _cpp_value_type = typename floating_point::_cpp_value_type;

    result_field_t() : _is_valid(false), _is_null(true), _value(0)
    {
    }

    void _validate()
    {
      _is_valid = true;
    }

    void _invalidate()
    {
      _is_valid = false;
      _is_null = true;
      _value = 0;
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

      return value() == 0;
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
          return 0;
        }
      }
      return _value;
    }

    template <typename Target>
    void _bind(Target& target, size_t i)
    {
      target._bind_floating_point_result(i, &_value, &_is_null);
    }

  private:
    bool _is_valid;
    bool _is_null;
    _cpp_value_type _value;
  };

  template <typename Db, typename FieldSpec>
  inline std::ostream& operator<<(std::ostream& os, const result_field_t<floating_point, Db, FieldSpec>& e)
  {
    return serialize(e, os);
  }
}
#endif
