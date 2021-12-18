/*
 * Copyright (c) 2013 - 2016, Roland Bock
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

#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/sqlpp11.h>

#include "IntegralSample.h"
#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <iostream>
#include <limits>

namespace sql = sqlpp::sqlite3;

const auto intSample = IntegralSample{};

template <typename L, typename R>
auto require_equal(int line, const L& l, const R& r) -> void
{
  if (l != r)
  {
    std::cerr << line << ": ";
    serialize(::sqlpp::wrap_operand_t<L>{l}, std::cerr);
    std::cerr << " != ";
    serialize(::sqlpp::wrap_operand_t<R>{r}, std::cerr);
    throw std::runtime_error("Unexpected result");
  }
}

int Integral(int, char*[])
{
  sql::connection_config config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = true;

  sql::connection db(config);
  db.execute(R"(CREATE TABLE integral_sample (
      signed_value INTEGER,
      unsigned_value INTEGER
  ))");

  // The connector supports uint64_t values and will always retrieve the correct value from the database.
  // Sqlite3 stores the values as int64_t internally though, so big uint64_t values will be converted
  // and the library has to intepret the int64_t values correctly as uint64_t.
  // Therefore, we test uint64_t values in an out of the range of int64_t and test if they are retrieved
  // correctly from the database in both cases.
  uint64_t uint64_t_value_supported = std::numeric_limits<int64_t>::max();
  int64_t int64_t_value_max = std::numeric_limits<int64_t>::max();

  uint64_t uint64_t_value_unsupported = std::numeric_limits<uint64_t>::max();
  int64_t int64_t_value_min = std::numeric_limits<int64_t>::min();

  std::size_t size_t_value_max = std::numeric_limits<std::size_t>::max();
  std::size_t size_t_value_min = std::numeric_limits<std::size_t>::min();

  uint32_t uint32_t_value = std::numeric_limits<uint32_t>::max();
  int32_t int32_t_value = std::numeric_limits<int32_t>::max();

  db(insert_into(intSample).set(intSample.signedValue = int64_t_value_max,
                                intSample.unsignedValue = uint64_t_value_supported));

  auto prepared_insert =
      db.prepare(insert_into(intSample).set(intSample.signedValue = parameter(intSample.signedValue),
                                            intSample.unsignedValue = parameter(intSample.unsignedValue)));
  prepared_insert.params.signedValue = int64_t_value_min;
  prepared_insert.params.unsignedValue = uint64_t_value_unsupported;
  db(prepared_insert);

  db(insert_into(intSample).set(intSample.signedValue = size_t_value_min, intSample.unsignedValue = size_t_value_max));
  db(insert_into(intSample).set(intSample.signedValue = int32_t_value, intSample.unsignedValue = uint32_t_value));

  auto q = select(intSample.signedValue, intSample.unsignedValue).from(intSample).unconditionally();

  auto rows = db(q);

  require_equal(__LINE__, rows.front().signedValue.value(), int64_t_value_max);
  require_equal(__LINE__, rows.front().unsignedValue.value(), uint64_t_value_supported);
  rows.pop_front();

  require_equal(__LINE__, rows.front().signedValue.value(), int64_t_value_min);
  require_equal(__LINE__, rows.front().unsignedValue.value(), uint64_t_value_unsupported);
  rows.pop_front();

  require_equal(__LINE__, rows.front().signedValue.value(), int64_t{});
  require_equal(__LINE__, rows.front().unsignedValue.value(), size_t_value_max);
  rows.pop_front();

  require_equal(__LINE__, rows.front().signedValue.value(), int32_t_value);
  require_equal(__LINE__, rows.front().unsignedValue.value(), uint32_t_value);
  rows.pop_front();

  return 0;
}
