/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/tests/core/tables.h>
#include <sqlpp11/sqlpp11.h>

namespace test
{
  SQLPP_CREATE_NAME_TAG(incomplete);
  SQLPP_CREATE_NAME_TAG(basic);
  SQLPP_CREATE_NAME_TAG(referencing);
  SQLPP_CREATE_NAME_TAG(recursive);
  SQLPP_CREATE_NAME_TAG(alias);
}  // namespace

template<typename BlankWith>
struct extract_with;

template<typename... Ctes>
struct extract_with<sqlpp::blank_with_t<Ctes...>>
{
  using type = sqlpp::with_t<Ctes...>;
};

template<typename BlankWith>
using extract_with_t = typename extract_with<BlankWith>::type;

void test_with()
{
#warning: test parameter exposure
  const auto foo = test::TabFoo{};

  // ctes referencing other CTEs require such ctes. `have_correct_dependencies` checks that.
  {
    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).unconditionally());
    using Basic = decltype(basic);

    auto referencing = sqlpp::cte(test::referencing).as(select(basic.id).from(basic).unconditionally());
    using Referencing = decltype(referencing);

    // Simple good cases.
    static_assert(sqlpp::have_correct_dependencies<Basic>::value, "");
    static_assert(sqlpp::have_correct_dependencies<sqlpp::dynamic_t<Basic>>::value, "");
    static_assert(sqlpp::have_correct_dependencies<Basic, Referencing>::value, "");
    static_assert(sqlpp::have_correct_dependencies<Basic, sqlpp::dynamic_t<Referencing>>::value, "");

    // The library has no way of knowing if `Basic` and `Referencing` are dynamically added in the correct combinations
    // (`Basic` has to be present if `Referencing` is added). It has to assume that the library user knows what they are
    // doing.
    static_assert(sqlpp::have_correct_dependencies<sqlpp::dynamic_t<Basic>, sqlpp::dynamic_t<Referencing>>::value, "");

    // `Referencing` requires the cte it references.
    static_assert(not sqlpp::have_correct_dependencies<Referencing>::value, "");
    static_assert(not sqlpp::have_correct_dependencies<sqlpp::dynamic_t<Referencing>>::value, "");

    // `Referencing` has to mentioned after the cte it references.
    static_assert(not sqlpp::have_correct_dependencies<Referencing, Basic>::value, "");
    static_assert(not sqlpp::have_correct_dependencies<sqlpp::dynamic_t<Referencing>, Basic>::value, "");
    static_assert(not sqlpp::have_correct_dependencies<sqlpp::dynamic_t<Referencing>, sqlpp::dynamic_t<Basic>>::value, "");

    // `Referencing` statically requires the cte it references. It is not sufficient to have a dynamic `Basic` cte.
    static_assert(not sqlpp::have_correct_dependencies<sqlpp::dynamic_t<Basic>, Referencing>::value, "");

#warning: Need to add test for recursive CTEs.
   }

  // `with` exposes parameters from it's CTEs
  {
    auto a = sqlpp::parameter(sqlpp::boolean{}, sqlpp::alias::a);
    using A = decltype(a);
    auto b = sqlpp::parameter(sqlpp::boolean{}, sqlpp::alias::b);
    using B = decltype(b);

    auto basic = sqlpp::cte(test::basic).as(select(foo.id).from(foo).where(a));
    auto referencing = sqlpp::cte(test::referencing).as(select(basic.id).from(basic).where(b));

    {
      using W = extract_with_t<decltype(with(basic))>;
      static_assert(std::is_same<sqlpp::parameters_of_t<W>, sqlpp::detail::type_vector<A>>::value, "");
    }
    {
      using W = extract_with_t<decltype(with(basic, referencing))>;
      static_assert(std::is_same<sqlpp::parameters_of_t<W>, sqlpp::detail::type_vector<A, B>>::value, "");
    }
    {
      using W = extract_with_t<decltype(with(dynamic(true, basic), dynamic(false, referencing)))>;
      static_assert(std::is_same<sqlpp::parameters_of_t<W>, sqlpp::detail::type_vector<A, B>>::value, "");
    }
   }

}

int main()
{
  void test_with();
}
