/*
 * Copyright (c) 2025, Roland Bock
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
#include <sqlpp23/tests/core/tables.h>

SQLPP_CREATE_NAME_TAG(something);

void test_parameter() {
  const auto foo = test::TabFoo{};

  {
    auto p = parameter(foo.id);
    using P = decltype(p);
    static_assert(
        std::is_same<sqlpp::value_type_of_t<P>, sqlpp::integral>::value, "");
    static_assert(std::is_same<sqlpp::parameters_of_t<P>,
                               sqlpp::detail::type_vector<P>>::value,
                  "");

    // If parameters had a name tag, it would be possible to use them as select
    // columns.
    static_assert(not sqlpp::has_name_tag<P>::value, "");

    static_assert(sqlpp::has_enabled_as<P>::value, "");
    static_assert(sqlpp::has_enabled_comparison<P>::value, "");
  }
  {
    auto p = parameter(foo.doubleN);
    using P = decltype(p);
    static_assert(std::is_same<sqlpp::value_type_of_t<P>,
                               std::optional<sqlpp::floating_point>>::value,
                  "");
    static_assert(std::is_same<sqlpp::parameters_of_t<P>,
                               sqlpp::detail::type_vector<P>>::value,
                  "");

    static_assert(not sqlpp::has_name_tag<P>::value, "");
    static_assert(sqlpp::has_enabled_as<P>::value, "");
    static_assert(sqlpp::has_enabled_comparison<P>::value, "");
  }

  {
    auto p = parameter(sqlpp::integral{}, something);
    using P = decltype(p);
    static_assert(
        std::is_same<sqlpp::value_type_of_t<P>, sqlpp::integral>::value, "");
    static_assert(std::is_same<sqlpp::parameters_of_t<P>,
                               sqlpp::detail::type_vector<P>>::value,
                  "");

    static_assert(not sqlpp::has_name_tag<P>::value, "");
    static_assert(sqlpp::has_enabled_as<P>::value, "");
    static_assert(sqlpp::has_enabled_comparison<P>::value, "");
  }
  {
    auto p = parameter(std::optional<sqlpp::blob>{}, something);
    using P = decltype(p);
    static_assert(std::is_same<sqlpp::value_type_of_t<P>,
                               std::optional<sqlpp::blob>>::value,
                  "");
    static_assert(std::is_same<sqlpp::parameters_of_t<P>,
                               sqlpp::detail::type_vector<P>>::value,
                  "");

    static_assert(not sqlpp::has_name_tag<P>::value, "");
    static_assert(sqlpp::has_enabled_as<P>::value, "");
    static_assert(sqlpp::has_enabled_comparison<P>::value, "");
  }
}

int main() {
  void test_column();
}
