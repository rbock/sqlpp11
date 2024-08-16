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

#include <sqlpp11/core/compat/optional.h>

namespace sqlpp
{
  template <typename T>
  struct is_optional : public std::false_type
  {
  };

  template <typename T>
  struct is_optional<::sqlpp::optional<T>> : public std::true_type
  {
  };

  template <>
  struct is_optional<::sqlpp::nullopt_t> : public std::true_type
  {
  };

  template <typename T>
  struct remove_optional
  {
    using type = T;
  };

  template <typename T>
  struct remove_optional<::sqlpp::optional<T>>
  {
    using type = T;
  };

  template <typename T>
  using remove_optional_t = typename remove_optional<T>::type;

  template <typename T>
  struct force_optional
  {
    using type = ::sqlpp::optional<T>;
  };

  template <typename T>
  struct force_optional<::sqlpp::optional<T>>
  {
    using type = ::sqlpp::optional<T>;
  };

  template <typename T>
  using force_optional_t = typename force_optional<T>::type;


}
