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

#ifndef SQLPP_RESULT_FIELD_BASE_H
#define SQLPP_RESULT_FIELD_BASE_H

#include <sqlpp11/alias_operators.h>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/exception.h>
#include <sqlpp11/result_field.h>
#include <sqlpp11/type_traits.h>
#include <utility>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(assert_result_field_value_is_safe_t, "result field value needs to be checked for NULL");

  template <typename Db, typename FieldSpec, typename StorageType = typename value_type_of<FieldSpec>::_cpp_value_type>
  struct result_field_base
  {
    using _db_t = Db;
    using _field_spec_t = FieldSpec;
    using _alias_t = typename FieldSpec::_alias_t;
    using _cpp_value_type = typename value_type_of<FieldSpec>::_cpp_value_type;
    using _cpp_storage_type = StorageType;

    static constexpr bool _null_is_trivial =
        column_spec_can_be_null_t<_field_spec_t>::value and
        (null_is_trivial_value_t<_field_spec_t>::value or not enforce_null_result_treatment_t<_db_t>::value);
    using _traits = make_traits<value_type_of<_field_spec_t>,
                                tag::is_result_field,
                                tag::is_expression,
                                tag_if<tag::null_is_trivial_value, _null_is_trivial>>;

    using _nodes = detail::type_vector<>;
    using _can_be_null = column_spec_can_be_null_t<_field_spec_t>;

    result_field_base() : _is_valid{false}, _is_null{true}, _value{}
    {
    }

    bool operator==(const _cpp_value_type& rhs) const
    {
      return value() == rhs;
    }

    bool operator!=(const _cpp_value_type& rhs) const
    {
      return not operator==(rhs);
    }

    void _validate()
    {
      _is_valid = true;
    }

    void _invalidate()
    {
      _is_valid = false;
      _is_null = true;
      _value = {};
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

      return value() == _cpp_storage_type{};
    }

    _cpp_value_type value() const
    {
      if (not _is_valid)
        throw exception("accessing value in non-existing row");

      if (_is_null)
      {
        if (not _null_is_trivial)
        {
          throw exception("accessing value of NULL field");
        }
        else
        {
          return {};
        }
      }
      return _value;
    }

    operator typename std::conditional<_null_is_trivial or (not _can_be_null::value),
                                       _cpp_value_type,
                                       assert_result_field_value_is_safe_t>::type() const
    {
      return value();
    }

    bool _is_valid;
    bool _is_null;
    _cpp_storage_type _value;
  };
}
#endif
