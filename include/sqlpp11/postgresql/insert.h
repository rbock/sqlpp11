#pragma once

/**
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

#include <sqlpp11/insert.h>
#include <sqlpp11/postgresql/on_conflict.h>
#include <sqlpp11/postgresql/returning.h>

namespace sqlpp
{
  namespace postgresql
  {
    template <typename Database>
    using blank_insert_t =
        statement_t<Database, insert_t, no_into_t, no_insert_value_list_t, no_on_conflict_t, no_returning_t>;

    inline auto insert() -> blank_insert_t<void>
    {
      return {blank_insert_t<void>()};
    }

    template <typename Table>
    constexpr auto insert_into(Table table) -> decltype(blank_insert_t<void>().into(table))
    {
      return {blank_insert_t<void>().into(table)};
    }

    template <typename Database>
    constexpr auto dynamic_insert(const Database&) -> decltype(blank_insert_t<Database>())
    {
      static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
      return {blank_insert_t<Database>()};
    }

    template <typename Database, typename Table>
    constexpr auto dynamic_insert_into(const Database&, Table table) -> decltype(blank_insert_t<Database>().into(table))
    {
      static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
      return {blank_insert_t<Database>().into(table)};
    }
  }  // namespace postgresql
}  // namespace sqlpp
