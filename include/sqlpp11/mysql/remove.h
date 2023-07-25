#pragma once

/*
 * Copyright (c) 2021-2021, Roland Bock, ZerQAQ
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

#include <sqlpp11/remove.h>
#include <sqlpp11/order_by.h>
#include <sqlpp11/limit.h>

namespace sqlpp
{
  namespace mysql
  {
    template <typename Database>
    using blank_remove_t =
        statement_t<Database, remove_t, no_from_t, no_using_t, no_where_t<true>, no_order_by_t, no_limit_t>;

    inline auto remove() -> blank_remove_t<void>
    {
      return {};
    }

    template <typename Table>
    auto remove_from(Table table) -> decltype(blank_remove_t<void>().from(table))
    {
      return {blank_remove_t<void>().from(table)};
    }

    template <typename Database>
    auto dynamic_remove(const Database& /*unused*/) -> decltype(blank_remove_t<Database>())
    {
      static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
      return {blank_remove_t<Database>()};
    }

    template <typename Database, typename Table>
    auto dynamic_remove_from(const Database& /*unused*/, Table table)
        -> decltype(blank_remove_t<Database>().from(table))
    {
      static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
      return {blank_remove_t<Database>().from(table)};
    }
  }  // namespace mysql

}  // namespace sqlpp
