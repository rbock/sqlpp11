#pragma once

/*
 * Copyright (c) 2013 - 2015, Roland Bock
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

#include <sqlpp23/mysql/database/connection.h>
#include <sqlpp23/sqlpp23.h>

namespace sqlpp {
template <typename Lhs, typename Rhs, typename Condition>
auto to_sql_string(mysql::context_t&,
                   const join_t<Lhs, full_outer_join_t, Rhs, Condition>&)
    -> std::string {
  SQLPP_STATIC_ASSERT((wrong_t<Lhs, Rhs>::value),
                      "MySQL: No support for full outer join");
  return {};
}

namespace mysql {
inline auto to_sql_string(mysql::context_t&, const insert_default_values_t&)
    -> std::string {
  return " () VALUES()";
}

inline auto quoted_name_to_sql_string(mysql::context_t&,
                                      const std::string_view& name)
    -> std::string {
  return '`' + std::string(name) + '`';
}
}  // namespace mysql
}  // namespace sqlpp
