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

namespace sqlpp
{
  struct no_name_t
  {
  };

  struct name_tag_base{}; // Used by SQLPP_ALIAS_PROVIDER and ddl2cpp

  template <typename T, bool IsNameTag>
  struct name_tag_of_impl
  {
    using type = no_name_t;
  };
  template <typename T>
  struct name_tag_of_impl<T, true>
  {
    using type = typename T::_alias_t;
  };

  template <typename T>
  struct name_tag_of
  {
    using type = typename name_tag_of_impl<T, std::is_base_of<name_tag_base, T>::value>::type;
  };

  template <typename T>
  using name_tag_of_t = typename name_tag_of<T>::type;

#warning: rename to has_name_tag
  // Override this for other classes like columns or tables.
  template<typename T>
  struct has_name : public std::integral_constant<bool, not std::is_same<name_tag_of_t<T>, no_name_t>::value> {};

}  // namespace sqlpp
