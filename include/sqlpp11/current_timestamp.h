#pragma once

/*
 * Copyright (c) 2013-2015, Roland Bock
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

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/time_point/data_type.h>

namespace sqlpp
{
  struct current_timestamp_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "current_timestamp_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T current_timestamp;
        T& operator()()
        {
          return current_timestamp;
        }
        const T& operator()() const
        {
          return current_timestamp;
        }
      };
    };
  };

  struct current_timestamp_t : public expression_operators<current_timestamp_t, time_point>, public alias_operators<current_timestamp_t>
  {
    using _traits = make_traits<time_point, tag::is_expression, tag::is_selectable>;

    using _nodes = detail::type_vector<>;
    using _can_be_null = std::false_type;

    using _auto_alias_t = current_timestamp_alias_t;

    constexpr current_timestamp_t()
    {
    }

    current_timestamp_t(const current_timestamp_t&) = default;
    current_timestamp_t(current_timestamp_t&&) = default;
    current_timestamp_t& operator=(const current_timestamp_t&) = default;
    current_timestamp_t& operator=(current_timestamp_t&&) = default;
    ~current_timestamp_t() = default;
  };

  template <typename Context>
  Context& serialize(const current_timestamp_t&, Context& context)
  {
    context << "CURRENT_TIMESTAMP";
    return context;
  }

  constexpr current_timestamp_t current_timestamp{};
}  // namespace sqlpp
