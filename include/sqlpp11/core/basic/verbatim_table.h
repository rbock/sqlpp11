#pragma once

/*
 * Copyright (c) 2013-2016, Roland Bock
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

#include <utility>

#include <sqlpp11/core/basic/enable_join.h>
#include <sqlpp11/core/type_traits.h>

namespace sqlpp
{
  template <typename NameTagProvider>
  struct verbatim_table_as_t : public enable_join<verbatim_table_as_t<NameTagProvider>>
  {
    verbatim_table_as_t(std::string representation) : _representation(std::move(representation))
    {
    }

    verbatim_table_as_t(const verbatim_table_as_t& rhs) = default;
    verbatim_table_as_t(verbatim_table_as_t&& rhs) = default;
    verbatim_table_as_t& operator=(const verbatim_table_as_t& rhs) = default;
    verbatim_table_as_t& operator=(verbatim_table_as_t&& rhs) = default;
    ~verbatim_table_as_t() = default;

    std::string _representation;
  };

  template <typename NameTagProvider>
  struct is_table<verbatim_table_as_t<NameTagProvider>> : std::true_type
  {
  };

  template <typename Context, typename NameTagProvider>
  auto to_sql_string(Context& context, const verbatim_table_as_t<NameTagProvider>& t) -> std::string
  {
    return t._representation + " AS " + name_to_sql_string(context, name_tag_of_t<NameTagProvider>::name);
  }

  struct verbatim_table_t: public enable_join<verbatim_table_t>
  {
    verbatim_table_t(std::string representation) : _representation(std::move(representation))
    {
    }

    verbatim_table_t(const verbatim_table_t& rhs) = default;
    verbatim_table_t(verbatim_table_t&& rhs) = default;
    verbatim_table_t& operator=(const verbatim_table_t& rhs) = default;
    verbatim_table_t& operator=(verbatim_table_t&& rhs) = default;
    ~verbatim_table_t() = default;

    template <typename NameTagProvider>
    verbatim_table_as_t<NameTagProvider> as(const NameTagProvider& /*unused*/) const
    {
      return {_representation};
    }

    std::string _representation;
  };

  template <>
  struct is_table<verbatim_table_t> : std::true_type
  {
  };

  template <typename Context>
  auto to_sql_string(Context& , const verbatim_table_t& t) -> std::string
  {
    return t._representation;
  }

  inline verbatim_table_t verbatim_table(std::string name)
  {
    return {std::move(name)};
  }
}  // namespace sqlpp