/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
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

#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/MockDb.h>
#include <sqlpp23/tests/core/tables.h>

namespace {
template <typename T, typename V>
using is_same_type = std::is_same<sqlpp::value_type_of_t<T>, V>;

template <typename T, typename V>
using is_select_column_same_type =
    std::is_same<sqlpp::select_column_value_type_of_t<T>, V>;

SQLPP_CREATE_NAME_TAG(always);
SQLPP_CREATE_NAME_TAG(sometimes);
SQLPP_CREATE_NAME_TAG(something);
SQLPP_CREATE_NAME_TAG(table);
SQLPP_CREATE_NAME_TAG(foo);
} // namespace

template <typename Value> void test_select_as(Value v) {
  auto v_not_null = sqlpp::value(v).as(always);
  auto v_maybe_null = sqlpp::value(std::make_optional(v)).as(sometimes);

  using ValueType = sqlpp::value_type_of_t<Value>;
  using OptValueType = sqlpp::value_type_of_t<std::optional<Value>>;

  // SINGLE VALUE, NOT NULL
  {
    auto s = select(v_not_null);
    // A select of a single value can be used as a value.
    static_assert(is_same_type<decltype(s), ValueType>(), "");

    // A select of a single value has a value but no name.
    static_assert(not sqlpp::has_name_tag<decltype(s)>::value, "");
    static_assert(is_same_type<decltype(value(s)), ValueType>(), "");

    // A select of a single value can be named and used as a pseudo table
    static_assert(sqlpp::has_name_tag<decltype(s.as(something))>::value, "");
    static_assert(sqlpp::is_table<decltype(s.as(something))>::value, "");

    // A named select of a single value has no value.
    static_assert(not sqlpp::has_value_type<decltype(s.as(something))>::value,
                  "");

    // The column of a single-value pseudo table can be used as named value
    static_assert(sqlpp::is_column<decltype(s.as(something).always)>::value,
                  "");
    static_assert(sqlpp::has_name_tag<decltype(s.as(something).always)>::value,
                  "");
    static_assert(is_same_type<decltype(s.as(something).always), ValueType>(),
                  "");

    static_assert(
        sqlpp::has_name_tag<decltype(s.as(something).always.as(foo))>::value,
        "");
    static_assert(sqlpp::select_column_has_name<
                      decltype(s.as(something).always.as(foo))>::value,
                  "");
    static_assert(
        is_select_column_same_type<decltype(s.as(something).always.as(foo)),
                                   ValueType>(),
        "");
  }

  // SINGLE VALUE, MAYBE NULL
  {
    auto s = select(v_maybe_null);

    // The column of a single-value pseudo table can be renamed and used as
    // named value A select of a single value can be used as a value.
    static_assert(is_same_type<decltype(s), OptValueType>(), "");

    // A select of a single value has a value but no name.
    static_assert(not sqlpp::has_name_tag<decltype(s)>::value, "");
    static_assert(is_same_type<decltype(value(s)), OptValueType>(), "");

    // A select of a single value can be named and used as a pseudo table
    static_assert(sqlpp::has_name_tag<decltype(s.as(something))>::value, "");
    static_assert(sqlpp::is_table<decltype(s.as(something))>::value, "");

    // A named select of a single value has no value.
    static_assert(not sqlpp::has_value_type<decltype(s.as(something))>::value,
                  "");

    // The column of a single-value pseudo table can be used as named value
    static_assert(sqlpp::is_column<decltype(s.as(something).sometimes)>::value,
                  "");
    static_assert(
        sqlpp::has_name_tag<decltype(s.as(something).sometimes)>::value, "");
    static_assert(
        is_same_type<decltype(s.as(something).sometimes), OptValueType>(), "");

    // The column of a single-value pseudo table can be renamed and used as
    // named value
    static_assert(
        sqlpp::has_name_tag<decltype(s.as(something).sometimes.as(foo))>::value,
        "");
    static_assert(sqlpp::select_column_has_name<
                      decltype(s.as(something).sometimes.as(foo))>::value,
                  "");
    static_assert(
        is_select_column_same_type<decltype(s.as(something).sometimes.as(foo)),
                                   OptValueType>(),
        "");
  }

  // SINGLE PARAMETER, NOT NULL
  {
    auto p = parameter(sqlpp::value_type_of_t<Value>{}, always);
    auto s = select(p.as(always));

    using P = decltype(p);
    using S = decltype(s);

    // Parameters are exposed by select_as.
    static_assert(std::is_same<sqlpp::nodes_of_t<decltype(s.as(something))>,
                               sqlpp::detail::type_vector<S>>::value,
                  "");
    static_assert(
        std::is_same<sqlpp::parameters_of_t<decltype(s.as(something))>,
                     sqlpp::detail::type_vector<P>>::value,
        "");

    // A select of a single value can be used as a value.
    static_assert(is_same_type<decltype(s), ValueType>(), "");

    // A select of a single value has a value but no name.
    static_assert(not sqlpp::has_name_tag<decltype(s)>::value, "");
    static_assert(is_same_type<decltype(value(s)), ValueType>(), "");

    // A select of a single value can be named and used as a pseudo table
    static_assert(sqlpp::has_name_tag<decltype(s.as(something))>::value, "");
    static_assert(sqlpp::is_table<decltype(s.as(something))>::value, "");

    // The column of a single-value pseudo table can be used as named value
    static_assert(sqlpp::is_column<decltype(s.as(something).always)>::value,
                  "");
    static_assert(sqlpp::has_name_tag<decltype(s.as(something).always)>::value,
                  "");
    static_assert(is_same_type<decltype(s.as(something).always), ValueType>(),
                  "");

    // The column of a single-value pseudo table can be renamed and used as
    // named value
    static_assert(
        sqlpp::has_name_tag<decltype(s.as(something).always.as(foo))>::value,
        "");
    static_assert(sqlpp::select_column_has_name<
                      decltype(s.as(something).always.as(foo))>::value,
                  "");
    static_assert(
        is_select_column_same_type<decltype(s.as(something).always.as(foo)),
                                   ValueType>(),
        "");
  }

  // MULTIPLE VALUES
  {
    auto s = select(v_not_null, v_maybe_null);

    // A select of multiple values can not be used as a value.
    static_assert(not sqlpp::has_value_type<decltype(s)>::value, "");

    // A select of multiple values can be named and used as a named value.
    static_assert(sqlpp::has_name_tag<decltype(s.as(something))>::value, "");
    static_assert(not sqlpp::has_value_type<decltype(s.as(something))>::value,
                  "");

    // A select of multiple values can be named and used as a pseudo table
    static_assert(sqlpp::is_table<decltype(s.as(table))>::value, "");

    // The column of a multi-value pseudo table can be used as named value
    static_assert(sqlpp::is_column<decltype(s.as(table).always)>::value, "");
    static_assert(sqlpp::is_column<decltype(s.as(table).sometimes)>::value, "");

    static_assert(sqlpp::has_name_tag<decltype(s.as(table).always)>::value, "");
    static_assert(sqlpp::has_name_tag<decltype(s.as(table).sometimes)>::value,
                  "");

    static_assert(is_same_type<decltype(s.as(table).always), ValueType>(), "");
    static_assert(is_same_type<decltype(s.as(table).sometimes), OptValueType>(),
                  "");

    // The column of a multi-value pseudo table can be renamed and used as named
    // value
    static_assert(
        sqlpp::has_name_tag<decltype(s.as(table).always.as(foo))>::value, "");
    static_assert(
        sqlpp::has_name_tag<decltype(s.as(table).sometimes.as(foo))>::value,
        "");

    static_assert(sqlpp::select_column_has_name<decltype(s.as(table).always.as(
                      foo))>::value,
                  "");
    static_assert(sqlpp::select_column_has_name<
                      decltype(s.as(table).sometimes.as(foo))>::value,
                  "");

    static_assert(
        is_select_column_same_type<decltype(s.as(table).always.as(foo)),
                                   ValueType>(),
        "");
    static_assert(
        is_select_column_same_type<decltype(s.as(table).sometimes.as(foo)),
                                   OptValueType>(),
        "");
  }
}

int main() {
  // boolean
  test_select_as(bool{true});

  // integral
  test_select_as(int{7});

  // unsigned integral
  test_select_as(unsigned{7});

  // floating point
  test_select_as(float{7.7});

  // text
  test_select_as("seven");

  // blob
  test_select_as(std::vector<uint8_t>{});

  // date
  test_select_as(::sqlpp::chrono::day_point{});

  // timestamp
  test_select_as(::sqlpp::chrono::microsecond_point{});
  using minute_point =
      std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>;
  test_select_as(minute_point{});

  // time_of_day
  test_select_as(std::chrono::microseconds{});
}
