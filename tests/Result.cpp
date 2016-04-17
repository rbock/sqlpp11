/*
 * Copyright (c) 2013-2015, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include "Sample.h"
#include "MockDb.h"
#include <sqlpp11/sqlpp11.h>

static_assert(not sqlpp::enforce_null_result_treatment_t<MockDb>::value, "MockDb interprets NULL as trivial");
static_assert(sqlpp::enforce_null_result_treatment_t<EnforceDb>::value, "MockDb does not interpret NULL as trivial");

int Result(int, char* [])
{
  MockDb db = {};
  EnforceDb edb{};

  const auto t = test::TabBar{};

  static_assert(sqlpp::can_be_null_t<decltype(t.alpha)>::value, "t.alpha can be null");
  static_assert(not sqlpp::null_is_trivial_value_t<decltype(t.alpha)>::value, "t.alpha does not say null_is_trivial");

  // Using a non-enforcing db
  for (const auto& row : db(select(all_of(t), t.beta.like("")).from(t).unconditionally()))
  {
    static_assert(sqlpp::can_be_null_t<decltype(row.alpha)>::value, "row.alpha can be null");
    static_assert(sqlpp::null_is_trivial_value_t<decltype(row.alpha)>::value, "row.alpha interprets null_is_trivial");
    static_assert(std::is_same<bool, decltype(row.alpha.is_null())>::value, "Yikes");
    using T = sqlpp::wrap_operand_t<decltype(row.alpha)>;
    static_assert(sqlpp::can_be_null_t<T>::value, "row.alpha can be null");
    static_assert(sqlpp::is_result_field_t<T>::value, "result_fields are not wrapped");

    bool x = (t.alpha == row.alpha)._rhs._is_null();
    bool y = (t.alpha == row.alpha)._rhs._is_default();
    std::cerr << x << std::endl;
    std::cerr << y << std::endl;

    for (const auto& sub : db(select(all_of(t)).from(t).where(t.alpha == row.alpha)))
    {
      std::cerr << sub.alpha << std::endl;
    }
    db(insert_into(t).set(t.beta = row.beta, t.gamma = false));
  }

  sqlpp::select((t.alpha + 1).as(t.alpha)).flags(sqlpp::all).from(t);
  for (const auto& row : db(select(all_of(t)).from(t).unconditionally()))
  {
    static_assert(sqlpp::can_be_null_t<decltype(row.alpha)>::value, "row.alpha can be null");
    static_assert(sqlpp::null_is_trivial_value_t<decltype(row.alpha)>::value, "row.alpha interprets null_is_trivial");
  }

  // Using a non-enforcing db
  for (const auto& row : edb(select(all_of(t)).from(t).unconditionally()))
  {
    static_assert(sqlpp::can_be_null_t<decltype(row.alpha)>::value, "row.alpha can be null");
    static_assert(not sqlpp::null_is_trivial_value_t<decltype(row.alpha)>::value,
                  "row.alpha interprets null_is_trivial");
  }

  sqlpp::select((t.alpha + 1).as(t.alpha)).flags(sqlpp::all).from(t);
  for (const auto& row : edb(select(all_of(t)).from(t).unconditionally()))
  {
    static_assert(sqlpp::can_be_null_t<decltype(row.alpha)>::value, "row.alpha can be null");
    static_assert(not sqlpp::null_is_trivial_value_t<decltype(row.alpha)>::value,
                  "row.alpha interprets null_is_trivial");
  }

  return 0;
}
