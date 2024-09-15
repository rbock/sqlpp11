#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#include <tuple>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/to_sql_string.h>
#include <sqlpp11/core/compat/utility.h>

namespace sqlpp
{
  struct tuple_operand
  {
    template <typename Context, typename T>
    auto operator()(Context& context, const T& t, size_t index) const -> std::string
    {
      const auto prefix = index ? std::string{separator} : std::string{};
      return prefix + operand_to_sql_string(context, t);
    }

    sqlpp::string_view separator;
  };

  // Used to serialize tuple that should ignore dynamic elements.
  struct tuple_operand_no_dynamic
  {
    template <typename Context, typename T>
    auto operator()(Context& context, const T& t, size_t ) const -> std::string
    {
      const auto prefix = need_prefix ? std::string{separator} : std::string{};
      need_prefix = true;
      return prefix + operand_to_sql_string(context, t);
    }

    template <typename Context, typename T>
    auto operator()(Context& context, const sqlpp::dynamic_t<T>& t, size_t index) const -> std::string
    {
      if (t._condition)
      {
        return operator()(context, t._expr, index);
      }
      return "";
    }

    sqlpp::string_view separator;
    mutable bool need_prefix = false;
  };

  // Used to serialize select columns.
  // In particular, it serializes unselected dynamic columns as "NULL AS <name>".
  struct tuple_operand_select_column
  {
    template <typename Context, typename T>
    auto operator()(Context& context, const T& t, size_t index) const -> std::string
    {
      const auto prefix = index ? std::string{separator} : std::string{};
      return prefix + operand_to_sql_string(context, t);
    }

    template <typename Context, typename T, typename NameProvider>
    auto operator()(Context& context,
                    const expression_as<sqlpp::dynamic_t<T>, NameProvider>& t,
                    size_t index) const -> std::string
    {
      if (t._expression._condition)
      {
        return operator()(context, as(t._expression._expr, NameProvider{}), index);
      }
      return operator()(context, as(sqlpp::nullopt, NameProvider{}), index);
    }

    template <typename Context, typename T>
    auto operator()(Context& context, const sqlpp::dynamic_t<T>& t, size_t index) const -> std::string
    {
      if (t._condition)
      {
        return operator()(context, t._expr, index);
      }
      static_assert(has_name<T>::value, "select columns have to have a name");
      return operator()(context, as(sqlpp::nullopt, t._expr), index);
    }

    sqlpp::string_view separator;
  };

  struct tuple_clause
  {
    template <typename Context, typename T>
    auto operator()(Context& context, const T& t, size_t index) const -> std::string
    {
      const auto prefix = index ? std::string{separator} : std::string{};
      return prefix + to_sql_string(context, t);
    }

    sqlpp::string_view separator;
  };

  template <typename Context, typename Tuple, typename Strategy, size_t... Is>
  auto tuple_to_sql_string_impl(Context& context,
                                const Tuple& t,
                                const Strategy& strategy,
                                const ::sqlpp::index_sequence<Is...>&
                                /*unused*/) -> std::string
  {
    // Note: A braced-init-list does guarantee the order of evaluation according to 12.6.1 [class.explicit.init]
    // paragraph 2 and 8.5.4 [dcl.init.list] paragraph 4.
    // See for example: "http://en.cppreference.com/w/cpp/utility/integer_sequence"
    // See also: "http://stackoverflow.com/questions/6245735/pretty-print-stdtuple/6245777#6245777"
    // Beware of gcc-bug: "http://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253", otherwise an empty swallow struct could
    // be used.
    auto result = std::string{};
    using swallow = int[];
    (void)swallow{0,  // workaround against -Wpedantic GCC warning "zero-size array 'int [0]'"
                  (result += strategy(context, std::get<Is>(t), Is), 0)...};
    return result;
  }

  template <typename Context, typename Tuple, typename Strategy>
  auto tuple_to_sql_string(Context& context, const Tuple& t, const Strategy& strategy) -> std::string
  {
    return tuple_to_sql_string_impl(context, t, strategy,
                                ::sqlpp::make_index_sequence<std::tuple_size<Tuple>::value>{});
  }

}  // namespace sqlpp
