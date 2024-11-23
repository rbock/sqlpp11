#pragma once

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

#include <sqlpp11/core/query/dynamic.h>
#include <sqlpp11/core/basic/parameter.h>
#include <sqlpp11/core/database/parameter_list.h>
#include <sqlpp11/core/aggregate_function.h>
#include <sqlpp11/core/function/trim.h>
#include <sqlpp11/core/operator/case.h>
#include <sqlpp11/core/function/lower.h>
#include <sqlpp11/core/function/upper.h>
#include <sqlpp11/core/value_type.h>
#include <sqlpp11/core/basic/verbatim.h>  // Csaba Csoma suggests: unsafe_sql instead of verbatim
#include <sqlpp11/core/basic/parameterized_verbatim.h>
#include <sqlpp11/core/basic/verbatim_table.h>
#include <sqlpp11/core/basic/value.h>
#include <sqlpp11/core/eval.h>

namespace sqlpp
{
  template <typename Expression, typename Db>
  auto flatten(const Expression& exp, Db& db) -> verbatim_t<value_type_of_t<Expression>>
  {
    static_assert(not make_parameter_list_t<Expression>::size::value,
                  "parameters are not allowed in flattened expressions");
    auto context = db.get_serializer_context();
    to_sql_string(context, exp);
    return {context.str()};
  }

  template <typename T>
  constexpr const char* get_sql_name(const T& /*unused*/)
  {
    return name_tag_of_t<T>::_name_t::template char_ptr<void>();
  }
}  // namespace sqlpp
