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

#ifndef SQLPP_RESULT_FIELD_METHODS_H
#define SQLPP_RESULT_FIELD_METHODS_H

#include <utility>
#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/alias_operators.h>

namespace sqlpp
{
  template <typename Field>
  struct result_field_methods_t
  {
    static_assert(wrong_t<result_field_methods_t>::value, "Invalid argument for result_field_base");
  };

  template <typename ValueType, typename Db, typename FieldSpec>
  struct result_field_methods_t<result_field_t<ValueType, Db, FieldSpec>>
  {
    using _derived_t = result_field_t<ValueType, Db, FieldSpec>;
    using _field_spec_t = FieldSpec;
    using _db_t = Db;

    static constexpr bool _null_is_trivial =
        column_spec_can_be_null_t<_field_spec_t>::value and
        (null_is_trivial_value_t<_field_spec_t>::value or not enforce_null_result_treatment_t<_db_t>::value);
    using _traits = make_traits<value_type_of<_field_spec_t>,
                                tag::is_result_field,
                                tag::is_expression,
                                tag_if<tag::null_is_trivial_value, _null_is_trivial>>;

    using _nodes = detail::type_vector<>;
    using _can_be_null = column_spec_can_be_null_t<_field_spec_t>;

    operator cpp_value_type_of<_field_spec_t>() const
    {
      return static_cast<const _derived_t&>(*this).value();
    }
  };
}
#endif
