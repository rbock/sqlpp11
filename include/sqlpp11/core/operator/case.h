#pragma once

/*
 * Copyright (c) 2015, Roland Bock
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

#include <sqlpp11/core/name/char_sequence.h>
#include <sqlpp11/core/detail/type_set.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename When, typename Then, typename Else>
  struct case_t : public enable_as<case_t<When, Then, Else>>, public enable_comparison<case_t<When, Then, Else>>
  {
    case_t(When when, Then then, Else else_) : _when(when), _then(then), _else(else_)
    {
    }

    case_t(const case_t&) = default;
    case_t(case_t&&) = default;
    case_t& operator=(const case_t&) = default;
    case_t& operator=(case_t&&) = default;
    ~case_t() = default;

    When _when;
    Then _then;
    Else _else;
  };

  template <typename When, typename Then, typename Else>
  struct nodes_of<case_t<When, Then, Else>>
  {
    using type = ::sqlpp::detail::type_vector<When, Then, Else>;
  };

  template <typename When, typename Then, typename Else>
  struct value_type_of<case_t<When, Then, Else>>
      : public std::conditional<can_be_null<When>::value or can_be_null<Then>::value or can_be_null<Else>::value,
                                force_optional_t<value_type_of_t<Then>>,
                                value_type_of_t<Then>>
  {
  };

  template <typename When, typename Then, typename Else>
  struct requires_parentheses<case_t<When, Then, Else>> : public std::true_type{};

  template <typename When, typename Then>
  class case_then_t
  {
  public:
    case_then_t(When when, Then then) : _when(when), _then(then)
    {
    }

    case_then_t(const case_then_t&) = default;
    case_then_t(case_then_t&&) = default;
    case_then_t& operator=(const case_then_t&) = default;
    case_then_t& operator=(case_then_t&&) = default;
    ~case_then_t() = default;

    template <typename Else, typename = sqlpp::enable_if_t<has_value_type<Else>::value>>
    auto else_(Else else_) -> case_t<When, Then, Else>
    {
      SQLPP_STATIC_ASSERT((values_are_comparable<Then, Else>::value),
                          "argument of then() and else() are not of the same type");

      return case_t<When, Then, Else>{_when, _then, else_};
    }

    auto else_(sqlpp::nullopt_t) -> case_t<When, Then, sqlpp::nullopt_t>
    {
      return case_t<When, Then, sqlpp::nullopt_t>{_when, _then, sqlpp::nullopt};
    }

  private:
    When _when;
    Then _then;
  };

  template <typename When>
  class case_when_t
  {
  public:
    case_when_t(When when) : _when(when)
    {
    }

    case_when_t(const case_when_t&) = default;
    case_when_t(case_when_t&&) = default;
    case_when_t& operator=(const case_when_t&) = default;
    case_when_t& operator=(case_when_t&&) = default;
    ~case_when_t() = default;

    template <typename Then, typename = sqlpp::enable_if_t<has_value_type<Then>::value>>
    auto then(Then t) -> case_then_t<When, Then>
    {
      return case_then_t<When, Then>{_when, std::move(t)};
    }

  private:
    When _when;
  };

  template <typename Context, typename When, typename Then, typename Else>
  auto to_sql_string(Context& context, const case_t<When, Then, Else>& t) -> std::string
  {
    // Note: Temporary required to enforce parameter ordering.
    auto ret_val = "CASE WHEN "+  operand_to_sql_string(context, t._when);
    ret_val += " THEN " + operand_to_sql_string(context, t._then);
    return ret_val + " ELSE " + operand_to_sql_string(context, t._else) + " END";
  }

  template <typename When, typename = sqlpp::enable_if_t<is_boolean<When>::value>>
  auto case_when(When when) -> case_when_t<When>
  {
    return case_when_t<When>{std::move(when)};
  }
}  // namespace sqlpp
