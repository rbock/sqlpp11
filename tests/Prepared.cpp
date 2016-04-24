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

#include "Sample.h"
#include "MockDb.h"
#include "is_regular.h"
#include <sqlpp11/functions.h>
#include <sqlpp11/select.h>

int Prepared(int, char* [])
{
  MockDb db = {};
  // test::TabFoo f;
  const auto t = test::TabBar{};

  // empty parameter lists
  {
    using P = sqlpp::make_parameter_list_t<decltype(t.alpha)>;
    static_assert(P::size::value == 0, "type requirement");
  }

  // single parameter
  {
    using P = sqlpp::make_parameter_list_t<decltype(parameter(t.alpha))>;
    static_assert(P::size::value == 1, "type requirement");
    auto p = P{};
    p.alpha = 7;
  }

  // single parameter
  {
    using P = sqlpp::make_parameter_list_t<decltype(parameter(t.beta))>;
    static_assert(P::size::value == 1, "type requirement");
    auto p = P{};
    p.beta = "cheesecake";
  }

  // single parameter in expression
  {
    using P = sqlpp::make_parameter_list_t<decltype(t.alpha == parameter(t.alpha))>;
    static_assert(P::size::value == 1, "type requirement");
    auto p = P{};
    p.alpha = 7;
  }

  // single parameter in larger expression
  {
    using P = sqlpp::make_parameter_list_t<decltype((t.beta.like("%") and t.alpha == parameter(t.alpha)) or
                                                    t.gamma != false)>;
    static_assert(P::size::value == 1, "type requirement");
    auto p = P{};
    p.alpha = 7;
  }

  // three parameters in expression
  {
    using P = sqlpp::parameters_of<decltype((t.beta.like(parameter(t.beta)) and t.alpha == parameter(t.alpha)) or
                                            t.gamma != parameter(t.gamma))>;
    // FIXME: make some test, that does not depend on detail namespace, but still checks the correct order of the
    // parameters
    static_assert(std::is_same<P, sqlpp::detail::type_vector<decltype(parameter(t.beta)), decltype(parameter(t.alpha)),
                                                             decltype(parameter(t.gamma))>>::value,
                  "type requirement");
  }

  // OK, fine, now create a named parameter list from an expression
  {
    using Exp =
        decltype((t.beta.like(parameter(t.beta)) and t.alpha == parameter(t.alpha)) or t.gamma != parameter(t.gamma));
    using P = sqlpp::make_parameter_list_t<Exp>;
    P npl;
    static_assert(
        std::is_same<sqlpp::parameter_value_t<sqlpp::value_type_of<decltype(t.alpha)>>, decltype(npl.alpha)>::value,
        "type requirement");
    static_assert(
        std::is_same<sqlpp::parameter_value_t<sqlpp::value_type_of<decltype(t.beta)>>, decltype(npl.beta)>::value,
        "type requirement");
    static_assert(
        std::is_same<sqlpp::parameter_value_t<sqlpp::value_type_of<decltype(t.gamma)>>, decltype(npl.gamma)>::value,
        "type requirement");
  }

  // Wonderful, now take a look at the parameter list of a select
  {
    auto s = select(all_of(t)).from(t).where((t.beta.like(parameter(t.beta)) and t.alpha == parameter(t.alpha)) or
                                             t.gamma != parameter(t.gamma));
    auto p = db.prepare(s);
    p.params.alpha = 7;
    p.params.alpha = sqlpp::tvin(0);
    using S = decltype(s);
    using P = sqlpp::make_parameter_list_t<S>;
    P npl;

    static_assert(
        std::is_same<sqlpp::parameter_value_t<sqlpp::value_type_of<decltype(t.alpha)>>, decltype(npl.alpha)>::value,
        "type requirement");
    static_assert(
        std::is_same<sqlpp::parameter_value_t<sqlpp::value_type_of<decltype(t.beta)>>, decltype(npl.beta)>::value,
        "type requirement");
    static_assert(
        std::is_same<sqlpp::parameter_value_t<sqlpp::value_type_of<decltype(t.gamma)>>, decltype(npl.gamma)>::value,
        "type requirement");
    npl.alpha = 7;
    auto x = npl;
    x = npl;
    std::cerr << x.alpha << std::endl;
    x = decltype(npl)();
    std::cerr << x.alpha << std::endl;
  }

  // Can we prepare a query without parameters?
  {
    auto ps = db.prepare(select(all_of(t)).from(t).where((t.beta.like("%"))));
    auto res = db(ps);
  }

  // Check that a prepared select is default-constructable
  {
    auto s = select(all_of(t)).from(t).where((t.beta.like(parameter(t.beta)) and t.alpha == parameter(t.alpha)) or
                                             t.gamma != parameter(t.gamma));
    using P = decltype(db.prepare(s));
    P p;
  }

  return 0;
}
