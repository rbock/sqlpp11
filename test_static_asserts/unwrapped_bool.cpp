/*
 * Copyright (c) 2015-2016, Roland Bock
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

#include "MockDb.h"
#include "Sample.h"
#include <iostream>
#include <sqlpp11/sqlpp11.h>

namespace
{
  constexpr auto t = test::TabBar{};

  template <typename T>
  struct wrap
  {
  };

  template <typename T>
  void print_type_on_error(std::true_type)
  {
  }

  template <typename T>
  void print_type_on_error(std::false_type)
  {
    wrap<T>::_print_me_;
  }

  template <typename Assert, typename Lhs, typename Rhs>
  void and_check(const Lhs& lhs, const Rhs& rhs)
  {
    using ReturnType = decltype(lhs and rhs);
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, sqlpp::bad_expression<sqlpp::boolean>>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Lhs, typename Rhs>
  void or_check(const Lhs& lhs, const Rhs& rhs)
  {
    using ReturnType = decltype(lhs or rhs);
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, sqlpp::bad_expression<sqlpp::boolean>>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Lhs>
  void not_check(const Lhs& lhs)
  {
    using ReturnType = decltype(not lhs);
    using ExpectedReturnType =
        sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, sqlpp::bad_expression<sqlpp::boolean>>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  template <typename Assert, typename Condition>
  void where_check(const Condition& condition)
  {
    using ReturnType = decltype(sqlpp::where(condition));
    using ExpectedReturnType = sqlpp::logic::all_t<Assert::value xor std::is_same<ReturnType, Assert>::value>;
    print_type_on_error<ReturnType>(ExpectedReturnType{});
    static_assert(ExpectedReturnType::value, "Unexpected return type");
  }

  void boolean()
  {
    and_check<sqlpp::consistent_t>(t.gamma, t.gamma);
    and_check<sqlpp::assert_valid_operands>(t.gamma, true);
    // and_check<sqlpp::assert_valid_operands>(true, t.gamma); // Cannot currently do that

    or_check<sqlpp::consistent_t>(t.gamma, t.gamma);
    or_check<sqlpp::assert_valid_operands>(t.gamma, true);
    // or_check<sqlpp::assert_valid_operands>(true, t.gamma); // Cannot currently do that

    not_check<sqlpp::consistent_t>(t.gamma);
  }

  void where()
  {
    where_check<sqlpp::consistent_t>(t.gamma);
    where_check<sqlpp::assert_where_arg_is_not_cpp_bool_t>(true);
  }
}

int main(int, char* [])
{
  boolean();
  where();
}
