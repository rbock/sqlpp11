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

#ifndef SQLPP_ASSIGNMENT_H
#define SQLPP_ASSIGNMENT_H

#include <sqlpp11/default_value.h>
#include <sqlpp11/null.h>
#include <sqlpp11/tvin.h>
#include <sqlpp11/rhs_wrap.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/simple_column.h>

namespace sqlpp
{
  template <typename Lhs, typename Rhs>
  struct assignment_t
  {
    using _traits = make_traits<no_value_t, tag::is_assignment>;
    using _lhs_t = Lhs;
    using _rhs_t = rhs_wrap_t<allow_tvin_t<Rhs>, trivial_value_is_null_t<_lhs_t>::value>;
    using _nodes = detail::type_vector<_lhs_t, _rhs_t>;

    static_assert(can_be_null_t<_lhs_t>::value ? true
                                               : not(std::is_same<_rhs_t, null_t>::value or is_tvin_t<_rhs_t>::value),
                  "column must not be null");

    assignment_t(_lhs_t lhs, _rhs_t rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    assignment_t(const assignment_t&) = default;
    assignment_t(assignment_t&&) = default;
    assignment_t& operator=(const assignment_t&) = default;
    assignment_t& operator=(assignment_t&&) = default;
    ~assignment_t() = default;

    _lhs_t _lhs;
    _rhs_t _rhs;
  };

  template <typename Context, typename Lhs, typename Rhs>
  struct serializer_t<Context, assignment_t<Lhs, Rhs>>
  {
    using T = assignment_t<Lhs, Rhs>;
    using _serialize_check = serialize_check_of<Context, typename T::_lhs_t, typename T::_rhs_t>;

    static Context& _(const T& t, Context& context)
    {
      serialize(simple_column(t._lhs), context);
      context << "=";
      serialize_operand(t._rhs, context);
      return context;
    }
  };
}

#endif
