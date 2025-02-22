#pragma once

/*
 * Copyright (c) 2025, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
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

#include <sqlpp23/core/logic.h>
#include <sqlpp23/core/operator/enable_as.h>
#include <sqlpp23/core/operator/enable_comparison.h>
#include <sqlpp23/core/to_sql_string.h>
#include <sqlpp23/core/type_traits.h>

namespace sqlpp {
template <typename... Args>
struct concat_t : public enable_comparison<concat_t<Args...>>,
                  public enable_as<concat_t<Args...>> {
  concat_t(const Args... args) : _args(std::move(args)...) {}

  concat_t(const concat_t &) = default;
  concat_t(concat_t &&) = default;
  concat_t &operator=(const concat_t &) = default;
  concat_t &operator=(concat_t &&) = default;
  ~concat_t() = default;

  std::tuple<Args...> _args;
};

template <typename... Args> struct value_type_of<concat_t<Args...>> {
  using type = std::conditional_t<
      logic::any<is_optional<value_type_of_t<Args>>::value...>::value,
      std::optional<sqlpp::text>, sqlpp::text>;
};

template <typename... Args> struct nodes_of<concat_t<Args...>> {
  using type = detail::type_vector<Args...>;
};

template <typename Context, typename... Args>
auto to_sql_string(Context &context, const concat_t<Args...> &t)
    -> std::string {
  return "CONCAT(" +
         tuple_to_sql_string(context, t._args, tuple_operand{", "}) + ")";
}

template <typename... Args>
using check_concat_args =
    std::enable_if_t<logic::all<is_text<Args>::value...>::value>;

template <typename... Args,
          typename = check_concat_args<remove_dynamic_t<Args>...>>
auto concat(Args... args) -> concat_t<Args...> {
  SQLPP_STATIC_ASSERT(sizeof...(Args) > 0,
                      "at least one argument required in concat()")
  return {std::move(args)...};
}

} // namespace sqlpp
