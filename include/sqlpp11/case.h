/*
 * Copyright (c) 2015-2015, Roland Bock
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

#ifndef SQLPP_CASE_H
#define SQLPP_CASE_H

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/boolean.h>
#include <sqlpp11/detail/type_set.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
  SQLPP_PORTABLE_STATIC_ASSERT(assert_case_else_expression_t, "argument is not a value expression in else()");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_case_then_else_same_type_t,
                               "argument of then() and else() are not of the same type");

  template <typename Then, typename Else>
  using check_case_else_t = static_combined_check_t<
      static_check_t<is_expression_t<wrap_operand_t<Else>>::value, assert_case_else_expression_t>,
      static_check_t<logic::any_t<is_sql_null_t<Then>::value,
                                  is_sql_null_t<wrap_operand_t<Else>>::value,
                                  std::is_same<value_type_of<Then>, value_type_of<wrap_operand_t<Else>>>::value>::value,
                     assert_case_then_else_same_type_t>>;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_case_then_expression_t, "argument is not a value expression in then()");
  template <typename Then>
  using check_case_then_t =
      static_check_t<logic::all_t<is_expression_t<wrap_operand_t<Then>>::value>::value, assert_case_then_expression_t>;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_case_when_boolean_expression_t,
                               "argument is not a boolean expression in case_when()");
  template <typename When>
  using check_case_when_t = static_check_t<
      logic::all_t<is_boolean_t<wrap_operand_t<When>>::value, is_expression_t<wrap_operand_t<When>>::value>::value,
      assert_case_when_boolean_expression_t>;

  template <typename When, typename Then, typename Else>
  struct case_t
      : public expression_operators<
            case_t<When, Then, Else>,
            typename std::conditional<is_sql_null_t<Then>::value, value_type_of<Else>, value_type_of<Then>>::type>,
        public alias_operators<case_t<When, Then, Else>>
  {
    using _traits = make_traits<value_type_of<Then>, tag::is_expression>;
    using _nodes = detail::type_vector<When, Then, Else>;

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

  template <typename When, typename Then>
  class case_then_t
  {
    template <typename Else>
    auto _else_impl(consistent_t, Else else_) -> case_t<When, Then, Else>
    {
      return {_when, _then, else_};
    }

    template <typename Check, typename Else>
    auto _else_impl(Check, Else else_) -> inconsistent<Check>;

  public:
    case_then_t(When when, Then then) : _when(when), _then(then)
    {
    }

    case_then_t(const case_then_t&) = default;
    case_then_t(case_then_t&&) = default;
    case_then_t& operator=(const case_then_t&) = default;
    case_then_t& operator=(case_then_t&&) = default;
    ~case_then_t() = default;

    template <typename Else>
    auto else_(Else else_) -> decltype(this->_else_impl(check_case_else_t<Then, Else>{}, else_))
    {
      return _else_impl(check_case_else_t<Then, Else>{}, else_);
    }

  private:
    When _when;
    Then _then;
  };

  template <typename When>
  class case_when_t
  {
    template <typename Then>
    auto _then_impl(consistent_t, Then t) -> case_then_t<When, wrap_operand_t<Then>>
    {
      return {_when, t};
    }

    template <typename Check, typename Then>
    auto _then_impl(Check, Then t) -> inconsistent<Check>;

  public:
    case_when_t(When when) : _when(when)
    {
    }

    case_when_t(const case_when_t&) = default;
    case_when_t(case_when_t&&) = default;
    case_when_t& operator=(const case_when_t&) = default;
    case_when_t& operator=(case_when_t&&) = default;
    ~case_when_t() = default;

    template <typename Then>
    auto then(Then t) -> decltype(this->_then_impl(check_case_then_t<Then>{}, t))
    {
      return _then_impl(check_case_then_t<Then>{}, t);
    }

  private:
    When _when;
  };

  template <typename Context, typename When, typename Then, typename Else>
  struct serializer_t<Context, case_t<When, Then, Else>>
  {
    using _serialize_check = serialize_check_of<When, Then, Else>;
    using T = case_t<When, Then, Else>;

    static Context& _(const T& t, Context& context)
    {
      context << "(CASE WHEN ";
      serialize(t._when, context);
      context << " THEN ";
      serialize(t._then, context);
      context << " ELSE ";
      serialize(t._else, context);
      context << " END)";
      return context;
    }
  };

  namespace detail
  {
    template <typename When>
    auto case_when_impl(consistent_t, When when) -> case_when_t<wrap_operand_t<When>>
    {
      return {when};
    }

    template <typename Check, typename When>
    auto case_when_impl(Check, When when) -> inconsistent<Check>;
  }

  template <typename When>
  auto case_when(When when) -> decltype(detail::case_when_impl(check_case_when_t<When>{}, when))
  {
    return detail::case_when_impl(check_case_when_t<When>{}, when);
  }
}

#endif
