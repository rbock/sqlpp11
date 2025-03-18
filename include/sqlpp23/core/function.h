#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/core/aggregate_function.h>
#include <sqlpp23/core/basic/parameter.h>
#include <sqlpp23/core/basic/parameterized_verbatim.h>
#include <sqlpp23/core/basic/value.h>
#include <sqlpp23/core/basic/verbatim.h>
#include <sqlpp23/core/basic/verbatim_table.h>
#include <sqlpp23/core/database/parameter_list.h>
#include <sqlpp23/core/function/concat.h>
#include <sqlpp23/core/function/lower.h>
#include <sqlpp23/core/function/trim.h>
#include <sqlpp23/core/function/upper.h>
#include <sqlpp23/core/operator/case.h>
#include <sqlpp23/core/query/dynamic.h>
#include <sqlpp23/core/value_type.h>

namespace sqlpp {
template <typename Context, typename Expression>
auto flatten(Context& context, const Expression& exp)
    -> verbatim_t<value_type_of_t<Expression>> {
  static_assert(parameters_of_t<Expression>::empty(),
                "parameters are not allowed in flattened expressions");
  return {to_sql_string(context, exp)};
}

template <typename T>
constexpr const char* get_sql_name(const T& /*unused*/) {
  return name_tag_of_t<T>::name;
}

template <typename T>
constexpr const char* get_sql_name() {
  return name_tag_of_t<T>::name;
}
}  // namespace sqlpp
